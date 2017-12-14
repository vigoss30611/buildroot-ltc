#ifndef __DRAM_A9_H__
#define __DRAM_A9_H__

/* uMCTL Registers */
#define DDR_UMCTL_PCFG_0                 DWC_DDR_UMCTL_BASE_ADDR+0x400    //Port 0   Configuration Register
#define DDR_UMCTL_PCFG_1                 DWC_DDR_UMCTL_BASE_ADDR+0x404    //Port 1   Configuration Register
#define DDR_UMCTL_PCFG_2                 DWC_DDR_UMCTL_BASE_ADDR+0x408    //Port 2   Configuration Register
#define DDR_UMCTL_PCFG_3                 DWC_DDR_UMCTL_BASE_ADDR+0x40c    //Port 3   Configuration Register
#define DDR_UMCTL_PCFG_4                 DWC_DDR_UMCTL_BASE_ADDR+0x410    //Port 4   Configuration Register
#define DDR_UMCTL_PCFG_5                 DWC_DDR_UMCTL_BASE_ADDR+0x414    //Port 5   Configuration Register
#define DDR_UMCTL_PCFG_6                 DWC_DDR_UMCTL_BASE_ADDR+0x418    //Port 6   Configuration Register
#define DDR_UMCTL_PCFG_7                 DWC_DDR_UMCTL_BASE_ADDR+0x41c    //Port 7   Configuration Register
#define DDR_UMCTL_PCFG_8                 DWC_DDR_UMCTL_BASE_ADDR+0x420    //Port 8   Configuration Register
#define DDR_UMCTL_PCFG_9                 DWC_DDR_UMCTL_BASE_ADDR+0x424    //Port 9   Configuration Register
#define DDR_UMCTL_PCFG_10                DWC_DDR_UMCTL_BASE_ADDR+0x428    //Port 10  Configuration Register
#define DDR_UMCTL_PCFG_11                DWC_DDR_UMCTL_BASE_ADDR+0x42c    //Port 11  Configuration Register
#define DDR_UMCTL_PCFG_12                DWC_DDR_UMCTL_BASE_ADDR+0x430    //Port 12  Configuration Register
#define DDR_UMCTL_PCFG_13                DWC_DDR_UMCTL_BASE_ADDR+0x434    //Port 13  Configuration Register
#define DDR_UMCTL_PCFG_14                DWC_DDR_UMCTL_BASE_ADDR+0x438    //Port 14  Configuration Register
#define DDR_UMCTL_PCFG_15                DWC_DDR_UMCTL_BASE_ADDR+0x43c    //Port 15  Configuration Register
#define DDR_UMCTL_PCFG_16                DWC_DDR_UMCTL_BASE_ADDR+0x440    //Port 16  Configuration Register
#define DDR_UMCTL_PCFG_17                DWC_DDR_UMCTL_BASE_ADDR+0x444    //Port 17  Configuration Register
#define DDR_UMCTL_PCFG_18                DWC_DDR_UMCTL_BASE_ADDR+0x448    //Port 18  Configuration Register
#define DDR_UMCTL_PCFG_19                DWC_DDR_UMCTL_BASE_ADDR+0x44c    //Port 19  Configuration Register
#define DDR_UMCTL_PCFG_20                DWC_DDR_UMCTL_BASE_ADDR+0x450    //Port 20  Configuration Register
#define DDR_UMCTL_PCFG_21                DWC_DDR_UMCTL_BASE_ADDR+0x454    //Port 21  Configuration Register
#define DDR_UMCTL_PCFG_22                DWC_DDR_UMCTL_BASE_ADDR+0x458    //Port 22  Configuration Register
#define DDR_UMCTL_PCFG_23                DWC_DDR_UMCTL_BASE_ADDR+0x45c    //Port 23  Configuration Register
#define DDR_UMCTL_PCFG_24                DWC_DDR_UMCTL_BASE_ADDR+0x460    //Port 24  Configuration Register
#define DDR_UMCTL_PCFG_25                DWC_DDR_UMCTL_BASE_ADDR+0x464    //Port 25  Configuration Register
#define DDR_UMCTL_PCFG_26                DWC_DDR_UMCTL_BASE_ADDR+0x468    //Port 26  Configuration Register
#define DDR_UMCTL_PCFG_27                DWC_DDR_UMCTL_BASE_ADDR+0x46c    //Port 27  Configuration Register
#define DDR_UMCTL_PCFG_28                DWC_DDR_UMCTL_BASE_ADDR+0x470    //Port 28  Configuration Register
#define DDR_UMCTL_PCFG_29                DWC_DDR_UMCTL_BASE_ADDR+0x474    //Port 29  Configuration Register
#define DDR_UMCTL_PCFG_30                DWC_DDR_UMCTL_BASE_ADDR+0x478    //Port 30  Configuration Register
#define DDR_UMCTL_PCFG_31                DWC_DDR_UMCTL_BASE_ADDR+0x47c    //Port 31  Configuration Register
#define DDR_UMCTL_CCFG                   DWC_DDR_UMCTL_BASE_ADDR+0x480    //Controller Configuration Register
#define DDR_UMCTL_DCFG                   DWC_DDR_UMCTL_BASE_ADDR+0x484    //DRAM Configuration Register
#define DDR_UMCTL_CSTAT                  DWC_DDR_UMCTL_BASE_ADDR+0x488    //Controller Status Register
#define DDR_UMCTL_CCFG1                  DWC_DDR_UMCTL_BASE_ADDR+0x48c    //Controller Status Register 1     add

/* _DDR_PUBL Registers  PHY */
#define DDR_PHY_RIDR                     DWC_DDR_PUBL_BASE_ADDR+0X000     //Revision Identification Register
#define DDR_PHY_PIR                      DWC_DDR_PUBL_BASE_ADDR+0X004     //PHY Initialization Register
#define DDR_PHY_PGCR                     DWC_DDR_PUBL_BASE_ADDR+0X008     //PHY General Configuration Register
#define DDR_PHY_PGSR                     DWC_DDR_PUBL_BASE_ADDR+0X00c     //PHY General Status Register
#define DDR_PHY_DLLGCR                   DWC_DDR_PUBL_BASE_ADDR+0X010     //DLL General Control Register
#define DDR_PHY_ACDLLCR                  DWC_DDR_PUBL_BASE_ADDR+0X014     //AC DLL Control Register
#define DDR_PHY_PTR0                     DWC_DDR_PUBL_BASE_ADDR+0X018     //PHY Timing Register 0
#define DDR_PHY_PTR1                     DWC_DDR_PUBL_BASE_ADDR+0X01c     //PHY Timing Register 1
#define DDR_PHY_PTR2                     DWC_DDR_PUBL_BASE_ADDR+0X020     //PHY Timing Register 2
#define DDR_PHY_ACIOCR                   DWC_DDR_PUBL_BASE_ADDR+0X024     //AC I/O Configuration Register
#define DDR_PHY_DXCCR                    DWC_DDR_PUBL_BASE_ADDR+0X028     //DATX8 Common Configuratio Register
#define DDR_PHY_DSGCR                    DWC_DDR_PUBL_BASE_ADDR+0X02c     //DDR System General Configuration Register
#define DDR_PHY_DCR                      DWC_DDR_PUBL_BASE_ADDR+0X030     //DRAM Configuration Register
#define DDR_PHY_DTPR0                    DWC_DDR_PUBL_BASE_ADDR+0X034     //DRAM Timing Parameters Register 0
#define DDR_PHY_DTPR1                    DWC_DDR_PUBL_BASE_ADDR+0X038     //DRAM Timing Parameters Register 1
#define DDR_PHY_DTPR2                    DWC_DDR_PUBL_BASE_ADDR+0X03c     //DRAM Timing Parameters Register 2
#define DDR_PHY_MR0                      DWC_DDR_PUBL_BASE_ADDR+0X040     //Mode Register 0
#define DDR_PHY_MR1                      DWC_DDR_PUBL_BASE_ADDR+0X044     //Mode Register 1
#define DDR_PHY_MR2                      DWC_DDR_PUBL_BASE_ADDR+0X048     //Mode Register 2
#define DDR_PHY_MR3                      DWC_DDR_PUBL_BASE_ADDR+0X04c     //Mode Register 3
#define DDR_PHY_ODTCR                    DWC_DDR_PUBL_BASE_ADDR+0X050     //ODT Configuration Register
#define DDR_PHY_DTAR                     DWC_DDR_PUBL_BASE_ADDR+0X054     //Data Training Address Register
#define DDR_PHY_DTDR0                    DWC_DDR_PUBL_BASE_ADDR+0X058     //Data Training Data Register 0
#define DDR_PHY_DTDR1                    DWC_DDR_PUBL_BASE_ADDR+0X05c     //Data Training Data Register 1
#define DDR_PHY_DCUAR                    DWC_DDR_PUBL_BASE_ADDR+0X0c0     //DCU Address Register
#define DDR_PHY_DCUDR                    DWC_DDR_PUBL_BASE_ADDR+0X0c4     //DCU Data Register
#define DDR_PHY_DCURR                    DWC_DDR_PUBL_BASE_ADDR+0X0c8     //DCU Run Register
#define DDR_PHY_DCULR                    DWC_DDR_PUBL_BASE_ADDR+0X0cc     //DCU Loop Register
#define DDR_PHY_DCUGCR                   DWC_DDR_PUBL_BASE_ADDR+0X0d0     //DCU General Configuration Register
#define DDR_PHY_DCUTPR                   DWC_DDR_PUBL_BASE_ADDR+0X0d4     //DCU Timing Rarameters Register
#define DDR_PHY_DCUSR0                   DWC_DDR_PUBL_BASE_ADDR+0X0d8     //DCU Status Register 0
#define DDR_PHY_DCUSR1                   DWC_DDR_PUBL_BASE_ADDR+0X0dc     //DCU Status Register 1
#define DDR_PHY_BISTRR                   DWC_DDR_PUBL_BASE_ADDR+0X100     //BIST Run Register
#define DDR_PHY_BISTMSKR0                DWC_DDR_PUBL_BASE_ADDR+0X104     //BIST Mask Register 0
#define DDR_PHY_BISTMSKR1                DWC_DDR_PUBL_BASE_ADDR+0X108     //BIST Mask Register 1
#define DDR_PHY_BISTWCR                  DWC_DDR_PUBL_BASE_ADDR+0X10c     //BIST Word Count  Register
#define DDR_PHY_BISTLSR                  DWC_DDR_PUBL_BASE_ADDR+0X110     //BIST LFSR Seed Register
#define DDR_PHY_BISTAR0                  DWC_DDR_PUBL_BASE_ADDR+0X114     //BIST Address Register 0
#define DDR_PHY_BISTAR1                  DWC_DDR_PUBL_BASE_ADDR+0X118     //BIST Address Register 1
#define DDR_PHY_BISTAR2                  DWC_DDR_PUBL_BASE_ADDR+0X11c     //BIST Address Register 2
#define DDR_PHY_BISTUDPR                 DWC_DDR_PUBL_BASE_ADDR+0X120     //BIST User Data Pattern Register
#define DDR_PHY_BISTGSR                  DWC_DDR_PUBL_BASE_ADDR+0X124     //BIST General Status Register
#define DDR_PHY_BISTWER                  DWC_DDR_PUBL_BASE_ADDR+0X128     //BIST Word Error Register
#define DDR_PHY_BISTBER0                 DWC_DDR_PUBL_BASE_ADDR+0X12c     //BIST Bit Error Register 0
#define DDR_PHY_BISTBER1                 DWC_DDR_PUBL_BASE_ADDR+0X130     //BIST Bit Error Register 1
#define DDR_PHY_BISTBER2                 DWC_DDR_PUBL_BASE_ADDR+0X134     //BIST Bit Error Register 2
#define DDR_PHY_BISTWCSR                 DWC_DDR_PUBL_BASE_ADDR+0X138     //BIST Wor Count Status Register
#define DDR_PHY_BISTFWR0                 DWC_DDR_PUBL_BASE_ADDR+0X13c     //BIST Fail Word Register 0
#define DDR_PHY_BISTFWR1                 DWC_DDR_PUBL_BASE_ADDR+0X140     //BIST Fail Word Register 1
#define DDR_PHY_ZQ0CR0                   DWC_DDR_PUBL_BASE_ADDR+0X180     //ZQ 0 Impedance Control Register 0
#define DDR_PHY_ZQ0CR1                   DWC_DDR_PUBL_BASE_ADDR+0X184     //ZQ 0 Impedance Control Register 1
#define DDR_PHY_ZQ0SR0                   DWC_DDR_PUBL_BASE_ADDR+0X188     //ZQ 0 Impedance Status Register 0
#define DDR_PHY_ZQ0SR1                   DWC_DDR_PUBL_BASE_ADDR+0X18c     //ZQ 0 Impedance Status Register 1
#define DDR_PHY_ZQ1CR0                   DWC_DDR_PUBL_BASE_ADDR+0X190     //ZQ 1 Impedance Control Register 0
#define DDR_PHY_ZQ1CR1                   DWC_DDR_PUBL_BASE_ADDR+0X194     //ZQ 1 Impedance Control Register 1
#define DDR_PHY_ZQ1SR0                   DWC_DDR_PUBL_BASE_ADDR+0X198     //ZQ 1 Impedance Status Register 0
#define DDR_PHY_ZQ1SR1                   DWC_DDR_PUBL_BASE_ADDR+0X19c     //ZQ 1 Impedance Status Register 1
#define DDR_PHY_ZQ2CR0                   DWC_DDR_PUBL_BASE_ADDR+0X1a0     //ZQ 2 Impedance Control Register 0
#define DDR_PHY_ZQ2CR1                   DWC_DDR_PUBL_BASE_ADDR+0X1a4     //ZQ 2 Impedance Control Register 1
#define DDR_PHY_ZQ2SR0                   DWC_DDR_PUBL_BASE_ADDR+0X1a8     //ZQ 2 Impedance Status Register 0
#define DDR_PHY_ZQ2SR1                   DWC_DDR_PUBL_BASE_ADDR+0X1ac     //ZQ 2 Impedance Status Register 1
#define DDR_PHY_ZQ3CR0                   DWC_DDR_PUBL_BASE_ADDR+0X1b0     //ZQ 3 Impedance Control Register 0
#define DDR_PHY_ZQ3CR1                   DWC_DDR_PUBL_BASE_ADDR+0X1b4     //ZQ 3 Impedance Control Register 1
#define DDR_PHY_ZQ3SR0                   DWC_DDR_PUBL_BASE_ADDR+0X1b8     //ZQ 3 Impedance Status Register 0
#define DDR_PHY_ZQ3SR1                   DWC_DDR_PUBL_BASE_ADDR+0X1bc     //ZQ 3 Impedance Status Register 1
#define DDR_PHY_DX0GCR                   DWC_DDR_PUBL_BASE_ADDR+0X1c0     //DATX8 0 General Configuration Register
#define DDR_PHY_DX0GSR0                  DWC_DDR_PUBL_BASE_ADDR+0X1c4     //DATX8 0 General Status Register 0
#define DDR_PHY_DX0GSR1                  DWC_DDR_PUBL_BASE_ADDR+0X1c8     //DATX8 0 General Status Register 1
#define DDR_PHY_DX0DLLCR                 DWC_DDR_PUBL_BASE_ADDR+0X1cc     //DATX8 0 DLL Control Register
#define DDR_PHY_DX0DQTR                  DWC_DDR_PUBL_BASE_ADDR+0X1d0     //DATX8 0 DQ Timing Register
#define DDR_PHY_DX0DQSTR                 DWC_DDR_PUBL_BASE_ADDR+0X1d4     //DATX8 0 DQS Timing Register
#define DDR_PHY_DX1GCR                   DWC_DDR_PUBL_BASE_ADDR+0X200     //DATX8 1 General Configuration Register
#define DDR_PHY_DX1GSR0                  DWC_DDR_PUBL_BASE_ADDR+0X204     //DATX8 1 General Status Register 0
#define DDR_PHY_DX1GSR1                  DWC_DDR_PUBL_BASE_ADDR+0X208     //DATX8 1 General Status Register 1
#define DDR_PHY_DX1DLLCR                 DWC_DDR_PUBL_BASE_ADDR+0X20c     //DATX8 1 DLL Control Register
#define DDR_PHY_DX1DQTR                  DWC_DDR_PUBL_BASE_ADDR+0X210     //DATX8 1 DQ Timing Register
#define DDR_PHY_DX1DQSTR                 DWC_DDR_PUBL_BASE_ADDR+0X214     //DATX8 1 DQS Timing Register
#define DDR_PHY_DX2GCR                   DWC_DDR_PUBL_BASE_ADDR+0X240     //DATX8 2 General Configuration Register
#define DDR_PHY_DX2GSR0                  DWC_DDR_PUBL_BASE_ADDR+0X244     //DATX8 2 General Status Register 0
#define DDR_PHY_DX2GSR1                  DWC_DDR_PUBL_BASE_ADDR+0X248     //DATX8 2 General Status Register 1
#define DDR_PHY_DX2DLLCR                 DWC_DDR_PUBL_BASE_ADDR+0X24c     //DATX8 2 DLL Control Register
#define DDR_PHY_DX2DQTR                  DWC_DDR_PUBL_BASE_ADDR+0X250     //DATX8 2 DQ Timing Register
#define DDR_PHY_DX2DQSTR                 DWC_DDR_PUBL_BASE_ADDR+0X254     //DATX8 2 DQS Timing Register
#define DDR_PHY_DX3GCR                   DWC_DDR_PUBL_BASE_ADDR+0X280     //DATX8 3 General Configuration Register
#define DDR_PHY_DX3GSR0                  DWC_DDR_PUBL_BASE_ADDR+0X284     //DATX8 3 General Status Register 0
#define DDR_PHY_DX3GSR1                  DWC_DDR_PUBL_BASE_ADDR+0X288     //DATX8 3 General Status Register 1
#define DDR_PHY_DX3DLLCR                 DWC_DDR_PUBL_BASE_ADDR+0X28c     //DATX8 3 DLL Control Register
#define DDR_PHY_DX3DQTR                  DWC_DDR_PUBL_BASE_ADDR+0X290     //DATX8 3 DQ Timing Register
#define DDR_PHY_DX3DQSTR                 DWC_DDR_PUBL_BASE_ADDR+0X294     //DATX8 3 DQS Timing Register
#define DDR_PHY_DX4GCR                   DWC_DDR_PUBL_BASE_ADDR+0X2c0     //DATX8 4 General Configuration Register
#define DDR_PHY_DX4GSR0                  DWC_DDR_PUBL_BASE_ADDR+0X2c4     //DATX8 4 General Status Register 0
#define DDR_PHY_DX4GSR1                  DWC_DDR_PUBL_BASE_ADDR+0X2c8     //DATX8 4 General Status Register 1
#define DDR_PHY_DX4DLLCR                 DWC_DDR_PUBL_BASE_ADDR+0X2cc     //DATX8 4 DLL Control Register
#define DDR_PHY_DX4DQTR                  DWC_DDR_PUBL_BASE_ADDR+0X2d0     //DATX8 4 DQ Timing Register
#define DDR_PHY_DX4DQSTR                 DWC_DDR_PUBL_BASE_ADDR+0X2d4     //DATX8 4 DQS Timing Register
#define DDR_PHY_DX5GCR                   DWC_DDR_PUBL_BASE_ADDR+0X300     //DATX8 5 General Configuration Register
#define DDR_PHY_DX5GSR0                  DWC_DDR_PUBL_BASE_ADDR+0X304     //DATX8 5 General Status Register 0
#define DDR_PHY_DX5GSR1                  DWC_DDR_PUBL_BASE_ADDR+0X308     //DATX8 5 General Status Register 1
#define DDR_PHY_DX5DLLCR                 DWC_DDR_PUBL_BASE_ADDR+0X30c     //DATX8 5 DLL Control Register
#define DDR_PHY_DX5DQTR                  DWC_DDR_PUBL_BASE_ADDR+0X310     //DATX8 5 DQ Timing Register
#define DDR_PHY_DX5DQSTR                 DWC_DDR_PUBL_BASE_ADDR+0X314     //DATX8 5 DQS Timing Register
#define DDR_PHY_DX6GCR                   DWC_DDR_PUBL_BASE_ADDR+0X340     //DATX8 6 General Configuration Register
#define DDR_PHY_DX6GSR0                  DWC_DDR_PUBL_BASE_ADDR+0X344     //DATX8 6 General Status Register 0
#define DDR_PHY_DX6GSR1                  DWC_DDR_PUBL_BASE_ADDR+0X348     //DATX8 6 General Status Register 1
#define DDR_PHY_DX6DLLCR                 DWC_DDR_PUBL_BASE_ADDR+0X34c     //DATX8 6 DLL Control Register
#define DDR_PHY_DX6DQTR                  DWC_DDR_PUBL_BASE_ADDR+0X350     //DATX8 6 DQ Timing Register
#define DDR_PHY_DX6DQSTR                 DWC_DDR_PUBL_BASE_ADDR+0X354     //DATX8 6 DQS Timing Register
#define DDR_PHY_DX7GCR                   DWC_DDR_PUBL_BASE_ADDR+0X380     //DATX8 7 General Configuration Register
#define DDR_PHY_DX7GSR0                  DWC_DDR_PUBL_BASE_ADDR+0X384     //DATX8 7 General Status Register 0
#define DDR_PHY_DX7GSR1                  DWC_DDR_PUBL_BASE_ADDR+0X388     //DATX8 7 General Status Register 1
#define DDR_PHY_DX7DLLCR                 DWC_DDR_PUBL_BASE_ADDR+0X38c     //DATX8 7 DLL Control Register
#define DDR_PHY_DX7DQTR                  DWC_DDR_PUBL_BASE_ADDR+0X390     //DATX8 7 DQ Timing Register
#define DDR_PHY_DX7DQSTR                 DWC_DDR_PUBL_BASE_ADDR+0X394     //DATX8 7 DQS Timing Register
#define DDR_PHY_DX8GCR                   DWC_DDR_PUBL_BASE_ADDR+0X3c0      //DATX8 8 General Configuration Register
#define DDR_PHY_DX8GSR0                  DWC_DDR_PUBL_BASE_ADDR+0X3c4      //DATX8 8 General Status Register 0
#define DDR_PHY_DX8GSR1                  DWC_DDR_PUBL_BASE_ADDR+0X3c8      //DATX8 8 General Status Register 1
#define DDR_PHY_DX8DLLCR                 DWC_DDR_PUBL_BASE_ADDR+0X3cc      //DATX8 8 DLL Control Register
#define DDR_PHY_DX8DQTR                  DWC_DDR_PUBL_BASE_ADDR+0X3d0      //DATX8 8 DQ Timing Register
#define DDR_PHY_DX8DQSTR                 DWC_DDR_PUBL_BASE_ADDR+0X3d4      //DATX8 8 DQS Timing Register

/* DWC_#define DDR_UMCTL2_MAP Registers */
#define DDR_UMCTL2_MSTR                   DWC_DDR_UMCTL_BASE_ADDR+0x000    //         Master Register *
#define DDR_UMCTL2_STAT                   DWC_DDR_UMCTL_BASE_ADDR+0x004    //         Operating Mode Status Register, read only
#define DDR_UMCTL2_MRCTRL0                DWC_DDR_UMCTL_BASE_ADDR+0x010    //         Mode Register Read/Write Control Register 0 *
#define DDR_UMCTL2_MRCTRL1                DWC_DDR_UMCTL_BASE_ADDR+0x014    //         Mode Register Read/Write Control Register 1 *
#define DDR_UMCTL2_MRSTAT                 DWC_DDR_UMCTL_BASE_ADDR+0x018    //         Mode Register Read/Write Status Register, read only.
#define DDR_UMCTL2_DERATEEN               DWC_DDR_UMCTL_BASE_ADDR+0x020    //         Temperature Derate Enable Register, LPDDR2
#define DDR_UMCTL2_DERATEINT              DWC_DDR_UMCTL_BASE_ADDR+0x024    //         Temperature Derate Interval Register, LPDDR2
#define DDR_UMCTL2_PWRCTL                 DWC_DDR_UMCTL_BASE_ADDR+0x030    //         Low Power Control Register, not use now
#define DDR_UMCTL2_PWRTMG                 DWC_DDR_UMCTL_BASE_ADDR+0x034    //         Low Power Timing Register
#define DDR_UMCTL2_RFSHCTL0               DWC_DDR_UMCTL_BASE_ADDR+0x050    //         Refresh Control Register 0, use default
#define DDR_UMCTL2_RFSHCTL1               DWC_DDR_UMCTL_BASE_ADDR+0x054    //         Refresh Control Register 1, RANK>=2
#define DDR_UMCTL2_RFSHCTL2               DWC_DDR_UMCTL_BASE_ADDR+0x058    //         Refresh Control Register 2, RANK=4
#define DDR_UMCTL2_RFSHCTL3               DWC_DDR_UMCTL_BASE_ADDR+0x060    //         Refresh Control Register 3, use default
#define DDR_UMCTL2_RFSHTMG                DWC_DDR_UMCTL_BASE_ADDR+0x064    //         Refresh Timing Register      *
#define DDR_UMCTL2_INIT0                  DWC_DDR_UMCTL_BASE_ADDR+0x0D0    //         SDRAM Initialization Register 0  *
#define DDR_UMCTL2_INIT1                  DWC_DDR_UMCTL_BASE_ADDR+0x0D4    //         SDRAM Initialization Register 1  *
#define DDR_UMCTL2_INIT2                  DWC_DDR_UMCTL_BASE_ADDR+0x0D8    //         SDRAM Initialization Register 2, LPDDR2
#define DDR_UMCTL2_INIT3                  DWC_DDR_UMCTL_BASE_ADDR+0x0DC    //         SDRAM Initialization Register 3
#define DDR_UMCTL2_INIT4                  DWC_DDR_UMCTL_BASE_ADDR+0x0E0    //         SDRAM Initialization Register 4
#define DDR_UMCTL2_INIT5                  DWC_DDR_UMCTL_BASE_ADDR+0x0E4    //         SDRAM Initialization Register 5, LPDDR2/DDR3 *
#define DDR_UMCTL2_DIMMCTL                DWC_DDR_UMCTL_BASE_ADDR+0x0F0    //         DIMM Control Register
#define DDR_UMCTL2_RANKCTL                DWC_DDR_UMCTL_BASE_ADDR+0x0F4    //         Rank Control Register *
#define DDR_UMCTL2_DRAMTMG0               DWC_DDR_UMCTL_BASE_ADDR+0x100    //         SDRAM Timing Register 0
#define DDR_UMCTL2_DRAMTMG1               DWC_DDR_UMCTL_BASE_ADDR+0x104    //         SDRAM Timing Register 1
#define DDR_UMCTL2_DRAMTMG2               DWC_DDR_UMCTL_BASE_ADDR+0x108    //         SDRAM Timing Register 2
#define DDR_UMCTL2_DRAMTMG3               DWC_DDR_UMCTL_BASE_ADDR+0x10C    //         SDRAM Timing Register 3
#define DDR_UMCTL2_DRAMTMG4               DWC_DDR_UMCTL_BASE_ADDR+0x110    //         SDRAM Timing Register 4
#define DDR_UMCTL2_DRAMTMG5               DWC_DDR_UMCTL_BASE_ADDR+0x114    //         SDRAM Timing Register 5
#define DDR_UMCTL2_DRAMTMG6               DWC_DDR_UMCTL_BASE_ADDR+0x118    //         SDRAM Timing Register 6
#define DDR_UMCTL2_DRAMTMG7               DWC_DDR_UMCTL_BASE_ADDR+0x11C    //         SDRAM Timing Register 7
#define DDR_UMCTL2_DRAMTMG8               DWC_DDR_UMCTL_BASE_ADDR+0x120    //         SDRAM Timing Register 8
#define DDR_UMCTL2_ZQCTL0                 DWC_DDR_UMCTL_BASE_ADDR+0x180    //         ZQ Control Register 0, LPDDR2/DDR3 *
#define DDR_UMCTL2_ZQCTL1                 DWC_DDR_UMCTL_BASE_ADDR+0x184    //         ZQ Control Register 1, LPDDR2/DDR3
#define DDR_UMCTL2_ZQCTL2                 DWC_DDR_UMCTL_BASE_ADDR+0x188    //         ZQ Control Register 2, LPDDR2
#define DDR_UMCTL2_ZQSTAT                 DWC_DDR_UMCTL_BASE_ADDR+0x18C    //         ZQ Status Register, LPDDR2
#define DDR_UMCTL2_DFITMG0                DWC_DDR_UMCTL_BASE_ADDR+0x190    //         DFI Timing Register 0  *
#define DDR_UMCTL2_DFITMG1                DWC_DDR_UMCTL_BASE_ADDR+0x194    //         DFI Timing Register 1  *
#define DDR_UMCTL2_DFIUPD0                DWC_DDR_UMCTL_BASE_ADDR+0x1A0    //         DFI Update Register 0  *
#define DDR_UMCTL2_DFIUPD1                DWC_DDR_UMCTL_BASE_ADDR+0x1A4    //         DFI Update Register 1
#define DDR_UMCTL2_DFIUPD2                DWC_DDR_UMCTL_BASE_ADDR+0x1A8    //         DFI Update Register 2
#define DDR_UMCTL2_DFIUPD3                DWC_DDR_UMCTL_BASE_ADDR+0x1AC    //         DFI Update Register 3
#define DDR_UMCTL2_DFIMISC                DWC_DDR_UMCTL_BASE_ADDR+0x1B0    //         DFI Miscellaneous Control Register
#define DDR_UMCTL2_TRAINCTL0              DWC_DDR_UMCTL_BASE_ADDR+0x1D0    //         PHY Eval Training Control Register 0
#define DDR_UMCTL2_TRAINCTL1              DWC_DDR_UMCTL_BASE_ADDR+0x1D4    //         PHY Eval Training Control Register 1
#define DDR_UMCTL2_TRAINCTL2              DWC_DDR_UMCTL_BASE_ADDR+0x1D8    //         PHY Eval Training Control Register 2
#define DDR_UMCTL2_TRAINSTAT              DWC_DDR_UMCTL_BASE_ADDR+0x1DC    //         PHY Eval Training Status Register
#define DDR_UMCTL2_ADDRMAP0               DWC_DDR_UMCTL_BASE_ADDR+0x200    //         Address Map Register 0 *
#define DDR_UMCTL2_ADDRMAP1               DWC_DDR_UMCTL_BASE_ADDR+0x204    //         Address Map Register 1
#define DDR_UMCTL2_ADDRMAP2               DWC_DDR_UMCTL_BASE_ADDR+0x208    //         Address Map Register 2
#define DDR_UMCTL2_ADDRMAP3               DWC_DDR_UMCTL_BASE_ADDR+0x20C    //         Address Map Register 3
#define DDR_UMCTL2_ADDRMAP4               DWC_DDR_UMCTL_BASE_ADDR+0x210    //         Address Map Register 4
#define DDR_UMCTL2_ADDRMAP5               DWC_DDR_UMCTL_BASE_ADDR+0x214    //         Address Map Register 5
#define DDR_UMCTL2_ADDRMAP6               DWC_DDR_UMCTL_BASE_ADDR+0x218    //         Address Map Register 6
#define DDR_UMCTL2_ODTCFG                 DWC_DDR_UMCTL_BASE_ADDR+0x240    //         ODT Configuration Register
#define DDR_UMCTL2_ODTMAP                 DWC_DDR_UMCTL_BASE_ADDR+0x244    //         ODT/Rank Map Register
#define DDR_UMCTL2_SCHED                  DWC_DDR_UMCTL_BASE_ADDR+0x250    //         Scheduler Control Register    *
#define DDR_UMCTL2_PERFHPR0               DWC_DDR_UMCTL_BASE_ADDR+0x258    //         High Priority Read CAM Register 0
#define DDR_UMCTL2_PERFHPR1               DWC_DDR_UMCTL_BASE_ADDR+0x25c    //         High Priority Read CAM Register 1
#define DDR_UMCTL2_PERFLPR0               DWC_DDR_UMCTL_BASE_ADDR+0x260    //         Low Priority Read CAM Register 0
#define DDR_UMCTL2_PERFLPR1               DWC_DDR_UMCTL_BASE_ADDR+0x264    //         Low Priority Read CAM Register 1
#define DDR_UMCTL2_PERFWR0                DWC_DDR_UMCTL_BASE_ADDR+0x268    //         Write CAM Register 0
#define DDR_UMCTL2_PERFWR1                DWC_DDR_UMCTL_BASE_ADDR+0x26C    //         Write CAM Register 1
#define DDR_UMCTL2_DBG0                   DWC_DDR_UMCTL_BASE_ADDR+0x300    //         Debug Register 0
#define DDR_UMCTL2_DBG1                   DWC_DDR_UMCTL_BASE_ADDR+0x304    //         Debug Register 1
#define DDR_UMCTL2_DBGCAM                 DWC_DDR_UMCTL_BASE_ADDR+0x308    //         CAM Debug Register
#define DDR_UMCTL2_PCCFG                  DWC_DDR_UMCTL_BASE_ADDR+0x400    //         Configuration Register
#define DDR_UMCTL2_PCFGR_0                DWC_DDR_UMCTL_BASE_ADDR+0x404    //Port 0   Configuration Register
#define DDR_UMCTL2_PCFGW_0                DWC_DDR_UMCTL_BASE_ADDR+0x408    //Port 0   Configuration Register
#define DDR_UMCTL2_PCFGIDMASKCH0_0        DWC_DDR_UMCTL_BASE_ADDR+0x40C    //Port 0   Configuration Register
#define DDR_UMCTL2_PCFGIDVALIDCH0_0       DWC_DDR_UMCTL_BASE_ADDR+0x410    //Port 0   Configuration Register
#define DDR_UMCTL2_PCFGR_1                DWC_DDR_UMCTL_BASE_ADDR+0x4B4    //Port 1  Configuration Register
#define DDR_UMCTL2_PCFGW_1                DWC_DDR_UMCTL_BASE_ADDR+0x4B8    //Port 1  Configuration Register
#define DDR_UMCTL2_PCFGIDMASKCH0_1        DWC_DDR_UMCTL_BASE_ADDR+0x4BC    //Port 1  Configuration Register
#define DDR_UMCTL2_PCFGIDVALIDCH0_1       DWC_DDR_UMCTL_BASE_ADDR+0x4C0    //Port 1  Configuration Register
#define DDR_UMCTL2_PCFGR_2                DWC_DDR_UMCTL_BASE_ADDR+0x564    //Port 2  Configuration Register
#define DDR_UMCTL2_PCFGW_2                DWC_DDR_UMCTL_BASE_ADDR+0x568    //Port 2  Configuration Register
#define DDR_UMCTL2_PCFGIDMASKCH0_2        DWC_DDR_UMCTL_BASE_ADDR+0x56C    //Port 2  Configuration Register
#define DDR_UMCTL2_PCFGIDVALIDCH0_2       DWC_DDR_UMCTL_BASE_ADDR+0x570    //Port 2  Configuration Register
#define DDR_UMCTL2_PCFGR_3                DWC_DDR_UMCTL_BASE_ADDR+0x614    //Port 3  Configuration Register
#define DDR_UMCTL2_PCFGW_3                DWC_DDR_UMCTL_BASE_ADDR+0x618    //Port 3  Configuration Register
#define DDR_UMCTL2_PCFGIDMASKCH0_3        DWC_DDR_UMCTL_BASE_ADDR+0x61C    //Port 3  Configuration Register
#define DDR_UMCTL2_PCFGIDVALIDCH0_3       DWC_DDR_UMCTL_BASE_ADDR+0x620    //Port 3  Configuration Register
#define DDR_UMCTL2_PCFGR_4                DWC_DDR_UMCTL_BASE_ADDR+0x614    //Port 4  Configuration Register
#define DDR_UMCTL2_PCFGW_4                DWC_DDR_UMCTL_BASE_ADDR+0x618    //Port 4  Configuration Register
#define DDR_UMCTL2_PCFGIDMASKCH0_4        DWC_DDR_UMCTL_BASE_ADDR+0x61C    //Port 4  Configuration Register
#define DDR_UMCTL2_PCFGIDVALIDCH0_4       DWC_DDR_UMCTL_BASE_ADDR+0x620    //Port 4  Configuration Register
#define DDR_UMCTL2_PCFGR_5                DWC_DDR_UMCTL_BASE_ADDR+0x6C4    //Port 5  Configuration Register
#define DDR_UMCTL2_PCFGW_5                DWC_DDR_UMCTL_BASE_ADDR+0x6C8    //Port 5  Configuration Register
#define DDR_UMCTL2_PCFGIDMASKCH0_5        DWC_DDR_UMCTL_BASE_ADDR+0x6CC    //Port 5  Configuration Register
#define DDR_UMCTL2_PCFGIDVALIDCH0_5       DWC_DDR_UMCTL_BASE_ADDR+0x6D0    //Port 5  Configuration Register
#define DDR_UMCTL2_PCFGR_6                DWC_DDR_UMCTL_BASE_ADDR+0x774    //Port 6  Configuration Register
#define DDR_UMCTL2_PCFGW_6                DWC_DDR_UMCTL_BASE_ADDR+0x778    //Port 6  Configuration Register
#define DDR_UMCTL2_PCFGIDMASKCH0_6        DWC_DDR_UMCTL_BASE_ADDR+0x77C    //Port 6  Configuration Register
#define DDR_UMCTL2_PCFGIDVALIDCH0_6       DWC_DDR_UMCTL_BASE_ADDR+0x780    //Port 6  Configuration Register
#define DDR_UMCTL2_PCFGR_7                DWC_DDR_UMCTL_BASE_ADDR+0x824    //Port 7  Configuration Register
#define DDR_UMCTL2_PCFGW_7                DWC_DDR_UMCTL_BASE_ADDR+0x828    //Port 7  Configuration Register
#define DDR_UMCTL2_PCFGIDMASKCH0_7        DWC_DDR_UMCTL_BASE_ADDR+0x82C    //Port 7  Configuration Register
#define DDR_UMCTL2_PCFGIDVALIDCH0_7       DWC_DDR_UMCTL_BASE_ADDR+0x830    //Port 7  Configuration Register

/* uPCTL Registers */
#define DDR_UPCTL_SCFG                   DWC_DDR_UMCTL_BASE_ADDR+0x000    //State Configuration Register
#define DDR_UPCTL_SCTL                   DWC_DDR_UMCTL_BASE_ADDR+0x004    //State Control Register
#define DDR_UPCTL_STAT                   DWC_DDR_UMCTL_BASE_ADDR+0x008    //State Status Register
#define DDR_UPCTL_INTRSTAT               DWC_DDR_UMCTL_BASE_ADDR+0x00c    //Interrupt Status Register
#define DDR_UPCTL_MCMD                   DWC_DDR_UMCTL_BASE_ADDR+0x040    //Memory Command Register
#define DDR_UPCTL_POWCTL                 DWC_DDR_UMCTL_BASE_ADDR+0x044    //Power Up Control Register
#define DDR_UPCTL_POWSTAT                DWC_DDR_UMCTL_BASE_ADDR+0x048    //Power Up Status Register
#define DDR_UPCTL_CMDTSTAT               DWC_DDR_UMCTL_BASE_ADDR+0x04c    //Command Timing Status Register
#define DDR_UPCTL_CMDTSTATEN             DWC_DDR_UMCTL_BASE_ADDR+0x050    //Command Timing Status Enable Register
#define DDR_UPCTL_MRRCFG0                DWC_DDR_UMCTL_BASE_ADDR+0x060    //MRR Configuration 0 Register
#define DDR_UPCTL_MRRSTAT0               DWC_DDR_UMCTL_BASE_ADDR+0x064    //MRR Status 0 Register
#define DDR_UPCTL_MRRSTAT1               DWC_DDR_UMCTL_BASE_ADDR+0x068    //MRR Status 1 Register
#define DDR_UPCTL_MCFG1                  DWC_DDR_UMCTL_BASE_ADDR+0x07c    //Memory Configuration Register 1  add
#define DDR_UPCTL_MCFG                   DWC_DDR_UMCTL_BASE_ADDR+0x080    //Memory Configuration Register
#define DDR_UPCTL_PPCFG                  DWC_DDR_UMCTL_BASE_ADDR+0x084    //Partially Populated Memories Configuration Register
#define DDR_UPCTL_MSTAT                  DWC_DDR_UMCTL_BASE_ADDR+0x088    //Memory Status Register
#define DDR_LPDDR2ZQCFG                  DWC_DDR_UMCTL_BASE_ADDR+0x08c    //LPDDR2 ZQ configuration register   add
#define DDR_UPCTL_DTUPDES                DWC_DDR_UMCTL_BASE_ADDR+0x094    //DTU Status
#define DDR_UPCTL_DTUNA                  DWC_DDR_UMCTL_BASE_ADDR+0x098    //DTU Number of Random Addresses Created
#define DDR_UPCTL_DTUNE                  DWC_DDR_UMCTL_BASE_ADDR+0x09c    //DTU Number of Errors
#define DDR_UPCTL_DTUPRD0                DWC_DDR_UMCTL_BASE_ADDR+0x0a0    //DTU Parallel Read 0
#define DDR_UPCTL_DTUPRD1                DWC_DDR_UMCTL_BASE_ADDR+0x0a4    //DTU Parallel Read 1
#define DDR_UPCTL_DTUPRD2                DWC_DDR_UMCTL_BASE_ADDR+0x0a8    //DTU Parallel Read 2
#define DDR_UPCTL_DTUPRD3                DWC_DDR_UMCTL_BASE_ADDR+0x0ac    //DTU Parallel Read 3
#define DDR_UPCTL_DTUAWDT                DWC_DDR_UMCTL_BASE_ADDR+0x0b0    //DTU Address Width
#define DDR_UPCTL_TOGCNT1U               DWC_DDR_UMCTL_BASE_ADDR+0x0c0    //toggle counter 1u Register
#define DDR_UPCTL_TINIT                  DWC_DDR_UMCTL_BASE_ADDR+0x0c4    //t_init timing Register
#define DDR_UPCTL_TRSTH                  DWC_DDR_UMCTL_BASE_ADDR+0x0c8    //reset high time Register
#define DDR_UPCTL_TOGCNT100N             DWC_DDR_UMCTL_BASE_ADDR+0x0cc    //toggle counter 100n Register
#define DDR_UPCTL_TREFI                  DWC_DDR_UMCTL_BASE_ADDR+0x0d0    //t_refi timing Register
#define DDR_UPCTL_TMRD                   DWC_DDR_UMCTL_BASE_ADDR+0x0d4    //t_mrd timing Register
#define DDR_UPCTL_TRFC                   DWC_DDR_UMCTL_BASE_ADDR+0x0d8    //t_rfc timing Register
#define DDR_UPCTL_TRP                    DWC_DDR_UMCTL_BASE_ADDR+0x0dc    //t_rp timing Register
#define DDR_UPCTL_TRTW                   DWC_DDR_UMCTL_BASE_ADDR+0x0e0    //t_rtw Register
#define DDR_UPCTL_TAL                    DWC_DDR_UMCTL_BASE_ADDR+0x0e4    //al latency Register
#define DDR_UPCTL_TCL                    DWC_DDR_UMCTL_BASE_ADDR+0x0e8    //cl timing Register
#define DDR_UPCTL_TCWL                   DWC_DDR_UMCTL_BASE_ADDR+0x0ec    //cwl Register
#define DDR_UPCTL_TRAS                   DWC_DDR_UMCTL_BASE_ADDR+0x0f0    //t_ras timing Register
#define DDR_UPCTL_TRC                    DWC_DDR_UMCTL_BASE_ADDR+0x0f4    //t_rc timing Register
#define DDR_UPCTL_TRCD                   DWC_DDR_UMCTL_BASE_ADDR+0x0f8    //t_rcd timing Register
#define DDR_UPCTL_TRRD                   DWC_DDR_UMCTL_BASE_ADDR+0x0fc    //t_rrd timing Register
#define DDR_UPCTL_TRTP                   DWC_DDR_UMCTL_BASE_ADDR+0x100    //t_rtp timing Register
#define DDR_UPCTL_TWR                    DWC_DDR_UMCTL_BASE_ADDR+0x104    //t_wr timing Register
#define DDR_UPCTL_TWTR                   DWC_DDR_UMCTL_BASE_ADDR+0x108    //t_wtr timing Register
#define DDR_UPCTL_TEXSR                  DWC_DDR_UMCTL_BASE_ADDR+0x10c    //t_exsr timing Register
#define DDR_UPCTL_TXP                    DWC_DDR_UMCTL_BASE_ADDR+0x110    //t_xp timing Register
#define DDR_UPCTL_TXPDLL                 DWC_DDR_UMCTL_BASE_ADDR+0x114    //t_xpdll timing Register
#define DDR_UPCTL_TZQCS                  DWC_DDR_UMCTL_BASE_ADDR+0x118    //t_zqcs timing Register
#define DDR_UPCTL_TZQCSI                 DWC_DDR_UMCTL_BASE_ADDR+0x11c    //t_zqcsi timing Register
#define DDR_UPCTL_TDQS                   DWC_DDR_UMCTL_BASE_ADDR+0x120    //t_dqs timing Register
#define DDR_UPCTL_TCKSRE                 DWC_DDR_UMCTL_BASE_ADDR+0x124    //t_cksre timing Register
#define DDR_UPCTL_TCKSRX                 DWC_DDR_UMCTL_BASE_ADDR+0x128    //t_cksrx timing Register
#define DDR_UPCTL_TCKE                   DWC_DDR_UMCTL_BASE_ADDR+0x12c    //t_cke timing Register
#define DDR_UPCTL_TMOD                   DWC_DDR_UMCTL_BASE_ADDR+0x130    //t_mod timing Register
#define DDR_UPCTL_TRSTL                  DWC_DDR_UMCTL_BASE_ADDR+0x134    //reset low timing Register
#define DDR_UPCTL_TZQCL                  DWC_DDR_UMCTL_BASE_ADDR+0x138    //t_zqcl timing Register
#define DDR_UPCTL_TMRR                   DWC_DDR_UMCTL_BASE_ADDR+0x13c    //t_mrr timing Register
#define DDR_UPCTL_TCKESR                 DWC_DDR_UMCTL_BASE_ADDR+0x140    //t_ckesr timing Register
#define DDR_UPCTL_TDPD                   DWC_DDR_UMCTL_BASE_ADDR+0x144    //t_dpd timing Register
#define DDR_UPCTL_ECCCFG                 DWC_DDR_UMCTL_BASE_ADDR+0x180    //ECC Configuration Register
#define DDR_UPCTL_ECCTST                 DWC_DDR_UMCTL_BASE_ADDR+0x184    //ECC Test Register
#define DDR_UPCTL_ECCCLR                 DWC_DDR_UMCTL_BASE_ADDR+0x188    //ECC Clear Register
#define DDR_UPCTL_ECCLOG                 DWC_DDR_UMCTL_BASE_ADDR+0x18c    //ECC Log Register
#define DDR_UPCTL_DTUWACTL               DWC_DDR_UMCTL_BASE_ADDR+0x200    //DTU write address control Register
#define DDR_UPCTL_DTURACTL               DWC_DDR_UMCTL_BASE_ADDR+0x204    //DTU read address control Register
#define DDR_UPCTL_DTUCFG                 DWC_DDR_UMCTL_BASE_ADDR+0x208    //DTU configuration Register
#define DDR_UPCTL_DTUECTL                DWC_DDR_UMCTL_BASE_ADDR+0x20c    //DTU execute control Register
#define DDR_UPCTL_DTUWD0                 DWC_DDR_UMCTL_BASE_ADDR+0x210    //DTU write data #0
#define DDR_UPCTL_DTUWD1                 DWC_DDR_UMCTL_BASE_ADDR+0x214    //DTU write data #1
#define DDR_UPCTL_DTUWD2                 DWC_DDR_UMCTL_BASE_ADDR+0x218    //DTU write data #2
#define DDR_UPCTL_DTUWD3                 DWC_DDR_UMCTL_BASE_ADDR+0x21c    //DTU write data #3
#define DDR_UPCTL_DTUWDM                 DWC_DDR_UMCTL_BASE_ADDR+0x220    //DTU write data mask
#define DDR_UPCTL_DTURD0                 DWC_DDR_UMCTL_BASE_ADDR+0x224    //DTU read data #0
#define DDR_UPCTL_DTURD1                 DWC_DDR_UMCTL_BASE_ADDR+0x228    //DTU read data #1
#define DDR_UPCTL_DTURD2                 DWC_DDR_UMCTL_BASE_ADDR+0x22c    //DTU read data #2
#define DDR_UPCTL_DTURD3                 DWC_DDR_UMCTL_BASE_ADDR+0x230    //DTU read data #3
#define DDR_UPCTL_DTULFSRWD              DWC_DDR_UMCTL_BASE_ADDR+0x234    //DTU LFSR seed for write data generation
#define DDR_UPCTL_DTULFSRRD              DWC_DDR_UMCTL_BASE_ADDR+0x238    //DTU LFSR seed for read data generation
#define DDR_UPCTL_DTUEAF                 DWC_DDR_UMCTL_BASE_ADDR+0x23c    //DTU error address FIFO
#define DDR_UPCTL_DFITCTRLDELAY          DWC_DDR_UMCTL_BASE_ADDR+0x240    //DFI tctrl_delay Register
#define DDR_UPCTL_DFIODTCFG              DWC_DDR_UMCTL_BASE_ADDR+0x244    //DFI ODT Configuration Register
#define DDR_UPCTL_DFIODTCFG1             DWC_DDR_UMCTL_BASE_ADDR+0x248    //DFI ODT Timing Configuration Register
#define DDR_UPCTL_DFIODTRANKMAP          DWC_DDR_UMCTL_BASE_ADDR+0x24c    //DFI ODT Rank Mapping Register
#define DDR_UPCTL_DFITPHYWRDATA          DWC_DDR_UMCTL_BASE_ADDR+0x250    //DFI tphy_wrdata Register
#define DDR_UPCTL_DFITPHYWRLAT           DWC_DDR_UMCTL_BASE_ADDR+0x254    //DFI tphy_wrlat Register
#define DDR_UPCTL_DFITRDDATAEN           DWC_DDR_UMCTL_BASE_ADDR+0x260    //DFI trddata_en Register
#define DDR_UPCTL_DFITPHYRDLAT           DWC_DDR_UMCTL_BASE_ADDR+0x264    //DFI tphy_rddata Register
#define DDR_UPCTL_DFITPHYUPDTYPE0        DWC_DDR_UMCTL_BASE_ADDR+0x270    //DFI tphyupd_type0 Register
#define DDR_UPCTL_DFITPHYUPDTYPE1        DWC_DDR_UMCTL_BASE_ADDR+0x274    //DFI tphyupd_type1 Register
#define DDR_UPCTL_DFITPHYUPDTYPE2        DWC_DDR_UMCTL_BASE_ADDR+0x278    //DFI tphyupd_type2 Register
#define DDR_UPCTL_DFITPHYUPDTYPE3        DWC_DDR_UMCTL_BASE_ADDR+0x27c    //DFI tphyupd_type3 Register
#define DDR_UPCTL_DFITCTRLUPDMIN         DWC_DDR_UMCTL_BASE_ADDR+0x280    //DFI tctrlupd_min Register
#define DDR_UPCTL_DFITCTRLUPDMAX         DWC_DDR_UMCTL_BASE_ADDR+0x284    //DFI tctrlupd_max Register
#define DDR_UPCTL_DFITCTRLUPDDLY         DWC_DDR_UMCTL_BASE_ADDR+0x288    //DFI tctrlupd_dly Register
#define DDR_UPCTL_DFIUPDCFG              DWC_DDR_UMCTL_BASE_ADDR+0x290    //DFI Update Configuration Register
#define DDR_UPCTL_DFITREFMSKI            DWC_DDR_UMCTL_BASE_ADDR+0x294    //DFI Masked Refresh Interval Register
#define DDR_UPCTL_DFITCTRLUPDI           DWC_DDR_UMCTL_BASE_ADDR+0x298    //DFI tctrlupd_interval Register
#define DDR_UPCTL_DFITRCFG0              DWC_DDR_UMCTL_BASE_ADDR+0x2ac    //DFI Training Configuration 0 Register
#define DDR_UPCTL_DFITRSTAT0             DWC_DDR_UMCTL_BASE_ADDR+0x2b0    //DFI Training Status 0 Register
#define DDR_UPCTL_DFIRWRLVLEN            DWC_DDR_UMCTL_BASE_ADDR+0x2b4    //DFI Training dfi_wrlvl_en Register
#define DDR_UPCTL_DFIRRDLVLEN            DWC_DDR_UMCTL_BASE_ADDR+0x2b8    //DFI Training dfi_rdlvl_en Register
#define DDR_UPCTL_DFIRRDLVLGATEEN        DWC_DDR_UMCTL_BASE_ADDR+0x2bc    //DFI Training dfi_rdlvl_gate_en Register
#define DDR_UPCTL_DFISTSTAT0             DWC_DDR_UMCTL_BASE_ADDR+0x2c0    //DFI STATUS Status 0 Register
#define DDR_UPCTL_DFISTCFG0              DWC_DDR_UMCTL_BASE_ADDR+0x2c4    //DFI STATUS Configuration 0 Register
#define DDR_UPCTL_DFISTCFG1              DWC_DDR_UMCTL_BASE_ADDR+0x2c8    //DFI STATUS Configuration 1 Register
#define DDR_UPCTL_DFITDRAMCLKEN          DWC_DDR_UMCTL_BASE_ADDR+0x2d0    //DFI tdram_clk_disable Register
#define DDR_UPCTL_DFITDRAMCLKDIS         DWC_DDR_UMCTL_BASE_ADDR+0x2d4    //DFI tdram_clk_enable Register
#define DDR_UPCTL_DFISTCFG2              DWC_DDR_UMCTL_BASE_ADDR+0x2d8    //DFI STATUS Configuration 2 Register
#define DDR_UPCTL_DFISTPARCLR            DWC_DDR_UMCTL_BASE_ADDR+0x2dc    //DFI STATUS Parity Clear Register
#define DDR_UPCTL_DFISTPARLOG            DWC_DDR_UMCTL_BASE_ADDR+0x2e0    //DFI STATUS Parity Log Register
#define DDR_UPCTL_DFILPCFG0              DWC_DDR_UMCTL_BASE_ADDR+0x2f0    //DFI Low Power Configuration 0 Register
#define DDR_UPCTL_DFITRWRLVLRESP0        DWC_DDR_UMCTL_BASE_ADDR+0x300    //DFI Training dfi_wrlvl_resp Status 0
#define DDR_UPCTL_DFITRWRLVLRESP1        DWC_DDR_UMCTL_BASE_ADDR+0x304    //DFI Training dfi_wrlvl_resp Status 1
#define DDR_UPCTL_DFITRWRLVLRESP2        DWC_DDR_UMCTL_BASE_ADDR+0x308    //DFI Training dfi_wrlvl_resp Status 2
#define DDR_UPCTL_DFITRRDLVLRESP0        DWC_DDR_UMCTL_BASE_ADDR+0x30c    //DFI Training dfi_rdlvl_resp Status 0
#define DDR_UPCTL_DFITRRDLVLRESP1        DWC_DDR_UMCTL_BASE_ADDR+0x310    //DFI Training dfi_rdlvl_resp Status 1
#define DDR_UPCTL_DFITRRDLVLRESP2        DWC_DDR_UMCTL_BASE_ADDR+0x314    //DFI Training dfi_rdlvl_resp Status 2
#define DDR_UPCTL_DFITRWRLVLDELAY0       DWC_DDR_UMCTL_BASE_ADDR+0x318    //DFI Training dfi_wrlvl_delay Configuration0
#define DDR_UPCTL_DFITRWRLVLDELAY1       DWC_DDR_UMCTL_BASE_ADDR+0x31c    //DFI Training dfi_wrlvl_delay Configuration1
#define DDR_UPCTL_DFITRWRLVLDELAY2       DWC_DDR_UMCTL_BASE_ADDR+0x320    //DFI Training dfi_wrlvl_delay Configuration2
#define DDR_UPCTL_DFITRRDLVLDELAY0       DWC_DDR_UMCTL_BASE_ADDR+0x324    //DFI Training dfi_rdlvl_delay Configuration0
#define DDR_UPCTL_DFITRRDLVLDELAY1       DWC_DDR_UMCTL_BASE_ADDR+0x328    //DFI Training dfi_rdlvl_delay Configuration1
#define DDR_UPCTL_DFITRRDLVLDELAY2       DWC_DDR_UMCTL_BASE_ADDR+0x32c    //DFI Training dfi_rdlvl_delay Configuration2
#define DDR_UPCTL_DFITRRDLVLGATEDELAY0   DWC_DDR_UMCTL_BASE_ADDR+0x330    //DFI Training dfi_rdlvl_gate_delay Configuration0
#define DDR_UPCTL_DFITRRDLVLGATEDELAY1   DWC_DDR_UMCTL_BASE_ADDR+0x334    //DFI Training dfi_rdlvl_gate_delay Configuration1
#define DDR_UPCTL_DFITRRDLVLGATEDELAY2   DWC_DDR_UMCTL_BASE_ADDR+0x338    //DFI Training dfi_rdlvl_gate_delay Configuration2
#define DDR_UPCTL_DFITRCMD               DWC_DDR_UMCTL_BASE_ADDR+0x33c    //DFI Training Command Register
#define DDR_UPCTL_IPVR                   DWC_DDR_UMCTL_BASE_ADDR+0x3f8    //IP Version Register
#define DDR_UPCTL_IPTR                   DWC_DDR_UMCTL_BASE_ADDR+0x3fc    //IP Type Register



#define LOCK_PLL        0x4
#define DRAM_PORT_RESET 0xc
#define DRAM_CORE_RESET 0x0

enum PORT_TYPE {
    A9DDR2,            
    A9DDR3,            
    A9LPDDR2,          
    A9MDDR,           
    A9UMCTL2_DDR3,     
    A9UMCTL2_DDR2,     
};

enum BL_TYPE {
    BL4,
    BL8,
};

enum TEST_TYPE {
    NORMAL_TEST,
    BISTLP_TEST,
    SIMULATION_TEST,
};

enum LATENCY_TYPE {
    A9CL4,
    A9CL5,
    A9CL6,
    A9CL7,
    A9CL8,
};

enum REGISTER_SET {
    EMPTY,
    DRAM_SET_ADD,
};

struct drama9_ctrl {
        int port_type;
        int bl_type;
        int test_type;
        int latency_type;
        int register_set;
};

enum umctl_index {
    UMCTL2_DFIMISC, 
    UMCTL2_MSTR,    
    UMCTL2_RFSHTMG, 
    UMCTL2_INIT0,   
    UMCTL2_INIT1,   
    UMCTL2_INIT3,   
    UMCTL2_INIT4,   
    UMCTL2_INIT5,   
    UMCTL2_RANKCTL, 
    UMCTL2_DRAMTMG0,
    UMCTL2_DRAMTMG1,
    UMCTL2_DRAMTMG2,
    UMCTL2_DRAMTMG3,
    UMCTL2_DRAMTMG4,
    UMCTL2_DRAMTMG5,
    UMCTL2_DRAMTMG6,
    UMCTL2_DRAMTMG7,
    UMCTL2_DRAMTMG8,
    UMCTL2_ZQCTL0,  
    UMCTL2_ZQCTL1,  
    UMCTL2_DFITMG0, 
    UMCTL2_DFITMG1, 
    UMCTL2_DFIUPD0, 
    UMCTL2_DFIUPD1, 
    UMCTL2_ADDRMAP0,
    UMCTL2_ADDRMAP1,
    UMCTL2_ADDRMAP2,
    UMCTL2_ADDRMAP3,
    UMCTL2_ADDRMAP4,
    UMCTL2_ADDRMAP5,
    UMCTL2_ADDRMAP6,
    UMCTL2_ODTMAP,  
    UMCTL2_SCHED,   
    UMCTL2_PERFHPR0,
    UMCTL2_PERFHPR1,
    UMCTL2_PERFLPR0,
    UMCTL2_PERFLPR1,
    UMCTL2_PERFWR0, 
    UMCTL2_PERFWR1, 
    UMCTL2_DBG0,    
    UMCTL2_PCCFG,   
    UMCTL2_PCFGR_0, 
    UMCTL2_PCFGW_0, 
    UMCTL2_PCFGR_1, 
    UMCTL2_PCFGW_1, 
    UMCTL2_PCFGR_2, 
    UMCTL2_PCFGW_2, 
    UMCTL2_PCFGR_3, 
    UMCTL2_PCFGW_3, 
    UMCTL2_PCFGR_4, 
    UMCTL2_PCFGW_4, 
    UMCTL2_PCFGR_5, 
    UMCTL2_PCFGW_5, 
    UMCTL2_PCFGR_6, 
    UMCTL2_PCFGW_6, 
    UMCTL2_PCFGR_7, 
    UMCTL2_PCFGW_7, 
};

enum phy_index {
    PHY_PGCR, 
    PHY_DCR,  
    PHY_MR0,  
    PHY_MR1,  
    PHY_MR2,  
    PHY_MR3,  
    PHY_DTPR0,
    PHY_DTPR1,
    PHY_DTPR2,
    PHY_PTR0, 
};

struct drama9_reg {
        uint32_t reg;
        uint32_t value;
        uint32_t flags;
};

#define ENABLE  1
#define DISABLE 0

#define SET_DRAM_REQ(_reg, _value)      \
{ \
    .reg   = _reg,                      \
    .value = _value,                    \
}

#define SET_DRAM_REQ_ABLE(_reg, _value, _flags)         \
{ \
    .reg   = _reg,                                      \
    .value = _value,                                    \
    .flags = _flags,                                    \
}

void drama9_init(void);
#endif
