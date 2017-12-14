#ifndef __IMAPX_DRAMC__
#define __IMAPX_DRAMC__

/*========================================================================
 * DWC_DDR_UMCTL_MAP Registers 
 *======================================================================*/

/* uMCTL Registers */
#define ADDR_UMCTL_PCFG(x)               (DWC_DDR_UMCTL_BASE_ADDR + 0x400 + 4 * x)     /* Port x */    
#define ADDR_UMCTL_PCFG_0                (DWC_DDR_UMCTL_BASE_ADDR+0x400)     /* Port 0   Configuration Register */ 
#define ADDR_UMCTL_PCFG_1                (DWC_DDR_UMCTL_BASE_ADDR+0x404)     /* Port 1   Configuration Register */ 
#define ADDR_UMCTL_PCFG_2                (DWC_DDR_UMCTL_BASE_ADDR+0x408)     /* Port 2   Configuration Register */ 
#define ADDR_UMCTL_PCFG_3                (DWC_DDR_UMCTL_BASE_ADDR+0x40c)     /* Port 3   Configuration Register */ 
#define ADDR_UMCTL_PCFG_4                (DWC_DDR_UMCTL_BASE_ADDR+0x410)     /* Port 4   Configuration Register */ 
#define ADDR_UMCTL_PCFG_5                (DWC_DDR_UMCTL_BASE_ADDR+0x414)     /* Port 5   Configuration Register */ 
#define ADDR_UMCTL_PCFG_6                (DWC_DDR_UMCTL_BASE_ADDR+0x418)     /* Port 6   Configuration Register */ 
#define ADDR_UMCTL_PCFG_7                (DWC_DDR_UMCTL_BASE_ADDR+0x41c)     /* Port 7   Configuration Register */ 
#define ADDR_UMCTL_PCFG_8                (DWC_DDR_UMCTL_BASE_ADDR+0x420)     /* Port 8   Configuration Register */ 
#define ADDR_UMCTL_PCFG_9                (DWC_DDR_UMCTL_BASE_ADDR+0x424)     /* Port 9   Configuration Register */ 
#define ADDR_UMCTL_PCFG_10               (DWC_DDR_UMCTL_BASE_ADDR+0x428)     /* Port 10  Configuration Register */ 
#define ADDR_UMCTL_PCFG_11               (DWC_DDR_UMCTL_BASE_ADDR+0x42c)     /* Port 11  Configuration Register */ 
#define ADDR_UMCTL_PCFG_12               (DWC_DDR_UMCTL_BASE_ADDR+0x430)     /* Port 12  Configuration Register */ 
#define ADDR_UMCTL_PCFG_13               (DWC_DDR_UMCTL_BASE_ADDR+0x434)     /* Port 13  Configuration Register */ 
#define ADDR_UMCTL_PCFG_14               (DWC_DDR_UMCTL_BASE_ADDR+0x438)     /* Port 14  Configuration Register */ 
#define ADDR_UMCTL_PCFG_15               (DWC_DDR_UMCTL_BASE_ADDR+0x43c)     /* Port 15  Configuration Register */ 
#define ADDR_UMCTL_PCFG_16               (DWC_DDR_UMCTL_BASE_ADDR+0x440)     /* Port 16  Configuration Register */ 
#define ADDR_UMCTL_PCFG_17               (DWC_DDR_UMCTL_BASE_ADDR+0x444)     /* Port 17  Configuration Register */ 
#define ADDR_UMCTL_PCFG_18               (DWC_DDR_UMCTL_BASE_ADDR+0x448)     /* Port 18  Configuration Register */ 
#define ADDR_UMCTL_PCFG_19               (DWC_DDR_UMCTL_BASE_ADDR+0x44c)     /* Port 19  Configuration Register */ 
#define ADDR_UMCTL_PCFG_20               (DWC_DDR_UMCTL_BASE_ADDR+0x450)     /* Port 20  Configuration Register */ 
#define ADDR_UMCTL_PCFG_21               (DWC_DDR_UMCTL_BASE_ADDR+0x454)     /* Port 21  Configuration Register */ 
#define ADDR_UMCTL_PCFG_22               (DWC_DDR_UMCTL_BASE_ADDR+0x458)     /* Port 22  Configuration Register */ 
#define ADDR_UMCTL_PCFG_23               (DWC_DDR_UMCTL_BASE_ADDR+0x45c)     /* Port 23  Configuration Register */ 
#define ADDR_UMCTL_PCFG_24               (DWC_DDR_UMCTL_BASE_ADDR+0x460)     /* Port 24  Configuration Register */ 
#define ADDR_UMCTL_PCFG_25               (DWC_DDR_UMCTL_BASE_ADDR+0x464)     /* Port 25  Configuration Register */ 
#define ADDR_UMCTL_PCFG_26               (DWC_DDR_UMCTL_BASE_ADDR+0x468)     /* Port 26  Configuration Register */ 
#define ADDR_UMCTL_PCFG_27               (DWC_DDR_UMCTL_BASE_ADDR+0x46c)     /* Port 27  Configuration Register */ 
#define ADDR_UMCTL_PCFG_28               (DWC_DDR_UMCTL_BASE_ADDR+0x470)     /* Port 28  Configuration Register */ 
#define ADDR_UMCTL_PCFG_29               (DWC_DDR_UMCTL_BASE_ADDR+0x474)     /* Port 29  Configuration Register */ 
#define ADDR_UMCTL_PCFG_30               (DWC_DDR_UMCTL_BASE_ADDR+0x478)     /* Port 30  Configuration Register */ 
#define ADDR_UMCTL_PCFG_31               (DWC_DDR_UMCTL_BASE_ADDR+0x47c)     /* Port 31  Configuration Register */ 
                                                                                               
#define ADDR_UMCTL_CCFG                  (DWC_DDR_UMCTL_BASE_ADDR+0x480)     /* Controller Configuration Register */
#define ADDR_UMCTL_DCFG                  (DWC_DDR_UMCTL_BASE_ADDR+0x484)     /* DRAM Configuration Register */       
#define ADDR_UMCTL_CSTAT                 (DWC_DDR_UMCTL_BASE_ADDR+0x488)     /* Controller Status Register */        
#define ADDR_UMCTL_CCFG1                 (DWC_DDR_UMCTL_BASE_ADDR+0x48c)     /* Controller Status Register 1     add */
                                                                                                 
/* uPCTL Registers                                                                               
   Operational State,Control,Status Registers */                                                
#define ADDR_UPCTL_SCFG                  (DWC_DDR_UMCTL_BASE_ADDR+0x000)     /* State Configuration Register */                                                                                                          
#define ADDR_UPCTL_SCTL                  (DWC_DDR_UMCTL_BASE_ADDR+0x004)     /* State Control Register */                                                                                                                
#define ADDR_UPCTL_STAT                  (DWC_DDR_UMCTL_BASE_ADDR+0x008)     /* State Status Register */                                                                                                               
#define ADDR_UPCTL_INTRSTAT              (DWC_DDR_UMCTL_BASE_ADDR+0x00c)     /* Interrupt Status Register */                                                                                                                  
                                                                                                 
/* Initialization Control and Status Registers */                                                
#define ADDR_UPCTL_MCMD                  (DWC_DDR_UMCTL_BASE_ADDR+0x040)     /* Memory Command Register */                                                                                                         
#define ADDR_UPCTL_POWCTL                (DWC_DDR_UMCTL_BASE_ADDR+0x044)     /* Power Up Control Register */                                                                                                                
#define ADDR_UPCTL_POWSTAT               (DWC_DDR_UMCTL_BASE_ADDR+0x048)     /* Power Up Status Register */                                                                                                                  
#define ADDR_UPCTL_CMDTSTAT              (DWC_DDR_UMCTL_BASE_ADDR+0x04c)     /* Command Timing Status Register */                                                                                                                  
#define ADDR_UPCTL_CMDTSTATEN            (DWC_DDR_UMCTL_BASE_ADDR+0x050)     /* Command Timing Status Enable Register */                                                                                                          
#define ADDR_UPCTL_MRRCFG0               (DWC_DDR_UMCTL_BASE_ADDR+0x060)     /* MRR Configuration 0 Register */                                                                                                                 
#define ADDR_UPCTL_MRRSTAT0              (DWC_DDR_UMCTL_BASE_ADDR+0x064)     /* MRR Status 0 Register */                                                                                                             
#define ADDR_UPCTL_MRRSTAT1              (DWC_DDR_UMCTL_BASE_ADDR+0x068)     /* MRR Status 1 Register */              
                                                                                                
/* Memory Control and Status Registers */                                                        
#define ADDR_UPCTL_MCFG1                 (DWC_DDR_UMCTL_BASE_ADDR+0x07c)     /* Memory Configuration Register 1  add */
#define ADDR_UPCTL_MCFG                  (DWC_DDR_UMCTL_BASE_ADDR+0x080)     /* Memory Configuration Register */                                                                                                              
#define ADDR_UPCTL_PPCFG                 (DWC_DDR_UMCTL_BASE_ADDR+0x084)     /* Partially Populated Memories Configuration Register */
#define ADDR_UPCTL_MSTAT                 (DWC_DDR_UMCTL_BASE_ADDR+0x088)     /* Memory Status Register */                                                                                                    
#define ADDR_LPDDR2ZQCFG                 (DWC_DDR_UMCTL_BASE_ADDR+0x08c)     /* LPDDR2 ZQ configuration register   add */ 
                                                                                                 
/* DTU Control and Status Registers */                                                           
#define ADDR_UPCTL_DTUPDES               (DWC_DDR_UMCTL_BASE_ADDR+0x094)     /* DTU Status */
#define ADDR_UPCTL_DTUNA                 (DWC_DDR_UMCTL_BASE_ADDR+0x098)     /* DTU Number of Random Addresses Created */
#define ADDR_UPCTL_DTUNE                 (DWC_DDR_UMCTL_BASE_ADDR+0x09c)     /* DTU Number of Errors */	
#define ADDR_UPCTL_DTUPRD0               (DWC_DDR_UMCTL_BASE_ADDR+0x0a0)     /* DTU Parallel Read 0 */
#define ADDR_UPCTL_DTUPRD1               (DWC_DDR_UMCTL_BASE_ADDR+0x0a4)     /* DTU Parallel Read 1 */
#define ADDR_UPCTL_DTUPRD2               (DWC_DDR_UMCTL_BASE_ADDR+0x0a8)     /* DTU Parallel Read 2 */
#define ADDR_UPCTL_DTUPRD3               (DWC_DDR_UMCTL_BASE_ADDR+0x0ac)     /* DTU Parallel Read 3 */
#define ADDR_UPCTL_DTUAWDT               (DWC_DDR_UMCTL_BASE_ADDR+0x0b0)     /* DTU Address Width */
                                                                                                 
/* Memory Timing Registers */                                                                   
#define ADDR_UPCTL_TOGCNT1U              (DWC_DDR_UMCTL_BASE_ADDR+0x0c0)     /* toggle counter 1u Register */
#define ADDR_UPCTL_TINIT                 (DWC_DDR_UMCTL_BASE_ADDR+0x0c4)     /* t_init timing Register */
#define ADDR_UPCTL_TRSTH                 (DWC_DDR_UMCTL_BASE_ADDR+0x0c8)     /* reset high time Register */
#define ADDR_UPCTL_TOGCNT100N            (DWC_DDR_UMCTL_BASE_ADDR+0x0cc)     /* toggle counter 100n Register */
#define ADDR_UPCTL_TREFI                 (DWC_DDR_UMCTL_BASE_ADDR+0x0d0)     /* t_refi timing Register */
#define ADDR_UPCTL_TMRD                  (DWC_DDR_UMCTL_BASE_ADDR+0x0d4)     /* t_mrd timing Register */
#define ADDR_UPCTL_TRFC                  (DWC_DDR_UMCTL_BASE_ADDR+0x0d8)     /* t_rfc timing Register */
#define ADDR_UPCTL_TRP                   (DWC_DDR_UMCTL_BASE_ADDR+0x0dc)     /* t_rp timing Register */
#define ADDR_UPCTL_TRTW                  (DWC_DDR_UMCTL_BASE_ADDR+0x0e0)     /* t_rtw Register */
#define ADDR_UPCTL_TAL                   (DWC_DDR_UMCTL_BASE_ADDR+0x0e4)     /* al latency Register */
#define ADDR_UPCTL_TCL                   (DWC_DDR_UMCTL_BASE_ADDR+0x0e8)     /* cl timing Register */
#define ADDR_UPCTL_TCWL                  (DWC_DDR_UMCTL_BASE_ADDR+0x0ec)     /* cwl Register */
#define ADDR_UPCTL_TRAS                  (DWC_DDR_UMCTL_BASE_ADDR+0x0f0)     /* t_ras timing Register */
#define ADDR_UPCTL_TRC                   (DWC_DDR_UMCTL_BASE_ADDR+0x0f4)     /* t_rc timing Register */
#define ADDR_UPCTL_TRCD                  (DWC_DDR_UMCTL_BASE_ADDR+0x0f8)     /* t_rcd timing Register */
#define ADDR_UPCTL_TRRD                  (DWC_DDR_UMCTL_BASE_ADDR+0x0fc)     /* t_rrd timing Register */
#define ADDR_UPCTL_TRTP                  (DWC_DDR_UMCTL_BASE_ADDR+0x100)     /* t_rtp timing Register */
#define ADDR_UPCTL_TWR                   (DWC_DDR_UMCTL_BASE_ADDR+0x104)     /* t_wr timing Register */
#define ADDR_UPCTL_TWTR                  (DWC_DDR_UMCTL_BASE_ADDR+0x108)     /* t_wtr timing Register */
#define ADDR_UPCTL_TEXSR                 (DWC_DDR_UMCTL_BASE_ADDR+0x10c)     /* t_exsr timing Register */
#define ADDR_UPCTL_TXP                   (DWC_DDR_UMCTL_BASE_ADDR+0x110)     /* t_xp timing Register */
#define ADDR_UPCTL_TXPDLL                (DWC_DDR_UMCTL_BASE_ADDR+0x114)     /* t_xpdll timing Register */
#define ADDR_UPCTL_TZQCS                 (DWC_DDR_UMCTL_BASE_ADDR+0x118)     /* t_zqcs timing Register */
#define ADDR_UPCTL_TZQCSI                (DWC_DDR_UMCTL_BASE_ADDR+0x11c)     /* t_zqcsi timing Register */
#define ADDR_UPCTL_TDQS                  (DWC_DDR_UMCTL_BASE_ADDR+0x120)     /* t_dqs timing Register */
#define ADDR_UPCTL_TCKSRE                (DWC_DDR_UMCTL_BASE_ADDR+0x124)     /* t_cksre timing Register */
#define ADDR_UPCTL_TCKSRX                (DWC_DDR_UMCTL_BASE_ADDR+0x128)     /* t_cksrx timing Register */
#define ADDR_UPCTL_TCKE                  (DWC_DDR_UMCTL_BASE_ADDR+0x12c)     /* t_cke timing Register */
#define ADDR_UPCTL_TMOD                  (DWC_DDR_UMCTL_BASE_ADDR+0x130)     /* t_mod timing Register */
#define ADDR_UPCTL_TRSTL                 (DWC_DDR_UMCTL_BASE_ADDR+0x134)     /* reset low timing Register */
#define ADDR_UPCTL_TZQCL                 (DWC_DDR_UMCTL_BASE_ADDR+0x138)     /* t_zqcl timing Register */
#define ADDR_UPCTL_TMRR                  (DWC_DDR_UMCTL_BASE_ADDR+0x13c)     /* t_mrr timing Register */
#define ADDR_UPCTL_TCKESR                (DWC_DDR_UMCTL_BASE_ADDR+0x140)     /* t_ckesr timing Register */
#define ADDR_UPCTL_TDPD                  (DWC_DDR_UMCTL_BASE_ADDR+0x144)     /* t_dpd timing Register */
                                                                                                
/* ECC Configuration,Control,and Status Registers */                                             
#define ADDR_UPCTL_ECCCFG                (DWC_DDR_UMCTL_BASE_ADDR+0x180)     /* ECC Configuration Register */
#define ADDR_UPCTL_ECCTST                (DWC_DDR_UMCTL_BASE_ADDR+0x184)     /* ECC Test Register */
#define ADDR_UPCTL_ECCCLR                (DWC_DDR_UMCTL_BASE_ADDR+0x188)     /* ECC Clear Register */
#define ADDR_UPCTL_ECCLOG                (DWC_DDR_UMCTL_BASE_ADDR+0x18c)     /* ECC Log Register */
                                                                                                
/* DTU Control and Status Registers */                                                           
#define ADDR_UPCTL_DTUWACTL              (DWC_DDR_UMCTL_BASE_ADDR+0x200)     /* DTU write address control Register */
#define ADDR_UPCTL_DTURACTL              (DWC_DDR_UMCTL_BASE_ADDR+0x204)     /* DTU read address control Register */
#define ADDR_UPCTL_DTUCFG                (DWC_DDR_UMCTL_BASE_ADDR+0x208)     /* DTU configuration Register */
#define ADDR_UPCTL_DTUECTL               (DWC_DDR_UMCTL_BASE_ADDR+0x20c)     /* DTU execute control Register */
#define ADDR_UPCTL_DTUWD0                (DWC_DDR_UMCTL_BASE_ADDR+0x210)     /* DTU write data #0 */
#define ADDR_UPCTL_DTUWD1                (DWC_DDR_UMCTL_BASE_ADDR+0x214)     /* DTU write data #1 */
#define ADDR_UPCTL_DTUWD2                (DWC_DDR_UMCTL_BASE_ADDR+0x218)     /* DTU write data #2 */
#define ADDR_UPCTL_DTUWD3                (DWC_DDR_UMCTL_BASE_ADDR+0x21c)     /* DTU write data #3 */
#define ADDR_UPCTL_DTUWDM                (DWC_DDR_UMCTL_BASE_ADDR+0x220)     /* DTU write data mask */
#define ADDR_UPCTL_DTURD0                (DWC_DDR_UMCTL_BASE_ADDR+0x224)     /* DTU read data #0 */
#define ADDR_UPCTL_DTURD1                (DWC_DDR_UMCTL_BASE_ADDR+0x228)     /* DTU read data #1 */
#define ADDR_UPCTL_DTURD2                (DWC_DDR_UMCTL_BASE_ADDR+0x22c)     /* DTU read data #2 */
#define ADDR_UPCTL_DTURD3                (DWC_DDR_UMCTL_BASE_ADDR+0x230)     /* DTU read data #3 */
#define ADDR_UPCTL_DTULFSRWD             (DWC_DDR_UMCTL_BASE_ADDR+0x234)     /* DTU LFSR seed for write data generation */
#define ADDR_UPCTL_DTULFSRRD             (DWC_DDR_UMCTL_BASE_ADDR+0x238)     /* DTU LFSR seed for read data generation */
#define ADDR_UPCTL_DTUEAF                (DWC_DDR_UMCTL_BASE_ADDR+0x23c)     /* DTU error address FIFO */
                                                                                                
/* DFI Control Registers */                                                                      
#define ADDR_UPCTL_DFITCTRLDELAY         (DWC_DDR_UMCTL_BASE_ADDR+0x240)     /* DFI tctrl_delay Register */
#define ADDR_UPCTL_DFIODTCFG             (DWC_DDR_UMCTL_BASE_ADDR+0x244)     /* DFI ODT Configuration Register */
#define ADDR_UPCTL_DFIODTCFG1            (DWC_DDR_UMCTL_BASE_ADDR+0x248)     /* DFI ODT Timing Configuration Register */
#define ADDR_UPCTL_DFIODTRANKMAP         (DWC_DDR_UMCTL_BASE_ADDR+0x24c)     /* DFI ODT Rank Mapping Register */
                                                                                                
/* DFI Write Data Registers */                                                                   
#define ADDR_UPCTL_DFITPHYWRDATA         (DWC_DDR_UMCTL_BASE_ADDR+0x250)     /* DFI tphy_wrdata Register */
#define ADDR_UPCTL_DFITPHYWRLAT          (DWC_DDR_UMCTL_BASE_ADDR+0x254)     /* DFI tphy_wrlat Register */
                                                                                                
/* DFI Read Data Registers */                                                                    
#define ADDR_UPCTL_DFITRDDATAEN          (DWC_DDR_UMCTL_BASE_ADDR+0x260)     /* DFI trddata_en Register */
#define ADDR_UPCTL_DFITPHYRDLAT          (DWC_DDR_UMCTL_BASE_ADDR+0x264)     /* DFI tphy_rddata Register */
                                                                                                 
/* DFI Update Registers */                                                                       
#define ADDR_UPCTL_DFITPHYUPDTYPE0       (DWC_DDR_UMCTL_BASE_ADDR+0x270)     /* DFI tphyupd_type0 Register */
#define ADDR_UPCTL_DFITPHYUPDTYPE1       (DWC_DDR_UMCTL_BASE_ADDR+0x274)     /* DFI tphyupd_type1 Register */
#define ADDR_UPCTL_DFITPHYUPDTYPE2       (DWC_DDR_UMCTL_BASE_ADDR+0x278)     /* DFI tphyupd_type2 Register */
#define ADDR_UPCTL_DFITPHYUPDTYPE3       (DWC_DDR_UMCTL_BASE_ADDR+0x27c)     /* DFI tphyupd_type3 Register */
#define ADDR_UPCTL_DFITCTRLUPDMIN        (DWC_DDR_UMCTL_BASE_ADDR+0x280)     /* DFI tctrlupd_min Register */
#define ADDR_UPCTL_DFITCTRLUPDMAX        (DWC_DDR_UMCTL_BASE_ADDR+0x284)     /* DFI tctrlupd_max Register */
#define ADDR_UPCTL_DFITCTRLUPDDLY        (DWC_DDR_UMCTL_BASE_ADDR+0x288)     /* DFI tctrlupd_dly Register */
#define ADDR_UPCTL_DFIUPDCFG             (DWC_DDR_UMCTL_BASE_ADDR+0x290)     /* DFI Update Configuration Register */
#define ADDR_UPCTL_DFITREFMSKI           (DWC_DDR_UMCTL_BASE_ADDR+0x294)     /* DFI Masked Refresh Interval Register */
#define ADDR_UPCTL_DFITCTRLUPDI          (DWC_DDR_UMCTL_BASE_ADDR+0x298)     /* DFI tctrlupd_interval Register */
                                                                                                
/* DFI Training Registers */                                                                     
#define ADDR_UPCTL_DFITRCFG0             (DWC_DDR_UMCTL_BASE_ADDR+0x2ac)     /* DFI Training Configuration 0 Register */
#define ADDR_UPCTL_DFITRSTAT0            (DWC_DDR_UMCTL_BASE_ADDR+0x2b0)     /* DFI Training Status 0 Register */
#define ADDR_UPCTL_DFIRWRLVLEN           (DWC_DDR_UMCTL_BASE_ADDR+0x2b4)     /* DFI Training dfi_wrlvl_en Register */
#define ADDR_UPCTL_DFIRRDLVLEN           (DWC_DDR_UMCTL_BASE_ADDR+0x2b8)     /* DFI Training dfi_rdlvl_en Register */
#define ADDR_UPCTL_DFIRRDLVLGATEEN       (DWC_DDR_UMCTL_BASE_ADDR+0x2bc)     /* DFI Training dfi_rdlvl_gate_en Register */
                                                                                                 
/* DFI Status Registers */                                                                       
#define ADDR_UPCTL_DFISTSTAT0            (DWC_DDR_UMCTL_BASE_ADDR+0x2c0)     /* DFI STATUS Status 0 Register */
#define ADDR_UPCTL_DFISTCFG0             (DWC_DDR_UMCTL_BASE_ADDR+0x2c4)     /* DFI STATUS Configuration 0 Register */
#define ADDR_UPCTL_DFISTCFG1             (DWC_DDR_UMCTL_BASE_ADDR+0x2c8)     /* DFI STATUS Configuration 1 Register */
#define ADDR_UPCTL_DFITDRAMCLKEN         (DWC_DDR_UMCTL_BASE_ADDR+0x2d0)     /* DFI tdram_clk_disable Register */
#define ADDR_UPCTL_DFITDRAMCLKDIS        (DWC_DDR_UMCTL_BASE_ADDR+0x2d4)     /* DFI tdram_clk_enable Register */
#define ADDR_UPCTL_DFISTCFG2             (DWC_DDR_UMCTL_BASE_ADDR+0x2d8)     /* DFI STATUS Configuration 2 Register */
#define ADDR_UPCTL_DFISTPARCLR           (DWC_DDR_UMCTL_BASE_ADDR+0x2dc)     /* DFI STATUS Parity Clear Register */
#define ADDR_UPCTL_DFISTPARLOG           (DWC_DDR_UMCTL_BASE_ADDR+0x2e0)     /* DFI STATUS Parity Log Register */
                                                                                                
/* DFI Low Power Registers */                                                                    
#define ADDR_UPCTL_DFILPCFG0             (DWC_DDR_UMCTL_BASE_ADDR+0x2f0)     /* DFI Low Power Configuration 0 Register */
                                                                                                 
/* DFI Training 2 Registers */                                                                  
#define ADDR_UPCTL_DFITRWRLVLRESP0       (DWC_DDR_UMCTL_BASE_ADDR+0x300)     /* DFI Training dfi_wrlvl_resp Status 0 */
#define ADDR_UPCTL_DFITRWRLVLRESP1       (DWC_DDR_UMCTL_BASE_ADDR+0x304)     /* DFI Training dfi_wrlvl_resp Status 1 */
#define ADDR_UPCTL_DFITRWRLVLRESP2       (DWC_DDR_UMCTL_BASE_ADDR+0x308)     /* DFI Training dfi_wrlvl_resp Status 2 */
#define ADDR_UPCTL_DFITRRDLVLRESP0       (DWC_DDR_UMCTL_BASE_ADDR+0x30c)     /* DFI Training dfi_rdlvl_resp Status 0 */
#define ADDR_UPCTL_DFITRRDLVLRESP1       (DWC_DDR_UMCTL_BASE_ADDR+0x310)     /* DFI Training dfi_rdlvl_resp Status 1 */
#define ADDR_UPCTL_DFITRRDLVLRESP2       (DWC_DDR_UMCTL_BASE_ADDR+0x314)     /* DFI Training dfi_rdlvl_resp Status 2 */
#define ADDR_UPCTL_DFITRWRLVLDELAY0      (DWC_DDR_UMCTL_BASE_ADDR+0x318)     /* DFI Training dfi_wrlvl_delay Configuration0 */ 
#define ADDR_UPCTL_DFITRWRLVLDELAY1      (DWC_DDR_UMCTL_BASE_ADDR+0x31c)     /* DFI Training dfi_wrlvl_delay Configuration1 */ 
#define ADDR_UPCTL_DFITRWRLVLDELAY2      (DWC_DDR_UMCTL_BASE_ADDR+0x320)     /* DFI Training dfi_wrlvl_delay Configuration2 */ 
#define ADDR_UPCTL_DFITRRDLVLDELAY0      (DWC_DDR_UMCTL_BASE_ADDR+0x324)     /* DFI Training dfi_rdlvl_delay Configuration0 */ 
#define ADDR_UPCTL_DFITRRDLVLDELAY1      (DWC_DDR_UMCTL_BASE_ADDR+0x328)     /* DFI Training dfi_rdlvl_delay Configuration1 */ 
#define ADDR_UPCTL_DFITRRDLVLDELAY2      (DWC_DDR_UMCTL_BASE_ADDR+0x32c)     /* DFI Training dfi_rdlvl_delay Configuration2 */ 
#define ADDR_UPCTL_DFITRRDLVLGATEDELAY0  (DWC_DDR_UMCTL_BASE_ADDR+0x330)     /* DFI Training dfi_rdlvl_gate_delay Configuration0 */ 
#define ADDR_UPCTL_DFITRRDLVLGATEDELAY1  (DWC_DDR_UMCTL_BASE_ADDR+0x334)     /* DFI Training dfi_rdlvl_gate_delay Configuration1 */ 
#define ADDR_UPCTL_DFITRRDLVLGATEDELAY2  (DWC_DDR_UMCTL_BASE_ADDR+0x338)     /* DFI Training dfi_rdlvl_gate_delay Configuration2 */ 
#define ADDR_UPCTL_DFITRCMD              (DWC_DDR_UMCTL_BASE_ADDR+0x33c)     /* DFI Training Command Register */
                                                                                                
/* IP Status Registers */                                                                        
#define ADDR_UPCTL_IPVR                  (DWC_DDR_UMCTL_BASE_ADDR+0x3f8)     /* IP Version Register */
#define ADDR_UPCTL_IPTR                  (DWC_DDR_UMCTL_BASE_ADDR+0x3fc)     /* IP Type Register */
                                                                                                
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

#endif /* imapx_dramc.h */
