/***************************************************************************** 
 ** infotm/drivers/display/display.c 
 ** 
 ** Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved. 
 ** 
 ** Use of Infotm's code is governed by terms and conditions 
 ** stated in the accompanying licensing statement. 
 ** 
 ** Description: File used for the standard function interface of output.
 **
 ** Author:  XXX by who? <mailbox> XXX
 **      
 ** Revision History: 
 ** ----------------- 
 ** 1.0  11/27/2012  XXX by who? XXX
 **      Description:
 **          To add logo in uboot start-up, we assume fblayer is reserved for LCD/MIPI.. display.
 **          No vmode, no extlayer, just local display
 **      Attention !!!!!!::
 **          Default :   all the ids relative clock is under EPLL, which is 888M
 **
 **
 **
 *****************************************************************************/ 

#include <common.h>
#include <asm/io.h>
#include <lowlevel_api.h>
#include <efuse.h>
#include <preloader.h>
#include <items.h>
#include <display.h>
#include "ids_reg.h"
#include <rballoc.h>
#include <linux/string.h>
//#include "R.h"                                                                    
//#include "G.h"                                                                    
//#include "B.h"
//new add
#include <ius.h>
#include <jpegdecapi.h>
#include <ppapi.h>
//
#include <pwm.h>
#define DISP_TYPE_LCD    1
#define DISP_TYPE_DSI    2
#define DISP_TYPE_HDMI   3
#define EPLL_CLK        1188000000
#define MAX_POWER_ON_STEPS   7

#define POWER_TYPE_PADS  1
#define POWER_TYPE_PMU   2
#define POWER_TYPE_RGB   3
#define POWER_TYPE_BL    4

/*(1)Author:summer
 *(2)Date:2014-6-23
 *(3)Reason:
 *   Close debug printf message.
 * */
//#define DISP_DEBUG

#ifdef DISP_DEBUG
#define IM_DEBUG(str,args...) printf(str,##args)
#else
#define IM_DEBUG(str,args...)
#endif
int displaylogo = 0 ;
struct lcdc_config_t{
    int width;
    int height;

    int vspw; //mVSyncPulseWidth
    int vbpd; //mVBlanking - mVSyncOffset -mVSyncPulseWidth
    int vfpd; //mVSyncOffset
    int hspw;
    int hbpd;
    int hfpd;

    int signalPwrenEnable;//1
    int signalPwrenInverse;//0
    int signalVclkInverse; //plority
    int signalHsyncInverse;
    int signalVsyncInverse;
    int signalDataInverse;//0
    int signalVdenInverse;//0

    int SignalDataMapping;//DATASIGMAPPING_RGB???
    int fpsx1000;//60000

    int phyWidth;//0
    int phyHeight;//0
};

typedef void (*step_func)(void *param);

typedef struct{
    step_func  func;
    int power_type;
    char power_str[16];
    int slp_time; // in ms
}power_on_sequence;


/*
 *(1)Tag1:summer modification starts here
 *################# Take Care #####################
 *(2)If you want to add one entry to the array,please add at the end of the array!!!!
*/
struct lcd_panel_param screen_params[] = {
[0] = {
	  .name = "KR070LB0S_1024_600",                                                  
      .dtd = {                                                                       
          .mCode = LCD_VIC,                                                          
		  .mHImageSize = 154, // mm. Different use with HDMI                         
          .mVImageSize = 90,                                                         
          .mHActive = 1024,                                                          
          .mVActive = 600,                                                           
          .mHBlanking = 320,                                                         
          .mVBlanking = 35,                                                          
          .mHSyncOffset = 80,                                                        
		  .mVSyncOffset = 6,                                                         
          .mHSyncPulseWidth = 80,                                                    
          .mVSyncPulseWidth = 6,                                                  
          .mHSyncPolarity = 0,                                                    
          .mVSyncPolarity = 0,                                                    
          .mVclkInverse   = 0,                                                    
          .mPixelClock = 5120,    // 60 fps                                       
      },                                                                          
      .rgb_seq = SEQ_RGB,                                                         
      .rgb_bpp = RGB888,                                                          
      .power_seq = pwr_seq_1024_600,                                                       
      .power_seq_num = sizeof(pwr_seq_1024_600)/sizeof(struct lcd_power_sequence), 
},
[1] = {
 .name = "KR070PB2S_800_480",                                                
      .dtd = {                                                                    
          .mCode = LCD_VIC,                                                       
          .mHImageSize = 154, // mm. Different use with HDMI                      
          .mVImageSize = 86,                                                      
          .mHActive = 800,                                                        
          .mVActive = 480,                                                        
          .mHBlanking = 256,                                                      
          .mVBlanking = 45,                                                       
          .mHSyncOffset = 210,                                                    
          .mVSyncOffset = 22,                                                     
          .mHSyncPulseWidth = 30,                                                 
          .mVSyncPulseWidth = 13,                                                 
          .mHSyncPolarity = 0,                                                    
          .mVSyncPolarity = 0,                                                    
          .mVclkInverse   = 1,                                                    
          .mPixelClock = 3326,    // 60 fps                                       
      },                                                                          
      .rgb_seq = SEQ_RGB,                                                         
      .rgb_bpp = RGB888,                                                          
      .power_seq = pwr_seq_800_480,                                                       
      .power_seq_num = sizeof(pwr_seq_800_480)/sizeof(struct lcd_power_sequence),    

},
[2] = {
	 .name = "KR079LA1S_768_1024",                                               
      .dtd = {                                                                    
          .mCode = LCD_VIC,                                                       
          .mHImageSize = 120, // mm. Different use with HDMI                      
          .mVImageSize = 160,                                                     
          .mHActive = 768,                                                        
          .mVActive = 1024,                                                       
          .mHBlanking = 160,                                                      
          .mVBlanking = 41,                                                       
          .mHSyncOffset = 40,                                                     
          .mVSyncOffset = 9,                                                      
          .mHSyncPulseWidth = 40,                                                 
          .mVSyncPulseWidth = 9,                                                  
          .mHSyncPolarity = 1,                                                    
          .mVSyncPolarity = 1,                                                    
          .mVclkInverse   = 0,                                                    
          .mPixelClock = 5930,    // 60 fps                                       
      },                                                                          
      .rgb_seq = SEQ_RGB,                                                         
      .rgb_bpp = RGB888,                                                          
      .power_seq = pwr_seq_768_1024,                                                       
      .power_seq_num = sizeof(pwr_seq_768_1024)/sizeof(struct lcd_power_sequence),   
},

//for box
[3] = {
	 .name = "KR079LA1S_720_1280",                                               
      .dtd = {                                                                    
          .mCode = LCD_VIC,                                                       
          .mHImageSize = 120, // mm. Different use with HDMI                      
          .mVImageSize = 160,                                                     
          .mHActive = 1280,                                                        
          .mVActive = 720,                                                       
          .mHBlanking = 370,                                                      
          .mVBlanking = 30,                                                       
          .mHSyncOffset = 110,                                                     
          .mVSyncOffset = 5,                                                      
          .mHSyncPulseWidth = 40,                                                 
          .mVSyncPulseWidth = 5,                                                  
          .mHSyncPolarity = 1,                                                    
          .mVSyncPolarity = 1,                                                    
          .mVclkInverse   = 0,                                                    
          .mPixelClock = 7425,    // 60 fps                                       
      },                                                                          
      .rgb_seq = SEQ_RGB,                                                         
      .rgb_bpp = RGB888,                                                          
      .power_seq = pwr_seq_768_1024,                                                       
      .power_seq_num = sizeof(pwr_seq_768_1024)/sizeof(struct lcd_power_sequence),   
    },

//added by Eason 2014/12/27
[4] = {
 .name = "GM05004001Q_800_480",                                                
      .dtd = {                                                                    
          .mCode = LCD_VIC,                                                       
          .mHImageSize = 154, // mm. Different use with HDMI                      
          .mVImageSize = 86,                                                      
          .mHActive = 800,                                                        
          .mVActive = 480,                                                        
          .mHBlanking = 256,                                                      
          .mVBlanking = 45,                                                       
          .mHSyncOffset = 210,                                                    
          .mVSyncOffset = 22,                                                     
          .mHSyncPulseWidth = 30,                                                 
          .mVSyncPulseWidth = 13,                                                 
          .mHSyncPolarity = 0,                                                    
          .mVSyncPolarity = 0,                                                    
          .mVclkInverse   = 1,                                                    
          .mPixelClock = 3326,    // 60 fps                                       
      },                                                                          
      .rgb_seq = SEQ_RGB,                                                         
      .rgb_bpp = RGB888,                                                          
      .power_seq = pwr_seq_GM05004001Q_800_480,                                                       
      .power_seq_num = sizeof(pwr_seq_GM05004001Q_800_480)/sizeof(struct lcd_power_sequence),    

},
		  
NULL,
};

//Tag1:summer modification ends here



void gpio_set(int param)
{
    // If pin num greater than 0, OK
    // If pin num less than 0, it is not correctly implemented in uboot yet.
    int pin = (param > 0)? param : (-param);
    IM_DEBUG(" gpio_set pin(%d) = %d -- \n",param,pin);
    gpio_mode_set(1, pin);
    gpio_dir_set(1, pin);
    gpio_output_set(((param> 0) ? 1 : 0), pin);
    return ;
}

void pmu_set(void *param)
{
    char *pstr = (char*)param;
    //.. to enable pmu power supply 
    // API not supported yet.
    // according to Jack, for out publick pcb version,
    //  pmu is default power on , no sw could manupulate them.
    //  But some private pcb version may have exceptions, so this
    //  api must be implemented also.
    pmu_regulator_set_voltage(pstr, 3300);
    pmu_regulator_en(pstr, 1);
    return ;
}

static  uint8_t *idslogo_buffer = NULL ;
static  uint8_t load_logo_from_nand_res= 0; //0---not success; 1---success
int ids_get_logo(int *buf, int imgWidth, int imgHeight,int number)
{
    int ret = 0;
#if 0
    unsigned int *addrIn, inLen;
    JpegDecInst mJpegDecInst = NULL;     
    JpegDecBuild mJpegBuild;      
    JpegDecInput mJpegDecIn;      
    JpegDecOutput mJpegDecOut;    
    JpegDecImageInfo mJpegDecInfo;
    JpegDecRet  jpegRet;
    PPInst mPpInst = NULL;
    PPConfig mPpCfg;
    PPResult mPpRet;
//    uint8_t *idslogo_buffer ;
    struct ius_desc *desc;

    addrIn =0;     // input addr;
    inLen = 0;     // source len;
	
	if(NULL == idslogo_buffer)
		idslogo_buffer = rballoc("idslogo",0x500000);
//	return 0;//nand-boot kernel ok

	//load logo from nand will take 40ms,so,there is no need to load twice
	if( 0 == load_logo_from_nand_res)
		//if load success,return 0;else return true value
		load_logo_from_nand_res= dlib_get_logo(idslogo_buffer); 
    if( load_logo_from_nand_res){
		load_logo_from_nand_res= 0; //When the function was called again,load again
    }else {
		load_logo_from_nand_res= 1; //When the function was called again,not load anymore
	}
    IM_DEBUG("succeed reading logo from nand\n");
	int count = 0;
	count = ius_get_count( (struct ius_hdr*)idslogo_buffer);
    if( count < number + 1 ){
        IM_DEBUG("fail to read logo number from nand,count=%d,number=%d\n",count,number);
        return -1;
    }
    desc = ius_get_desc((struct ius_hdr*) idslogo_buffer,number);

    addrIn = ( (desc->sector & 0xffffff) << 9 ) + idslogo_buffer;
    inLen =ius_desc_length(desc);
    // printf("dec_sector value = 0x%x\n", (desc->sector & 0xffffff) << 9);
    //   printf("isdlogo_buffer = 0x%x\n", idslogo_buffer);
    IM_DEBUG("addrIn=0x%x\n", addrIn);
    IM_DEBUG("inLen=0x%x\n", inLen);

	//return 0;
    jpegRet = JpegDecInit(&mJpegDecInst); 
    if(jpegRet != JPEGDEC_OK){
        IM_DEBUG("JPEGDecInit Failed error = %d\n", jpegRet);
        return -1;                         
    }
	//return 0;
    mPpRet = PPInit(&mPpInst);                                    
    if (PP_OK != mPpRet) {                                        
        IM_DEBUG("PPInit() failed, mPpRet=%d\n", mPpRet);
        return -1;
    }

    mPpRet = PPDecCombinedModeEnable(mPpInst, (const void*)mJpegDecInst, PP_PIPELINED_DEC_TYPE_JPEG);
    if(mPpRet != PP_OK)
    {
        IM_DEBUG("PPDecCombinedModeEnable failed,mPpRet=%d\n",mPpRet);
        ret = -1;
        goto Failed;
    }
    //
    // to get the input encoded logo source first 
    // 
    memset(&mJpegDecIn,0,sizeof(JpegDecInput));
    mJpegDecIn.streamBuffer.pVirtualAddress  = (unsigned int*)addrIn;
    mJpegDecIn.streamBuffer.busAddress = (unsigned int)addrIn;
    mJpegDecIn.streamLength  = inLen;
    mJpegDecIn.decImageType = 0;


    jpegRet = JpegDecGetImageInfo(mJpegDecInst, &mJpegDecIn, &mJpegDecInfo);
    if(jpegRet != JPEGDEC_OK)
    {
        IM_DEBUG("Mjpeg get Image Info failed,jpegRet=%d\n",jpegRet);
        ret = -1;
        goto Failed;
    }
    if((mJpegDecInfo.codingMode & JPEGDEC_PROGRESSIVE) || (mJpegDecInfo.codingMode & JPEGDEC_NONINTERLEAVED))                                 
    {
        IM_DEBUG("HW jpeg decoder not support %s stream!!!\n", (mJpegDecInfo.codingMode&JPEGDEC_PROGRESSIVE)? "progressive" :"non interleaved");
        ret = -1;
        goto Failed;
    }
    IM_DEBUG("Jpeg Info::\n");                                                
    IM_DEBUG("   displayWidth = %d\n", mJpegDecInfo.displayWidth);            
    IM_DEBUG("   displayHeight = %d\n", mJpegDecInfo.displayHeight);          
    IM_DEBUG("   outputWidth = %d\n", mJpegDecInfo.outputWidth);              
    IM_DEBUG("   outputHeight = %d\n", mJpegDecInfo.outputHeight);            
    IM_DEBUG("   version = %d\n", mJpegDecInfo.version);                      
    IM_DEBUG("   units = %d\n", mJpegDecInfo.units);                          
    IM_DEBUG("   xDensity = %d\n", mJpegDecInfo.xDensity);                    
    IM_DEBUG("   yDensity = %d\n", mJpegDecInfo.yDensity);                    
    IM_DEBUG("   outputFormat = %x\n", mJpegDecInfo.outputFormat);            
    IM_DEBUG("   codingMode = %d\n", mJpegDecInfo.codingMode);                
    IM_DEBUG("   thumbnailType = %d\n", mJpegDecInfo.thumbnailType);          
    IM_DEBUG("   displayHeightThumb = %d\n", mJpegDecInfo.displayHeightThumb);
    IM_DEBUG("   outputWidthThumb = %d\n", mJpegDecInfo.outputWidthThumb);    
    IM_DEBUG("   outputHeightThumb = %d\n", mJpegDecInfo.outputHeightThumb);  
    IM_DEBUG("   outputFormatThumb = %d\n", mJpegDecInfo.outputFormatThumb);  
    IM_DEBUG("   codingModeThumb = %d\n", mJpegDecInfo.codingModeThumb);      
    IM_DEBUG("Jpeg Info:: END\n");                                            

    mPpRet = PPGetConfig(mPpInst, &mPpCfg);
    if (PP_OK != mPpRet) {
        IM_DEBUG("PPGetConfig() failed, mPpRet=%d\n", mPpRet);
        ret = -1;
        goto Failed;
    }

    switch (mJpegDecInfo.outputFormat) {
        case JPEGDEC_YCbCr400:
            mPpCfg.ppInImg.pixFormat = PP_PIX_FMT_YCBCR_4_0_0;
            break;
        case JPEGDEC_YCbCr420_SEMIPLANAR:
            mPpCfg.ppInImg.pixFormat = PP_PIX_FMT_YCBCR_4_2_0_SEMIPLANAR;
            break;
        case JPEGDEC_YCbCr422_SEMIPLANAR:
            mPpCfg.ppInImg.pixFormat = PP_PIX_FMT_YCBCR_4_2_2_SEMIPLANAR;
            break;
        case JPEGDEC_YCbCr440:
            mPpCfg.ppInImg.pixFormat = PP_PIX_FMT_YCBCR_4_4_0;
            break;
        case JPEGDEC_YCbCr444_SEMIPLANAR:
            mPpCfg.ppInImg.pixFormat = PP_PIX_FMT_YCBCR_4_4_4_SEMIPLANAR;
            break;
        default:
            IM_DEBUG("Do not support this jpeg output format(%d)\n", mJpegDecInfo.outputFormat);
            ret = -1;
            goto Failed;
    }
    mPpCfg.ppInImg.picStruct = PP_PIC_FRAME_OR_TOP_FIELD;
    mPpCfg.ppInImg.videoRange = 0;                       
    mPpCfg.ppInImg.width = mJpegDecInfo.outputWidth;     
    mPpCfg.ppInImg.height = mJpegDecInfo.outputHeight;   
    mPpCfg.ppInImg.bufferBusAddr = 0;                    
    mPpCfg.ppInImg.bufferCbBusAddr = 0;                  
    mPpCfg.ppInCrop.enable = 0;
    mPpCfg.ppInRotation.rotation = PP_ROTATION_NONE;

    mPpCfg.ppOutImg.pixFormat = PP_PIX_FMT_RGB32;
    mPpCfg.ppOutRgb.alpha = 255;
    mPpCfg.ppOutImg.width = imgWidth;
    mPpCfg.ppOutImg.height = imgHeight;
    mPpCfg.ppOutImg.bufferBusAddr = (u32)buf;
    mPpCfg.ppOutImg.bufferChromaBusAddr = 0;

    mPpCfg.ppOutMask1.enable = 0;
    mPpCfg.ppOutMask2.enable = 0;
    mPpCfg.ppOutFrmBuffer.enable = 0;
    mPpCfg.ppOutDeinterlace.enable = 0;

    mPpRet = PPSetConfig(mPpInst, &mPpCfg);                         
    if (PP_OK != mPpRet) {
        IM_DEBUG("PPSetConfig() failed, ret=%d\n", mPpRet);
        ret = -1;
        goto Failed;
    }                                                               

//	printf("=============ids_get_logo summer \n");
//	return 0;
    do{
		dcache_flush();
		dcache_disable(); 
        jpegRet = JpegDecDecode(mJpegDecInst, &mJpegDecIn, &mJpegDecOut);
        switch(jpegRet)
        {
            case JPEGDEC_FRAME_READY:
                break;
            default:
                IM_DEBUG("\njped decode error happen,err %d\n", jpegRet);
                ret = -1;
				dcache_enable();
                goto Failed;
        }
		dcache_enable();
    }while(jpegRet != JPEGDEC_FRAME_READY);
//	return 0;//boot from nand,ok
    mPpRet = PPGetResult(mPpInst);
    if (PP_OK != mPpRet) {
        printf("PPGetResult() failed, ret=%d\n", mPpRet);
        ret = -1;
        goto Failed;
    }
	//return 0;//failed
    IM_DEBUG(" ####### Enter :: %s ##############\n",__func__);
	return ret;

Failed:
    if (mPpInst != NULL)
        PPRelease(mPpInst);
    if (mJpegDecInst != NULL)
        JpegDecRelease(mJpegDecInst);
#endif    
    return ret;
}
static void module_power_on(int idsx)
{
    int i;
    IM_DEBUG(" ####### Enter :: %s ##############\n",__func__);
    // system manager power on
    // ATTENTION::: mipi clk system management not enabled, later add it .
    module_enable(IDS1_BASE_ADDR);
    //model_reset

    // fix bus1/ids0-osd/ids0-eitf clock binded to epll, which should be 888M
    writel(0x02, 0x21e0a130); // bus1
    writel(EPLL_CLK/300000000, 0x21e0a134); // bus1 fixed to 300M = 888M/3
    writel(0x02, IDS0EITF_CLK_BASE);
    writel(0x02, IDS0OSD_CLK_BASE);

    // default, flush regs directly.
    writel(readl(OVCDCR)|(0x1<<OVCDCR_UpdateReg), OVCDCR);
    IM_DEBUG(" 0x22001000 = 0x%x --\n",readl(0x22001000));

    // flush yuv2rgb matrix.
    writel((1<<OVCOMC_ToRGB) | (16<<OVCOMC_oft_b) | (0<<OVCOMC_oft_a), OVCOMC);
    writel(298, OVCOEF11);
    writel(0, OVCOEF12);
    writel(409, OVCOEF13);
    writel(298, OVCOEF21);
    writel((unsigned int)-100, OVCOEF22);
    writel((unsigned int)-208, OVCOEF23);
    writel(298, OVCOEF31);
    writel(516, OVCOEF32);
    writel(0, OVCOEF33);
    return; 
}

static int dsi_configure(struct lcdc_config *cfg)
{
    return 0;
}

static int lcd_configure(struct lcdc_config_t *cfg, void *addr)
{
    int div1,div2,clkdiv,i, val, rgb_order;
    int imgWidth,imgHeight,acmlevel = 0;
    unsigned int pixTotal,clock,clkdiff,valdiff,resclk,Resclk;
    unsigned int rclk,rfpsx1000;
    char str[ITEM_MAX_LEN];

    IM_DEBUG("================= func %s start\n",__func__);
    IM_DEBUG("width =%d - \n",cfg->width);
    IM_DEBUG("height=%d - \n",cfg->height);
    IM_DEBUG("vspw  =%d - \n",cfg->vspw);
    IM_DEBUG("vbpd  =%d - \n",cfg->vbpd);
    IM_DEBUG("vfpd  =%d - \n",cfg->vfpd);
    IM_DEBUG("hspw  =%d - \n",cfg->hspw);
    IM_DEBUG("hbpd  =%d - \n",cfg->hbpd);
    IM_DEBUG("hfpd  =%d - \n",cfg->hfpd);
    IM_DEBUG("fpsx1000=%d - \n",cfg->fpsx1000);

    imgWidth = cfg->width & (~0x07);
    imgHeight = cfg->height & (~0x03);

    rgb_order = cfg->SignalDataMapping;
    if(item_exist("lcd.rgb")){
        item_string(str,"lcd.rgb",0);
    }else if (item_exist("ids.loc.dev0.rgb_order")){
        item_string(str, "ids.loc.dev0.rgb_order", 0);
    }
    if(strcmp(str,"rgb") == 0){
        rgb_order = DATASIGMAPPING_RGB;
    }
    else if(strcmp(str,"rbg") ==0){
        rgb_order = DATASIGMAPPING_RBG;
    }
    else if(strcmp(str,"grb") ==0){
        rgb_order = DATASIGMAPPING_GRB;
    }
    else if(strcmp(str,"gbr") ==0){
        rgb_order = DATASIGMAPPING_GBR;
    }
    else if(strcmp(str,"brg") ==0){
        rgb_order = DATASIGMAPPING_BRG;
    }
    else if(strcmp(str,"bgr") ==0){
        rgb_order = DATASIGMAPPING_BGR;
    }

    if(item_exist("ids.acm")){
        acmlevel = item_integer("ids.acm", 0);
    }else if(item_exist("ids.loc.dev0.acm_level")){
        acmlevel = item_integer("ids.loc.dev0.acm_level", 0);
    }
    // calc the clock needed , fps is fixed to 60
    pixTotal = (cfg->width + cfg->hspw + cfg->hbpd + cfg->hfpd) * (cfg->height + cfg->vspw + cfg->vfpd + cfg->vbpd);
    IM_DEBUG(" pixTotal = %d - \n",pixTotal);
    clock = pixTotal / 1000 * cfg->fpsx1000  ;
    IM_DEBUG(" clock = %d - \n",clock);
    // div = EPLL_CLK / clock;
    clkdiff = 20000000; // 20M clk
    for(div2=1; div2<=7 ;div2++)
    {
        div1 = EPLL_CLK / (clock * div2);
        for(i=0; i<2 ;i++)
        {
            resclk = EPLL_CLK / ( (div1 + i) * div2);
            valdiff = (resclk > clock) ? (resclk - clock) : (clock - resclk);
            if (valdiff < clkdiff) {
                clkdiff = valdiff;
                clkdiv = div2;
                rclk = resclk; //Polling the closest frequency
            }
        }
    }
    div2 = clkdiv;
    rfpsx1000 = ( rclk + pixTotal / 2) / pixTotal * 1000 ;
    rbsetint("rclk", rclk);
    rbsetint("rfpsx1000",rfpsx1000);
    rbsetint("div2", div2);
    IM_DEBUG(" rclk = %d - \n",rclk);
    IM_DEBUG(" rfpsx1000 = %d - \n",rfpsx1000);
    div1 = EPLL_CLK / (rclk * div2);
    if ((div1 * (rclk * div2) + ((rclk * div2) >> 1)) < EPLL_CLK){
        div1 ++;
    } 

    while(div1 >= 32){
        div2++;
        div1 = EPLL_CLK / (rclk * div2);
        if ((div1 * (rclk * div2) + ((rclk * div2) >> 1)) < EPLL_CLK){
            div1 ++;
        } 
    }
    IM_DEBUG(" div1 = %d - \n",div1);
    IM_DEBUG(" div2 = %d - \n",div2);
    if(displaylogo != 1){
        writel(div1 - 1, IDS0EITF_CLK_BASE + 0x04);
        writel((div1 >> 1) - 1, IDS0OSD_CLK_BASE + 0x04);
    }


    // to configure the ids set and show the logo
    // osd 
    if(displaylogo != 1){
        writel(0x0B << OVCWxCR_BPPMODE, OVCW0CR);// set panorama
        writel(imgWidth, OVCW0VSSR);
    }
    writel((uint32_t)addr, OVCW0B0SAR);//set buffer
    udelay(10*1000);
    val = (((cfg->width-imgWidth)>>1) << OVCWxPCAR_LeftTopX) | (((cfg->height-imgHeight)>>1) << OVCWxPCAR_LeftTopY);
    writel(val, OVCW0PCAR);//set position
    val = ((((cfg->width-imgWidth)>>1) + imgWidth-1) << OVCWxPCBR_RightBotX) | 
        ((((cfg->height-imgHeight)>>1) + imgHeight-1) << OVCWxPCBR_RightBotY);
    writel(val, OVCW0PCBR);
    // writel(0, OVCW0CMR);//set mapclr newadd
    writel(readl(OVCW0CR) | (1<<OVCWxCR_ENWIN), OVCW0CR);
    //    writel((uint32_t)addr, OVCW0B0SAR);//set region
    // lcd 
    val = ((cfg->vbpd-1) << LCDCON2_VBPD) | ((cfg->vfpd-1) << LCDCON2_VFPD);
    writel(val, LCDCON2);
    val = ((cfg->vspw-1) << LCDCON3_VSPW) | ((cfg->hspw-1) << LCDCON3_HSPW);
    writel(val, LCDCON3);
    val = ((cfg->hbpd-1) << LCDCON4_HBPD) | ((cfg->hfpd-1) << LCDCON4_HFPD);
    writel(val, LCDCON4);
    val = (rgb_order << LCDCON5_RGBORDER) | (0 << LCDCON5_DSPTYPE) |
        (cfg->signalVclkInverse << LCDCON5_INVVCLK) | (cfg->signalHsyncInverse << LCDCON5_INVVLINE) |
        (cfg->signalVsyncInverse << LCDCON5_INVVFRAME) | (cfg->signalDataInverse << LCDCON5_INVVD) | 
        (cfg->signalVdenInverse << LCDCON5_INVVDEN) | (cfg->signalPwrenInverse << LCDCON5_INVPWREN) |
        (cfg->signalPwrenEnable << LCDCON5_PWREN);                                                   
    writel(val, LCDCON5);
    val = ((cfg->height -1) << LCDCON6_LINEVAL) | ((cfg->width -1) << LCDCON6_HOZVAL);                
    writel(val, LCDCON6);
    val = ((div2 - 1) << LCDCON1_CLKVAL) | (0 << LCDCON1_VMMODE) | (3 << LCDCON1_PNRMODE) |           
        (12 << LCDCON1_STNBPP) | (0<<LCDCON1_ENVID);                                                    
    if(displaylogo != 1){
        writel(val, LCDCON1);
    }
    writel(readl(LCDVCLKFSR)|0xF000000, LCDVCLKFSR);
    val = ((cfg->height -1) << 16 ) | ((cfg->width -1) << 0);
    writel(val,0x22005800);
    val =  readl(0x22005804);
#if 0
    val &= (0 << 3)| (0 << 5) |(0 << 7);
	printf("bf1 5804 = 0x%x\n",val);
    writel(val,0x22005804);
	printf("af1 5804 = 0x%x\n",readl(0x22005804));
#endif
	val &=  ~((1 << 3)| (1 << 5)|(1 << 7) |(1 << 17));
    writel(val,0x22005804);
	acmlevel = 60;
    val = acmlevel * 50 + 1000;
	
    IM_DEBUG("acmval=%d\n",val);
    writel(val,0x2200588c);
    val = readl(0x2200588c);
    IM_DEBUG("read acmval=%d\n",val);
    val =0x800;
    writel(val,0x22005890);
    val =0x800;
    writel(val,0x22005894);
    val =0x800;
    writel(val,0x22005898);
    val =0x800;
    writel(val,0x2200589c);
#if 0
    val =  readl(0x22005804);
	printf("bf 5804 = 0x%x\n",val);
    val &= ~(1 << 17);
	printf("af 5804 = 0x%x\n",val);
    writel(val,0x22005804);
#endif
	/*  
	printf("5804 = 0x%x\n",readl(0x22005804));
	printf("5800 = 0x%x\n",readl(0x22005800));
	printf("588c = 0x%x\n",readl(0x2200588c));
	*/
	/*  
	val = readl(0x22000010);
	val |= 0x6 << 24;
	writel(val,0x22000010);
	*/
    IM_DEBUG("================= func %s end\n",__func__);
    return 0;
}
#define is_digit(c) ((c) >= '0' && (c) <= '9') 
static int skip_atoi(const char **s)
{
    int i=0;

    while (is_digit(**s))
        i = i*10 + *((*s)++) - '0';
    return i;
}


/* display LOGO
 * @number: logo.isi may contain multiple pictures, this parameter
 *          indicates which picture is to be displayed.
 */
void display_logo(int number)
{
    int i,j, param=0, padNum, disp_type = 0;
    int imgWidth, imgHeight,bl_power_index;
    int *addr,*p;
    int bl_freq , intensity;
    struct lcdc_config_t cfg;
    char str[ITEM_MAX_LEN];
    char *str1,*params = NULL;
    static int dis_flag = 0;
	int  screenDeviceType = 0;
	int R_length = 0 ;                                                      
  	int G_length = 0 ;                                                      
  	int B_length = 0 ;                                                      
  	int offset = 0;                                                         
  	int status = get_ius_card_state();                                      
  	int burn_status = 0;                                                    
  	int ret_val = 0;                                                        
	int load_time = 0;
  
 	if( status != 0xaaaaeeee)                                               
 		burn_status = 1; //boot from nand                               
    else                                                                    
  		burn_status = 2;//boot from ius 

	   
    if (dis_flag == 1)
	    return;
    dis_flag = 1;

    IM_DEBUG(" ####### Enter :: %s ##############\n",__func__);
    // uboot logo only supported for dsi & hdmi 
    if(item_exist("ids.default")){
        item_string(str,"ids.default",0);
    }else if (item_exist("ids.loc.dev0.interface")){
        item_string(str,"ids.loc.dev0.interface",0);
    }
    if (strcmp(str,"lcd") == 0){
        disp_type = DISP_TYPE_LCD;
    }
    else if (strcmp(str,"dsi") == 0){
        disp_type = DISP_TYPE_DSI;
    }
	
	if(item_exist("config.uboot.logo")){                                               
		unsigned char str[ITEM_MAX_LEN] = {0};                                  
	    item_string(str,"config.uboot.logo",0);                                        
		if(strncmp(str,"0",1) == 0){ 
			return;
		}
	}
	
    if (disp_type != DISP_TYPE_LCD && disp_type != DISP_TYPE_DSI){
		disp_type = DISP_TYPE_LCD; 
    }
    // to power on the ids module 
    if(displaylogo != 1){
        module_power_on(0);
    }

    // to get all the necessary parameters
    memset((void *)&cfg, 0, sizeof (struct lcdc_config_t));
    params = getenv("idsLcdConfig");
    
    // lcdstr = (struct lcdc_config_t *)getenv("idsLcdConfig");
    if (params){
        printf(" idsLcdConfig found , lcd configurations already save !!!!!!!!!1\n");
        //  memcpy((void *)&cfg, (void *)lcdstr, sizeof(struct lcdc_config_t));

#if 1
        // struct:: lcdc_config_t
        str1 = getenv ("width");
        cfg.width = simple_strtoul(str1,NULL,10);
        str1 = getenv ("height");
        cfg.height = simple_strtoul(str1,NULL,10);
        str1 = getenv("vspw");
        cfg.vspw = simple_strtoul(str1,NULL,10);
        str1 = getenv("vbpd");
        cfg.vbpd = simple_strtoul(str1,NULL,10);
        str1 = getenv("vfpd");
        cfg.vfpd = simple_strtoul(str1,NULL,10);
        str1 = getenv ("hspw");
        cfg.hspw = simple_strtoul(str1,NULL,10);
        str1 = getenv("hbpd");
        cfg.hbpd = simple_strtoul(str1,NULL,10);
        str1 = getenv("hfpd");
        cfg.hfpd = simple_strtoul(str1,NULL,10);
        str1 = getenv("signalPwrenEnable");
        cfg.signalPwrenEnable = simple_strtoul(str1,NULL,10);
        str1 = getenv("signalPwrenInverse");
        cfg.signalPwrenInverse = simple_strtoul(str1,NULL,10);
        str1 = getenv ("signalVclkInverse");
        cfg.signalVclkInverse = simple_strtoul(str1,NULL,10);
        str1 = getenv("signalHsyncInverse");
        cfg.signalHsyncInverse = simple_strtoul(str1,NULL,10);
        str1 = getenv("signalVsyncInverse");
        cfg.signalVsyncInverse = simple_strtoul(str1,NULL,10);
        str1 = getenv("signalDatsInverse");
        cfg.signalDataInverse = simple_strtoul(str1,NULL,10);
        str1 = getenv("signalVdenInverse");
        cfg.signalVdenInverse = simple_strtoul(str1,NULL,10);
        str1 = getenv ("SignalDataMapping");
        cfg.SignalDataMapping = simple_strtoul(str1,NULL,10);
        str1 = getenv ("fpsx1000");
        cfg.fpsx1000 = simple_strtoul(str1,NULL,10);
        str1 = getenv ("phyWidth");
        cfg.phyWidth = simple_strtoul(str1,NULL,10);
        str1 = getenv ("phyHeight");
        cfg.phyHeight = simple_strtoul(str1,NULL,10);

        IM_DEBUG(" -- fpsx1000 = %d - \n",cfg.fpsx1000);
        IM_DEBUG(" -- phyWidth = %d - \n",cfg.phyWidth);
        IM_DEBUG(" -- phyHeight = %d - \n",cfg.phyHeight);
#endif
    }
    else{
        IM_DEBUG(" idsLcdConfig no found , use default lcd configurations !!!!!!!!!1\n");
        if(item_exist("ids.fb.width")){
            cfg.width = item_integer("ids.fb.width", 0);
            cfg.height = item_integer("ids.fb.height", 0);
        }else {
 			/*
			*(1)Author:summer
			*(2)Date:2014-6-26
			*(3)Operation:modify
			*(4)Reason:We support uboot-logo,and add two entries in item.But user doesn't want to modify item.
			*  So,we change the code and won't change item.
			*/
            if(item_exist("dss.implementation.lcdpanel.name")){
                 item_string(str,"dss.implementation.lcdpanel.name", 0);
                 IM_DEBUG("str =%s\n",str);
			     char *p =(char*)str;                                             
                 p = strstr(p, "_");                                              
                 p++;                                                             
                 cfg.width = skip_atoi(&p);                                       
                 p = strstr(p, "_");                                              
                 p++;                                                             
                 cfg.height = skip_atoi(&p);                                       
                                                                                    
                 IM_DEBUG("cfg.width = %d\n",cfg.width);                          
                 IM_DEBUG("cfg.height = %d\n",cfg.height);
            }
      }
	  /* Author:summer
	   * Date:2014-5-28
	   * Reason:
	   *	(1)Get screen type from item 
	   *	(2)According to the type,choose approciate config
	   * Tag1:
	   * ++++++++++++++++++++++
	   * +Date:2014-7-21
	   * +Reason:
	   * +(1)IF items.itm has no entry named "dss.xxxxxx",that is to say,current system has no lcd.
	   * +   So we have no need to light the screen,because this operation cost a plenty of time.
	   * +(2)So we set the origional value of screenDeviceType to -1,meaning without any  lcd.
	   * +Seen:tag2
	   * */
	   //Tag1:summer modification starts here
	   screenDeviceType = 0; 
	   if(item_exist("dss.implementation.lcdpanel.name")){
		   memset(str,0,sizeof(str));
		   item_string(str,"dss.implementation.lcdpanel.name",0);
		   if(strcmp("KR070LB0S_1024_600",str) == 0){
				screenDeviceType = 0;
		   }
		   if(strcmp("KR070PB2S_800_480",str) == 0){
				screenDeviceType = 1;
		   }
		   if(strcmp("KR079LA1S_768_1024",str) == 0){
				screenDeviceType = 2;
		   }
                   if(strcmp("KR079LA1S_720_1280",str) == 0){
				screenDeviceType = 3;
		   }
		   if(strcmp("GM05004001Q_800_480",str) == 0){
				screenDeviceType = 4;
		   }

	   }else
		   screenDeviceType = -1;//tag2

	  /* Author:summer
	   * Date:2014-6-30
	   * Reason:
	   *(1)If box type,we will give different params
	   * */
	   	   
	   if(item_exist("dss.implementation.product.type")){
		   memset(str,0,sizeof(str));
		   item_string(str,"dss.implementation.product.type",0);
		   if(strcmp("box",str) == 0){
				screenDeviceType = 3;
		   }
	   }
	   //Tag1:summer modification ends here
        cfg.hspw = 20;
        cfg.hbpd = 160;
        cfg.hfpd = 160;
        cfg.vspw = 8;
        cfg.vfpd = 12;
        cfg.vbpd = 15;
        cfg.SignalDataMapping = 0x6;
        cfg.signalVclkInverse = 0;
        cfg.signalHsyncInverse = 1;
        cfg.signalVsyncInverse = 1;
        cfg.signalDataInverse = 0;
        cfg.signalVdenInverse = 0;
        cfg.signalPwrenInverse = 0;
        cfg.signalPwrenEnable = 1;
        cfg.fpsx1000 = 60000;

	   //Tag1:summer modification starts here
		if(screenDeviceType >= 0 && screenDeviceType < sizeof(screen_params)/sizeof(struct lcd_panel_param ) )
		{
				//width,height
				cfg.width = screen_params[screenDeviceType].dtd.mHActive; 
				cfg.height = screen_params[screenDeviceType].dtd.mVActive; 
				
				//vertical 
				cfg.vspw = screen_params[screenDeviceType].dtd.mVSyncPulseWidth; 
				cfg.vbpd = screen_params[screenDeviceType].dtd.mVBlanking - 
								screen_params[screenDeviceType].dtd.mVSyncOffset -
									screen_params[screenDeviceType].dtd.mVSyncPulseWidth; 
				cfg.vfpd = screen_params[screenDeviceType].dtd.mVSyncOffset; 
				
				//horizontal
				cfg.hspw = screen_params[screenDeviceType].dtd.mHSyncPulseWidth; 
				cfg.hbpd = screen_params[screenDeviceType].dtd.mHBlanking -
								screen_params[screenDeviceType].dtd.mHSyncOffset -
									screen_params[screenDeviceType].dtd.mHSyncPulseWidth; 
				cfg.hfpd = screen_params[screenDeviceType].dtd.mHSyncOffset; 

				//Inverse
				cfg.signalVclkInverse = screen_params[screenDeviceType].dtd.mVclkInverse; 
				cfg.signalHsyncInverse = screen_params[screenDeviceType].dtd.mHSyncPolarity; 
				cfg.signalVsyncInverse = screen_params[screenDeviceType].dtd.mVSyncPolarity; 
		}

	   //Tag1:summer modification ends here


    }

    printf(" %s screenDeviceType:%d, burn_status:%d, timest:%lu\n", __func__, screenDeviceType, burn_status, get_timer(0));
  /* Author:summer                                                        
  * Operation:modify                                                     
  * Date:2014-6-18                                                       
  * Reason:                                                              
  * (1)If there exists logo.isi in nand flash,we will display the logo   
  * (2)If load logo.isi from nand failed,we dill display default logo.   
  * (3)If no logo.isi in nand,we will display default logo.              
  * */              
  printf(" %s no for begin, timest:%lu\n", __func__, get_timer(0));

	       /*                                                                      
          * Author:summer                                                        
          * Operation:add                                                        
          * Date:2014-6-18                                                       
          * Reason:                                                              
          * (1)Check the data of addr,if all 0x0,we use default logo             
          * Position:starts here                                                 
          * */                                                                   


	/* 
	 *(1)Author:summer
	 *(2)Date:2014-7-21 
	 *(3)Reason:
	 *(3.1)IF it's box,its items.itm has no member named "dss.xxxx.name",and it's screenDeviceType is -1,
	 *     we will not execute the codes below,bacause it cost plenty of time.
	 *(3.2)If it's box,its items.itm has member named "dss.xxx.product.type box",and it's screenDeviceType is 3
	 *     we will not execute the codes below,bacause it cost plenty of time.
	 * */
	 if( -1 == screenDeviceType || 3 == screenDeviceType)
	 {
        rbsetint("ubootlogo", 1);
		return ;
	 }

    /*	1061
	 *(1)Author:summer	1062
	 *(2)Date:2014-7-8
	 *(3)Reason:There exist black lines in default logo(four penguins)
	 * */	
	dcache_flush();
	dcache_disable();
    lcd_configure(&cfg, (int *)addr);
	dcache_enable();
    // default power on sequence :
    // 0.  lcd power (dsi power)
    // 1.  rgb data supply
    // 2.  lvds power 
    // 3.  back light
    // If any other power sequence needed (eg. spi), add it later
    // STEP 0:
    if(item_exist("lcd.power.v33") || item_exist("ids.loc.dev0.lcd.power")){
        if(item_exist("lcd.power.v33")){
            item_string(str,"lcd.power.v33", 0);
            if (strncmp(str, "pmu", 3) == 0){
                item_string(str,"lcd.power.v33",1);
                pmu_set((void *)str);
            }
            else if (!strncmp(str, "pads", 4)){
                padNum = item_integer("lcd.power.v33",1);
                IM_DEBUG(" padNum = %d - \n",padNum);
                //gpio_reset();
                gpio_set(padNum);
            }

        }else if (item_exist("ids.loc.dev0.lcd.power")){
            item_string(str, "ids.loc.dev0.lcd.power",0);

            if (strncmp(str, "pmu", 3) == 0){
                item_string(str,"ids.loc.dev0.lcd.power",1);
                pmu_set((void *)str);
            }
            else if (!strncmp(str, "pads", 4)){
                padNum = item_integer("ids.loc.dev0.lcd.power",1);
                IM_DEBUG(" padNum = %d - \n",padNum);
                //gpio_reset();
                gpio_set(padNum);
            }
        }
    }else { 
        if(item_exist("dsi.power.v33")){
            item_string(str,"dsi.power.v33", 0);
        }else if (item_exist("ids.loc.dev0.dsi.power")){
            item_string(str, "ids.loc.dev0.dsi.power",0);
        }
        if (strncmp(str, "pmu", 3) == 0){
            item_string(str,"ids.loc.dev0.dsi.power",1);
            pmu_set((void *)str);
        }
        else if (!strncmp(str, "pads", 4)){
            padNum = item_integer("ids.loc.dev0.dsi.power",1);
            gpio_set(padNum);
        }
    }
	
	//LCD Enable
	if(item_exist("board.cpu"))
	{
		item_string(str,"board.cpu",0);
		{
			if(strcmp(str,"x9") == 0){
				gpio_set(28);
			}
			if(strcmp(str,"x15") == 0){
				gpio_set(94);
			}
		}
	}
    // STEP 1:

    udelay(10*1000);
    if (disp_type == DISP_TYPE_DSI){
        dsi_configure(&cfg);
    }
    writel(readl(LCDCON1)|0x01,LCDCON1); // enable

    if((item_exist("board.cpu") == 0) || item_equal("board.cpu", "imapx820", 0)) {
        for(i=82; i <= 109; i ++){
            gpio_mode_set(0, i);
        }
    }else if(item_equal("board.cpu", "x15", 0) || item_equal("board.cpu", "i15", 0)) {    
        for(i=66; i <= 93; i ++){
            gpio_mode_set(0, i);
        }
	}else if(item_equal("board.cpu", "x9", 0)) {
			IM_DEBUG("board cpu :x9 gpio set function\n");
        for(i=84; i <= 111; i ++){
            gpio_mode_set(0, i);
        }
        // pull enable ?
    }
    // STEP 2:
    if(item_exist("lcd.power.lvds") || item_exist("ids.loc.dev0.lvds.power")){
        if(item_exist("lcd.power.lvds")){       
            item_string(str,"lcd.power.lvds", 0); 
        }else if (item_exist("ids.loc.dev0.lvds.power")){
            item_string(str, "ids.loc.dev0.lvds.power",0);
        }
        udelay(10*1000);
        if (strncmp(str, "pmu", 3) == 0){
            item_string(str,"ids.loc.dev0.lvds.power",1);
            pmu_set((void *)str);
        }
        else if (strncmp(str, "pads", 4)){
            padNum = item_integer("ids.loc.dev0.lvds.power",1);
            gpio_set(padNum);
        }
    }

        udelay(200*1000);
    // STEP 3: pwm default as 0 for backlight
    // Currently just pull the gpio, later add pwm implement,
    // it may cause backlight dim during the kernel run
      
    // pwm adjust 
        pwm_init(item_integer("bl.ctrl",1));
        if (item_exist("bl.start"))
        {   
            bl_freq = item_integer("bl.start",0);
        }   
        if (item_exist("bl.start"))
        {      
            intensity = item_integer("bl.def_intensity",0); 
        }
        //printf("bl_freq=%d,intensity=%d\n",bl_freq ,intensity);
        pwm_set(bl_freq, intensity);
        //

        if (item_exist("bl.power")){
            if(item_exist("codec.power")){
                item_string(str, "codec.power",0);
                if (strncmp(str, "pmu", 3) == 0){ 
                    item_string(str,"codec.power",1);
                    pmu_set((void *)str);
                }   
            }   
            // }   
            item_string(str, "bl.power",0);
            if (strncmp(str, "pads", 4) == 0){ 
                bl_power_index = item_integer("bl.power",1);
                gpio_set(bl_power_index);
            }   
        }


	/* 
	 *(1)Author:summer
	 *(2)Date:2014-8-5
	 *(3)Reason:
	 *(3.1)For bl(backlight),when we call gpio_set(109) to pull it to high,it only increase 1v,in fact it should be 3.3v
	 *(3.2)According to liu.hai,we should add the codes below .
	 *(4)Ok.
	 * */
      if(item_exist("codec.power")){
            item_string(str, "codec.power",0);
            if (strncmp(str, "pmu", 3) == 0){ 
                item_string(str,"codec.power",1);
                pmu_set((void *)str);
            }   
      }   

	  if(item_exist("board.cpu"))                                                 
      {                                                                           
        item_string(str,"board.cpu",0);                                         
      	{                                                                          
              if(strcmp(str,"x9") == 0){                                             
                  gpio_set(27);                                                      
				  IM_DEBUG("bl power\n");
         	  }                                                                      
              if(strcmp(str,"x15") == 0){                                             
                  gpio_set(109);                                                      
				  IM_DEBUG("bl power\n");
         	  }                                                                      
         }                                                                          
      }          

        //udelay(250*1000);
      /*  if((item_exist("board.cpu") == 0) || item_equal("board.cpu", "imapx820", 0)) {
            i = item_integer("bl.ctrl",1);
            switch (i){
                case 0:
                    j = 72;
                    break;
                case 1:
                    j = 71;
                    break;
                case 2:
                    j = 70;
                    break;
                default:
                    printf(" --  backlight pwm num not supported \n"); 
                    return ;
            }
            if (item_exist("bl.power")){
                gpio_set(-j);
            }else{
                gpio_set(j);
            }
        }else if(item_equal("board.cpu", "x15", 0) || item_equal("board.cpu", "i15", 0)) {    
            i = item_integer("bl.ctrl",1);
            switch (i){
                case 0:
                    j = 59;
                    break;
                case 1:
                    j = 58;
                    break;
                case 2:
                    j = 57;
                    break;
                default:
                    printf(" --  backlight pwm num not supported \n"); 
                    return ;
            }
            if (item_exist("bl.power")){
                gpio_set(j);
            }else{
                gpio_set(j);
            }
            //gpio_pull_en(1,j);
        }*/
        IM_DEBUG(" 0x22001094 = 0x%x - \n",readl(0x22001094));
        IM_DEBUG(" ############# fuck display_logo end !!!!!!!!!!!!!!\n");
        if(displaylogo != 1){
            displaylogo ++;
        }
        rbsetint("ubootlogo", 1);
		//dcache_enable();
        return ;
}

