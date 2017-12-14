

#ifndef _UBOOT_IDS_REG_H_
#define _UBOOT_IDS_REG_H_

#include <asm/arch/imapx_base_reg.h>

#define SYSMGR_IDS0_BASE        0x21E24000

#define IDS0_REG(x)     ((x) + IDS0_BASE_ADDR)
#define DSI_REG(x)      ((x) + MIPI_DSI_BASE_ADDR)
#define OVCDCR         IDS0_REG(0x1000)
#define OVCPCR         IDS0_REG(0x1004)
#define OVCBKCOLOR     IDS0_REG(0x1008)
#define OVCW0CR        IDS0_REG(0x1080)
#define OVCW0PCAR      IDS0_REG(0x1084)
#define OVCW0PCBR      IDS0_REG(0x1088)
#define OVCW0VSSR      IDS0_REG(0x108c)
#define OVCW0CMR       IDS0_REG(0x1090)
#define OVCW0B0SAR     IDS0_REG(0x1094)
#define OVCW0B1SAR     IDS0_REG(0x1098)
#define OVCW0B2SAR     IDS0_REG(0x109c)
#define OVCW0B3SAR     IDS0_REG(0x10a0)
#define OVCW1CR        IDS0_REG(0x1100) 
#define OVCW1PCAR      IDS0_REG(0x1104)
#define OVCW1PCBR      IDS0_REG(0x1108)
#define OVCW1PCCR      IDS0_REG(0x110c)
#define OVCW1VSSR      IDS0_REG(0x1110)
#define OVCW1CKCR      IDS0_REG(0x1114)
#define OVCW1CKR       IDS0_REG(0x1118) 
#define OVCW1CMR       IDS0_REG(0x111c) 
#define OVCW1B0SAR     IDS0_REG(0x1120)
#define OVCW1B1SAR     IDS0_REG(0x1124)
#define OVCW1B2SAR     IDS0_REG(0x1128)
#define OVCW1B3SAR     IDS0_REG(0x112c)
#define OVCW2CR        IDS0_REG(0x1180)    
#define OVCW2PCAR      IDS0_REG(0x1184)  
#define OVCW2PCBR      IDS0_REG(0x1188)  
#define OVCW2PCCR      IDS0_REG(0x118c)  
#define OVCW2VSSR      IDS0_REG(0x1190)  
#define OVCW2CKCR      IDS0_REG(0x1194)  
#define OVCW2CKR       IDS0_REG(0x1198)   
#define OVCW2CMR       IDS0_REG(0x119c)   
#define OVCW2B0SAR     IDS0_REG(0x11a0)
#define OVCW2B1SAR     IDS0_REG(0x11a4) 
#define VCW2B2SAR      IDS0_REG(0x11a8)
#define OVCW2B3SAR     IDS0_REG(0x11ac)
#define OVCW3CR        IDS0_REG(0x1200)  
#define OVCW3PCAR      IDS0_REG(0x1204)
#define OVCW3PCBR      IDS0_REG(0x1208)
#define OVCW3PCCR      IDS0_REG(0x120c)
#define OVCW3VSSR      IDS0_REG(0x1210)
#define OVCW3CKCR      IDS0_REG(0x1214)
#define OVCW3CKR       IDS0_REG(0x1218)
#define OVCW3CMR       IDS0_REG(0x121c)
#define OVCW3B0SAR     IDS0_REG(0x1220)
#define OVCW3SABSAR    IDS0_REG(0x1224)
#define OVCBRB0SAR     IDS0_REG(0x1300)
#define OVCBRB1SAR     IDS0_REG(0x1304)
#define OVCBRB2SAR     IDS0_REG(0x1308)
#define OVCBRB3SAR     IDS0_REG(0x130C)
#define OVCOMC         IDS0_REG(0x1310)
#define OVCOEF11       IDS0_REG(0x1314)
#define OVCOEF12       IDS0_REG(0x1318)
#define OVCOEF13       IDS0_REG(0x131c)
#define OVCOEF21       IDS0_REG(0x1320) 
#define OVCOEF22       IDS0_REG(0x1324)
#define OVCOEF23       IDS0_REG(0x1328)
#define OVCOEF31       IDS0_REG(0x132c)
#define OVCOEF32       IDS0_REG(0x1330)
#define OVCOEF33       IDS0_REG(0x1334)

// osd offset
#define OVCDCR_UpdateReg    (11)
#define OVCDCR_IFTYPE       (1)
#define OVCWxCR_BPPMODE     (1)
#define OVCWxCR_ENWIN       (0)

#define OVCWxPCAR_LeftTopY  (16)
#define OVCWxPCAR_LeftTopX  (0)
#define OVCWxPCBR_RightBotY (16)
#define OVCWxPCBR_RightBotX (0) 
#define OVCOMC_ToRGB        (31)
#define OVCOMC_oft_b        (8)
#define OVCOMC_oft_a        (0)

// lcd reg & offset
#define LCDCON1                 IDS0_REG(0x000)   //LCD control 1          
#define LCDCON2                 IDS0_REG(0x004)   //LCD control 2          
#define LCDCON3                 IDS0_REG(0x008)   //LCD control 3          
#define LCDCON4                 IDS0_REG(0x00c)   //LCD control 4          
#define LCDCON5                 IDS0_REG(0x010)   //LCD control 5          
#define LCDCON6                 IDS0_REG(0x018)                               
#define LCDVCLKFSR              IDS0_REG(0x030)                               
#define IDSINTPND               IDS0_REG(0x054)   //LCD Interrupt pending  
#define IDSSRCPND               IDS0_REG(0x058)   //LCD Interrupt source   
#define IDSINTMSK               IDS0_REG(0x05c)   //LCD Interrupt mask     
                                                                 
#define LCDCON1_LINECNT         18  // [29:18]                   
#define LCDCON1_CLKVAL          8   // [17:8]                    
#define LCDCON1_VMMODE          7   // [7:7]                     
#define LCDCON1_PNRMODE         5                                
#define LCDCON1_STNBPP          1                                
#define LCDCON1_ENVID           0   // [0:0]                     
                                                                 
#define LCDCON2_VBPD            16  // [26:16]                   
#define LCDCON2_VFPD            0   // [10:0]                    
                                                                 
#define LCDCON3_VSPW            16  // [26:16]                   
#define LCDCON3_HSPW            0   // [10:0]                    
                                                                 
#define LCDCON4_HBPD            16  // [26:16]                   
#define LCDCON4_HFPD            0   // [10:0]                    

#define LCDCON5_RGBORDER        24  // [29:24]                                                                               
#define LCDCON5_CONFIGORDER     20  // [22:20] 0->dsi24bpp, 1->dsi16bpp1, 2->dsi16bpp2,3->dsi16bpp3,4->dsi18bpp1,5->dsi18bpp2
#define LCDCON5_VSTATUS         15  // [16:15]                                                                               
#define LCDCON5_HSTATUS         13  // [14:13]                                                                               
#define LCDCON5_DSPTYPE         11  // [12:11]                                                                               
#define LCDCON5_INVVCLK         10  // [10:10]                                                                               
#define LCDCON5_INVVLINE        9   // [9:9]                                                                                 
#define LCDCON5_INVVFRAME       8   // [8:8]                                                                                 
#define LCDCON5_INVVD           7   // [7:7]                                                                                 
#define LCDCON5_INVVDEN         6   // [6:6]                                                                                 
#define LCDCON5_INVPWREN        5   // [5:5]                                                                                 
#define LCDCON5_PWREN           3   // [3:3]                                                                                 
                                                                                                                             
#define LCDCON6_LINEVAL         16  // [26:16]                                                                               
#define LCDCON6_HOZVAL          0   // [10:0]                                                                                


#define DATASIGMAPPING_RGB  0x6     // [5:4]2b'00, [3:2]2b'01, [1:0]2b'10. RGB
#define DATASIGMAPPING_RBG  0x9     // [5:4]2b'00, [3:2]2b'10, [1:0]2b'01. RBG
#define DATASIGMAPPING_GRB  0x12    // [5:4]2b'01, [3:2]2b'00, [1:0]2b'10. GRB
#define DATASIGMAPPING_GBR  0x18    // [5:4]2b'01, [3:2]2b'10, [1:0]2b'00. GBR
#define DATASIGMAPPING_BRG  0x21    // [5:4]2b'10, [3:2]2b'00, [1:0]2b'01. BRG
#define DATASIGMAPPING_BGR  0x24    // [5:4]2b'10, [3:2]2b'01, [1:0]2b'00. BGR


//Tag1:summer modification starts here                                          
#define LCD_VIC 2000
typedef unsigned short u16;                                                     
typedef unsigned char u8;                                                       
typedef struct {                                                                
        u16 mCode;  // VIC                                                             
        u16 mPixelRepetitionInput;                                                     
        u16 mPixelClock;                                                               
        u8 mInterlaced;                                                                
        u16 mHActive;                                                                  
        u16 mHBlanking;                                                                
        u16 mHBorder;                                                                  
        u16 mHImageSize;                                                               
        u16 mHSyncOffset;                                                              
        u16 mHSyncPulseWidth;                                                          
        u8 mHSyncPolarity;                                                             
        u8 mVclkInverse;                                                               
        u16 mVActive;                                                                  
        u16 mVBlanking;                                                                
        u16 mVBorder;                                                                  
        u16 mVImageSize;                                                               
        u16 mVSyncOffset;                                                              
        u16 mVSyncPulseWidth;                                                          
        u8 mVSyncPolarity;                                                             
} dtd_t;               

struct lcd_power_sequence {                                                     
        int operation;                                                            
        int delay;      // ms                                                       
};                                                                            
                                                                                  
struct lcd_panel_param {                                                        
        char * name;                                                              
        dtd_t dtd;                                                                
        int rgb_seq;                                                              
        int rgb_bpp;                                                              
        struct lcd_power_sequence * power_seq;                                    
        int power_seq_num;              // be convient for reverse enumerate        
        int (*lcd_enable)(int enable);      // Specific enable sequence             
        int (*power_specific)(int operation, int enable);   // Specific power operation
};                                                                              
                                                                                
enum lcd_power_sequences {                                                      
        LCDPWR_MASTER_EN,                                                         
        LCDPWR_OUTPUT_EN,                                                         
        LCDPWR_DVDD_EN,                                                           
        LCDPWR_AVDD_EN,                                                           
        LCDPWR_VGH_EN,                                                            
        LCDPWR_VGL_EN,                                                            
        LCDPWR_BACKLIGHT_EN,                                                      
        LCDPWR_END,                                                               
};                         

enum rgb_sequences {                                                            
        SEQ_RGB,                                                                  
        SEQ_RBG,                                                                  
        SEQ_GRB,                                                                  
        SEQ_GBR,                                                                  
        SEQ_BRG,                                                                  
        SEQ_BGR,                                                                  
};                                                                              
                                                                                  
                                                                                  
enum rgb_bpps {                                                                 
        ARGB0888,                                                                 
        RGB888,                                                                   
        RGB565,                                                                   
        RGB666,                                                                   
};                                                                              
                                                                                  
static struct lcd_power_sequence pwr_seq_1024_600[] = {                         
        {LCDPWR_MASTER_EN, 1},                                                    
        {LCDPWR_DVDD_EN, 1},                                                      
        {LCDPWR_AVDD_EN, 0},                                                      
        {LCDPWR_VGH_EN, 1},                                                       
        {LCDPWR_VGL_EN, 1},                                                       
        {LCDPWR_OUTPUT_EN, 100},                                                  
        {LCDPWR_BACKLIGHT_EN, 0},                                                 
};                      

static struct lcd_power_sequence pwr_seq_800_480[] = {                          
        {LCDPWR_MASTER_EN, 1},                                                    
        {LCDPWR_DVDD_EN, 30},                                                     
        {LCDPWR_AVDD_EN, 20},                                                     
        {LCDPWR_OUTPUT_EN, 1},      // 10 VSD, 317ms                                
        {LCDPWR_BACKLIGHT_EN, 0},                                                 
};                                                                              
                                                                                  
static struct lcd_power_sequence pwr_seq_768_1024[] = {                         
        {LCDPWR_MASTER_EN, 1},                                                    
        {LCDPWR_DVDD_EN, 5},                                                      
        {LCDPWR_AVDD_EN, 0},                                                      
        {LCDPWR_VGH_EN, 0},                                                       
        {LCDPWR_VGL_EN, 17},                                                      
        {LCDPWR_OUTPUT_EN, 100},                                                  
        {LCDPWR_BACKLIGHT_EN, 0},                                                 
};                             

static struct lcd_power_sequence pwr_seq_GM05004001Q_800_480[] = {
	{LCDPWR_MASTER_EN, 1},		 // LCD_BEN, K17, GPIO119, controls main power of LCD.
//	{LCDPWR_DVDD_EN, 30},
//	{LCDPWR_AVDD_EN, 20},
//	{LCDPWR_OUTPUT_EN, 1},		// 10 VSD, 317ms
//	{LCDPWR_BACKLIGHT_EN, 250},
	{LCDPWR_BACKLIGHT_EN, 0},	// LCD_BL_EN, GPIO109, controls LED+ and LED- power of LCD
};

                                            
#endif  //_UBOOT_IDS_REG_H_


