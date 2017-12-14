/***************************************************************************** 
 ** 
 ** Copyright (c) 2014~2112 ShangHai Infotm Ltd all rights reserved. 
 ** 
 ** Use of Infotm's code is governed by terms and conditions 
 ** stated in the accompanying licensing statement. 
 ** 
 *****************************************************************************/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/clk.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/camera/csi/csi_reg.h>
#include <linux/regulator/consumer.h>
#include <linux/regulator/of_regulator.h>
#include <mach/pad.h>
#include <mach/items.h>
#include <mach/power-gate.h>
#include <mach/imap-iomap.h>

#include "stk3855.h"

#define STK3855_RESET_PIN    (44)
#define STK3855_ADAPTER_ID   (3)

#define STK3855_I2C_NAME "stk3855"

#define STK3855_DEBUG_IF

static const struct i2c_device_id stk3855_id[] = {
    {STK3855_I2C_NAME, 0},
    {},
};

static struct i2c_board_info stk3855_info = {
    I2C_BOARD_INFO(STK3855_I2C_NAME, 0x3f),
   // I2C_BOARD_INFO(STK3855_I2C_NAME, 0x10),
};

static struct i2c_adapter *stk3855_adapter = NULL;
static struct i2c_client  *stk3855_client  = NULL;

static struct sensor_drv_context stk3855_context = {STK3855_RESET_PIN};

MODULE_DEVICE_TABLE(i2c, stk3855_id);

static int stk3855_i2c_write(struct i2c_client *client, int datalen, u8 addr, u8 data)
{
    int ret = 0;
    u8 tmp_buf[128];
    unsigned int bytelen = 0;

    tmp_buf[0] = addr;

    bytelen+=1;

    if (datalen != 0/* && data != 0*/)
    {
        tmp_buf[1] = data;
        bytelen += datalen;
    }

    ret = i2c_master_send(client, tmp_buf, bytelen);

    return ret;
}

static int stk3855_i2c_read(struct i2c_client *client, int datalen, u8 addr, u8 *pdata)
{
    int ret = 0;

    ret = stk3855_i2c_write(client, 0, addr, 0);
    if (ret < 0)
    {
        printk("stk3855 %s set data address fail!\n", __func__);
        return ret;
    }

    ret =  i2c_master_recv(client, pdata, datalen);
    if (ret < 0)
    {
        printk("stk3855 %s set data address fail!\n", __func__);
        return ret; 
    }

    return ret;
}

//2015------------------------------

unsigned short usTG_WD_Width;
unsigned short usTG_HI_Height;

void fnCsbdSetTG(struct i2c_client *client);
void fnCsbdEnableSbu(struct i2c_client *client);

void fnCsbdResetSbu(struct i2c_client *client)
{
    unsigned char ucValue;

    printk("=>fnCsbdResetSbu\n");
    stk3855_i2c_write(client,1,0x00,0x02);	//reset SBU
    stk3855_i2c_write(client,1,0x06,0x00);	//reset SBU
    printk("<=fnCsbdResetSbu\n");
}

void fnCsbdRestartTG(struct i2c_client *client)
{	
    printk("=>fnCsbdRestartTG\n");
    fnCsbdResetSbu(client);
    fnCsbdSetTG(client);
    fnCsbdEnableSbu(client);
    printk("<=fnCsbdRestartTG\n");
}

void fnCsbdEnableSbu(struct i2c_client *client)
{
    printk("=>fnCsbdEnableSbu\n");
    stk3855_i2c_write(client,1,0x00,0x01);
    printk("<=fnCsbdEnableSbu\n");
}

void fnCsbdDisableSbu(struct i2c_client *client)
{
    printk("=>fnCsbdDisableSbu\n");
    stk3855_i2c_write(client,1,0x00,0x00);
    printk("<=fnCsbdDisableSbu\n");
}

/*---------------for CSI_bridge PIO setting---------------------*/
/*ucPIO_HCO: true = set clock inversion from host , false= don't invert*/ /*invert clock, reg A4H[2]*/
/*C1I: true = invert internal phase clock of SBU to latch sensor 0 , false= don't invert*/
/*C0I: true = invert internal phase clock of SBU to latch sensor 1 , false= don't invert*/

void fnCsbdInvClk(struct i2c_client *client, bool ucPIO_HCO, bool C1I, bool C0I)
{
    unsigned char ucValue;

    printk("=>fnCsbdInvClk\n");
    /*set clock inversion register A4H*/
    ucValue = C0I | (C1I << 1) | (ucPIO_HCO << 2);
    stk3855_i2c_write(client,1,0xa4,ucValue);
    printk("<=fnCsbdInvClk\n");
}

/*fnCsbdSetClkClkmSel: select SBU clock source: 
0: sensor 0, 1:sensor 1, 2:host clock 3:divided PLL clock*/
void fnCsbdSetClkClkmSrc(struct i2c_client *client, unsigned char ucClkmSrc)
{
    unsigned char ucValue;

    printk("=>fnCsbdSetClkClkmSrc\n");
    stk3855_i2c_read(client,1,0xa3,&ucValue);
    ucValue = (ucValue & 0xE7) | (ucClkmSrc << 3);
    stk3855_i2c_write(client,1,0xa3,ucValue);
    printk("<=fnCsbdSetClkClkmSrc\n");
}

/*set PLL power down : 1: power down PLL, 0: PLL normal operation*/
void fnCsbdSetPllPwrDown(struct i2c_client *client, int PllPowerDown)
{
    unsigned char ucValue;

    printk("=>fnCsbdSetPllPwrDown\n");
    stk3855_i2c_read(client,1,0xa2,&ucValue);
    ucValue = (ucValue & 0xFE) | (PllPowerDown);
    stk3855_i2c_write(client,1,0xa2,ucValue);
    printk("<=fnCsbdSetPllPwrDown\n");
}

void fnCsbdSetPllSrc(struct i2c_client *client, unsigned char ucPllSrc)
{
	unsigned char ucValue;

	printk("=>%s\n", __func__);
	stk3855_i2c_read(client,1,0xa2,&ucValue);
	ucValue = (ucValue & 0x3F) | (ucPllSrc << 6);
	stk3855_i2c_write(client,1,0xa2,ucValue);
	printk("<=%s\n", __func__);
}

/*select clock (SBU outputclock =  sensor 0 pixel clock * 2) for side by side mode */
void fnCsbdSetClkSide(struct i2c_client *client)
{	
    printk("=>fnCsbdSetClkSide\n");
    //power up pll
    fnCsbdSetPllPwrDown(client,0);	
    //select clock source as 3:divided PLL clockl
    fnCsbdSetClkClkmSrc(client,3);
	fnCsbdSetPllSrc(client, 0);
    printk("<=fnCsbdSetClkSide\n");
}

void fnCsbdSetClkNonSideBySide(struct i2c_client *client, unsigned char ucSnrSel)
{
    printk("=>fnCsbdSetClkNonSideBySide\n");
    //power down pll
    fnCsbdSetClkClkmSrc(client,ucSnrSel);
    fnCsbdSetPllPwrDown(client,1);
    printk("<=fnCsbdSetClkNonSideBySide\n");
}

/*disable  sensor serial communications from host, select SENSOR0 or SENSOR1*/
void fnCsbdSetSnrDisable(struct i2c_client *client, unsigned char ucSnrSel)
{
    unsigned char ucValue;

    printk("=>fnCsbdSetSnrDisable\n");
    stk3855_i2c_read(client,1,0xb0,&ucValue);
    ucValue = ucValue | (1 << ucSnrSel); 
    stk3855_i2c_write(client,1,0xb0,ucValue);
    printk("<=fnCsbdSetSnrDisable\n");
}

/*Enable sensor serial communications from host, select SENSOR0 or SENSOR1*/
void fnCsbdSetSnrEnable(struct i2c_client *client, unsigned char ucSnrSel)
{
    unsigned char ucValue;

    printk("=>fnCsbdSetSnrEnable\n");
    stk3855_i2c_read(client,1,0xB0,&ucValue);
    ucValue = ucValue & (0 << ucSnrSel);
    stk3855_i2c_write(client,1,0xB0,ucValue);
    printk("<=fnCsbdSetSnrEnable\n");
}

/*
   ucSensortype can be set as:
   SENSORTYPE_HN_VN	SENSORTYPE_HN_VP	SENSORTYPE_HP_VN	SENSORTYPE_HP_VP	

   ucSnrSourceSel can be set as:
   SNRSRC_SNR0BUF0		SNRSRC_SNR0BUF1		SNRSRC_AUTO			
   */
void fnCsbdSetSnrType(struct i2c_client *client, unsigned char ucSensortype, unsigned char ucSnrSourceSel)
{
    printk("=>fnCsbdSetSnrType\n");
    /*set sensor type*/
    stk3855_i2c_write(client,1,0x04,(ucSensortype | (ucSnrSourceSel << 4)));
    printk("<=fnCsbdSetSnrType\n");
}

/*ucModeCIGBypass set bypass sensor as: SENSOR0 or SENSOR1  reg04h[15:12]*/
/*ucMatrixTypeMT set as: MT_BAYLOR_INPUT 	MT_YUV_INPUT */		
void fnCsbdSetTM(struct i2c_client *client, unsigned char ucBypassSensor)
{
    printk("=>fnCsbdSetTM\n");	
    stk3855_i2c_write(client,1,0xa8,(0x7e << 1)|ucBypassSensor);
    printk("<=fnCsbdSetTM\n");
}

/*ucModeCIGSidebySide: left side of each line is SENSOR0 or SENSOR1*/
/*ucMatrixTypeMT set as: MT_BAYLOR_INPUT 	MT_YUV_INPUT */	
void fnCsbdSetSbuMode(struct i2c_client *client, unsigned char ucSbuMode, unsigned char ucModeCIGSidebySide)
{
    unsigned char ucValue;

    printk("=>fnCsbdSetSbuMode\n");
    ucValue = ucSbuMode | (ucModeCIGSidebySide << 4);
    stk3855_i2c_write(client,1,0x05,ucValue);
    printk("<=fnCsbdSetSbuMode\n");
}

/*must set after fnCsbdSetSbuMdxxxxxx is set*/
/*must set fnCsbdSetTGSidebySide in side by side mode! , fnCsbdSetTG in other modes!*/

void fnCsbdSetTG(struct i2c_client *client) // unsigned short usSnr0caphstart,unsigned short usSnr0capvstart)
{
    unsigned short ustmp;
    unsigned char ucValue;

    printk("=>fnCsbdSetTG\n");
    stk3855_i2c_write(client,1,0x06,0x11);	

    while(1){
        stk3855_i2c_read(client,1,0x02,&ucValue);
        if(ucValue & 0x1) break; /*detection success, end while*/
    }

    usTG_WD_Width = 0;
    usTG_HI_Height = 0;

    /*read out WD*/
    stk3855_i2c_read(client,1,0x09,&ucValue);
    printk("WD_Width MSB  = 0x%x\n", ucValue);
    usTG_WD_Width = (ucValue << 8);
    //SETWORDMSB(usTG_WD_Width,ucValue); /*store WB MSB*/

    stk3855_i2c_read(client,1,0x08,&ucValue);
    printk("WD_Width LSB  = 0x%x\n", ucValue);
    usTG_WD_Width = usTG_WD_Width + ucValue;
    //SETWORDLSB(usTG_WD_Width,ucValue); /*store WD LSB*/

    /*read out HI*/
    stk3855_i2c_read(client,1,0x0b,&ucValue);
    printk("HI_Height MSB = 0x%x\n", ucValue);
    usTG_HI_Height = (ucValue << 8);
    //SETWORDMSB(usTG_HI_Height,ucValue); /*store HI MSB*/

    stk3855_i2c_read(client,1,0x0a,&ucValue);
    printk("HI_Height LSB = 0x%x\n", ucValue);
    usTG_HI_Height = usTG_HI_Height + ucValue;
    //SETWORDLSB(usTG_HI_Height,ucValue); /*store HI LSB*/

    printk("WD_Width  = 0x%x\n", usTG_WD_Width);
    printk("HI_Height = 0x%x\n", usTG_HI_Height);

    /*TG HSYNC hstart*/
    stk3855_i2c_write(client,1,0x0c,0x00);
    stk3855_i2c_write(client,1,0x0d,0x00);

    /* TG output hend*/
    stk3855_i2c_write(client,1,0x0e,0xC3);
    stk3855_i2c_write(client,1,0x0f,0x06); //99142
//	stk3855_i2c_write(client,1,0x0e,0xd7);
//    stk3855_i2c_write(client,1,0x0f,0x05);

    /* TG VSYNC vstart*/
    stk3855_i2c_write(client,1,0x10,0x00);
    stk3855_i2c_write(client,1,0x11,0x00);

    /* TG VSYNC vend*/
    stk3855_i2c_write(client,1,0x12,0x23);
    stk3855_i2c_write(client,1,0x13,0x00);

    /*set iosync_thr to be at least half size of capture window in side by side mode*/
    ustmp = usTG_WD_Width/4;
    //ustmp = 1280;
    /*set iosync_thr LSB*/
    ucValue = GETWORDLSB(ustmp);
    stk3855_i2c_write(client,1,0x30,ucValue);

    /*set iosync_thr MSB*/
    ucValue = GETWORDMSB(ustmp);
    stk3855_i2c_write(client,1,0x31,ucValue);	

    /*set iosync_sel , select buffer0 smaller*/
    stk3855_i2c_write(client,1,0x32,0x00);
    printk("<=fnCsbdSetTG\n");
}

/*CSI_bridge set sensor 0 capture range*/

void fnCsbdSetSnr0Cap(struct i2c_client *client, STCsiEngineInfo* pstCsiEngineInfo, unsigned char ucSbuMode)
{ 
    unsigned short usSen0hstart, usSen0hend, usSen0vstart, usSen0vend;

    printk("=>fnCsbdSetSnr0Cap\n");

    if (ucSbuMode == SBU_MODE_SIDEBYSIDE)
    {
        usSen0hstart = pstCsiEngineInfo->usSrcStartX + 626+120;
        usSen0hend = pstCsiEngineInfo->usSrcStartX + 626 +(pstCsiEngineInfo->usSrcWidth/2)-1-120; //99142
  //      usSen0hstart = pstCsiEngineInfo->usSrcStartX;
    //    usSen0hend = pstCsiEngineInfo->usSrcStartX +(pstCsiEngineInfo->usSrcWidth/2)-1;
        usSen0vstart = pstCsiEngineInfo->usSrcStartY;
        usSen0vend = pstCsiEngineInfo->usSrcEndY;
    }
    else
    {
        usSen0hstart = 160; //pstCsiEngineInfo->usSrcStartX;
        usSen0hend = pstCsiEngineInfo->usSrcEndX;
        usSen0vstart = pstCsiEngineInfo->usSrcStartY;
        usSen0vend = pstCsiEngineInfo->usSrcEndY;
    }


    /* sensor 0 cap_hstart */
    stk3855_i2c_write(client,1,0x14,GETWORDLSB(usSen0hstart));
    stk3855_i2c_write(client,1,0x15,GETWORDMSB(usSen0hstart));

    /* sensor 0 cap_hend */
    stk3855_i2c_write(client,1,0x16,GETWORDLSB(usSen0hend));
    stk3855_i2c_write(client,1,0x17,GETWORDMSB(usSen0hend));

    /* sensor 0 cap_vstart*/
    stk3855_i2c_write(client,1,0x18,GETWORDLSB(usSen0vstart));
    stk3855_i2c_write(client,1,0x19,GETWORDMSB(usSen0vstart));

    /* sensor 0 cap_vend */
    stk3855_i2c_write(client,1,0x1a,GETWORDLSB(usSen0vend));
    stk3855_i2c_write(client,1,0x1b,GETWORDMSB(usSen0vend));
    printk("<=fnCsbdSetSnr0Cap\n");
}

/*CSI_bridge set sensor 1 capture range*/

void fnCsbdSetSnr1Cap(struct i2c_client *client, STCsiEngineInfo* pstCsiEngineInfo, unsigned char ucSbuMode)
{
    unsigned short usSen1hstart, usSen1hend, usSen1vstart, usSen1vend;

    if(ucSbuMode == SBU_MODE_SIDEBYSIDE)
    {
        usSen1hstart = pstCsiEngineInfo->usSrcStartX + 626+120;
       usSen1hend = pstCsiEngineInfo->usSrcStartX + 626 +(pstCsiEngineInfo->usSrcWidth/2)-1-120; //99141
  //      usSen1hstart = pstCsiEngineInfo->usSrcStartX;
    //    usSen1hend = pstCsiEngineInfo->usSrcStartX +(pstCsiEngineInfo->usSrcWidth/2)-1;
        usSen1vstart = pstCsiEngineInfo->usSrcStartY;
        usSen1vend = pstCsiEngineInfo->usSrcEndY;
    }
    else
    {
        usSen1hstart = 160; //pstCsiEngineInfo->usSrcStartX;
        usSen1hend = pstCsiEngineInfo->usSrcEndX;
        usSen1vstart = pstCsiEngineInfo->usSrcStartY;
        usSen1vend = pstCsiEngineInfo->usSrcEndY;
    }

    printk("=>fnCsbdSetSnr1Cap\n");
    /* sensor 1 cap_hstart */
    stk3855_i2c_write(client,1,0x1c,GETWORDLSB(usSen1hstart));
    stk3855_i2c_write(client,1,0x1d,GETWORDMSB(usSen1hstart));

    /* sensor 1 cap_hend */
    stk3855_i2c_write(client,1,0x1e,GETWORDLSB(usSen1hend));
    stk3855_i2c_write(client,1,0x1f,GETWORDMSB(usSen1hend));

    /* sensor 1 cap_vstart*/
    stk3855_i2c_write(client,1,0x20,GETWORDLSB(usSen1vstart));
    stk3855_i2c_write(client,1,0x21,GETWORDMSB(usSen1vstart));

    /* sensor 1 cap_vend */
    stk3855_i2c_write(client,1,0x22,GETWORDLSB(usSen1vend));
    stk3855_i2c_write(client,1,0x23,GETWORDMSB(usSen1vend));
    printk("<=fnCsbdSetSnr1Cap\n");
}

/*if side by side mode, don't need parameter  usOuthend, 
  but need usSnr0caphend , usSnr0caphstart  , usSnr1caphend , usSnr1caphstart */

void fnCsbdSetOutWindow(struct i2c_client *client, STCsiEngineInfo* pstCsiEngineInfo)
{
    unsigned short ustmp;
    unsigned short usOuthstart, usOuthend, usOutvstart, usOutvend;

    printk("=>fnCsbdSetOutWindow\n");

    usOuthstart = pstCsiEngineInfo->usSrcStartX;
    usOuthend = pstCsiEngineInfo->usSrcEndX-480;

    usOutvstart = pstCsiEngineInfo->usSrcStartY;
    usOutvend = pstCsiEngineInfo->usSrcEndY + 0;

    /* out_h start */
    stk3855_i2c_write(client,1,0x24,GETWORDLSB(usOuthstart));
    stk3855_i2c_write(client,1,0x25,GETWORDMSB(usOuthstart));

    /* out_h end  */
    stk3855_i2c_write(client,1,0x26,GETWORDLSB(usOuthend));
    stk3855_i2c_write(client,1,0x27,GETWORDMSB(usOuthend));

    /* out_v start*/
    stk3855_i2c_write(client,1,0x28,GETWORDLSB(usOutvstart));
    stk3855_i2c_write(client,1,0x29,GETWORDMSB(usOutvstart));

    /* out_v end */
    stk3855_i2c_write(client,1,0x2a,GETWORDLSB(usOutvend));
    stk3855_i2c_write(client,1,0x2b,GETWORDMSB(usOutvend));

    /*set the output point -- one pixel left (before) of the first captured point of sensor 0 */
    /*if h start = 0 , set iosync_h = WD , set iosync_v = snr0capvstart -1*/
    if(usOuthstart == 0)
    {
        /*set iosync_h LSB*/
        stk3855_i2c_write(client,1,0x2c,GETWORDLSB(usTG_WD_Width));
        /*set iosync_h MSB*/
        stk3855_i2c_write(client,1,0x2d,GETWORDMSB(usTG_WD_Width));

        if(usOutvstart == 0)
        {
            /*set iosync_v LSB*/	
            stk3855_i2c_write(client,1,0x2e,0x00);
            /*set iosync_v MSB*/
            stk3855_i2c_write(client,1,0x2f,0x00);
        }
        else
        {
            /* output  from (0, h) , iosync from (WD, h-1)*/
            /*set iosync_v LSB*/	
            ustmp = usOutvstart - 1;
            stk3855_i2c_write(client,1,0x2e,GETWORDLSB(ustmp));
            /*set iosync_v MSB*/
            stk3855_i2c_write(client,1,0x2f,GETWORDMSB(ustmp));
        }
    }
    else
    {
        /*if if h start != 0 , set iosync_h = (h-1, v)*/
        /*set iosync_h LSB*/
        ustmp = usOuthstart-1;
        stk3855_i2c_write(client,1,0x2c,GETWORDLSB(ustmp));	
        /*set iosync_h MSB*/
        stk3855_i2c_write(client,1,0x2d,GETWORDMSB(ustmp));	

        /*set iosync_v LSB*/	
        stk3855_i2c_write(client,1,0x2e,GETWORDLSB(usOutvstart));
        /*set iosync_v MSB*/
        stk3855_i2c_write(client,1,0x2f,GETWORDMSB(usOutvstart));
    }
    printk("<=fnCsbdSetOutWindow\n");
}

void fnCsbdStartSideBySideMode(struct i2c_client *client, STCsiEngineInfo* pstCsiEngineInfo, unsigned char ucMasterSensor)
{
    unsigned char ucSensorType = 0;
    bool bSensorInvert = false;
    printk("=>fnCsbdRunSidebySidePreview\n");
    fnCsbdResetSbu(client);
    fnCsbdInvClk(client, true, bSensorInvert, bSensorInvert);
    fnCsbdSetClkSide(client);
    fnCsbdSetTM(client,TM_MODE_NORMAL);
    ucSensorType = (pstCsiEngineInfo->ucControl1 & 0x1C)>>2;
    fnCsbdSetSnrType(client,ucSensorType, SNRSRC_AUTO);
    fnCsbdSetSbuMode(client,SBU_MODE_SIDEBYSIDE, ucMasterSensor);
    fnCsbdSetSnr0Cap(client,pstCsiEngineInfo, SBU_MODE_SIDEBYSIDE);
    fnCsbdSetSnr1Cap(client,pstCsiEngineInfo, SBU_MODE_SIDEBYSIDE);
    fnCsbdSetTG(client);
    fnCsbdSetOutWindow(client,pstCsiEngineInfo);
    fnCsbdEnableSbu(client);
    printk("<=fnCsbdRunSidebySidePreview\n");
}

void fnCsbdStartBypassMode(struct i2c_client *client, unsigned char ucMasterSensor)
{
	printk("=>fnCsbdRunSbuBypassPreview\n");
	switch(ucMasterSensor)
	{
		case SENSOR0:
			fnCsbdSetTM(client,TM_MODE_SENSOR0);
			break;
		case SENSOR1:
			fnCsbdSetTM(client,TM_MODE_SENSOR1);
			break;
	}
	printk("<=fnCsbdRunSbuBypassPreview\n");
}

/**
* @brief Print the values of the bridge ic's registers
*/
void fnCsbdPrintRegs(struct i2c_client *client)
{
	unsigned short i = 0;
	unsigned char ucValue;

	printk("        0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F \n");
	printk("-------------------------------------------------------");
	
	for(i = 0; i <= 0xFF; i++) 
	{
        stk3855_i2c_read(client,1,(unsigned char)i,&ucValue);
		if (i % 16 == 0) 
		{
			printk("\n[0x%02x] 0x%02x ", i, ucValue);
		}
		else 
		{
			printk("0x%02x ",ucValue);
		}
	}
	printk("\n");
	printk("-----------------------------------------------------\n");
}

void Receiver_config(struct i2c_client *client)
{
    STCsiEngineInfo pstcsiengineinfo;

    pstcsiengineinfo.ucControl1  = (0x03) << 2;
    pstcsiengineinfo.usSrcStartX = 0;
    pstcsiengineinfo.usSrcStartY = 0;
    pstcsiengineinfo.usSrcEndX   = 960*2 -1;//0 + 1280 * 2 -1;
    pstcsiengineinfo.usSrcEndY   = 0 + 720- 1;
    pstcsiengineinfo.usSrcWidth  = 960*2;//1280 * 2;

    fnCsbdStartSideBySideMode(client,&pstcsiengineinfo,SENSOR0);
    ////fnCsbdStartBypassMode(client,SENSOR0);/////jason add for test
    fnCsbdPrintRegs(client);
}

//2015------------------------------

#ifdef STK3855_DEBUG_IF
static int enable_status = 0;

static ssize_t stk3855_enable_show(struct class *class,
		struct class_attribute *attr, char *buf)
{
    printk("%s : hello !\n", __func__);
    fnCsbdPrintRegs(stk3855_client);
	return sprintf(buf, "%d\n", enable_status);
}

static ssize_t stk3855_enable_store(struct class *class,
				struct class_attribute *attr,
				const char *buf, size_t count)
{
    printk("%s : hello !\n", __func__);

    int tmp;
    tmp = simple_strtol(buf, NULL, 10);

	
    if (tmp != enable_status)
    {
        if (tmp == 0)
        {
            gpio_set_value(stk3855_context.reset_pads, 0);
            mdelay(100);
            enable_status = 0;
        }
	else if (tmp == 1)
        {
            gpio_set_value(stk3855_context.reset_pads, 1);
            mdelay(100);

            gpio_set_value(stk3855_context.reset_pads, 0);
            mdelay(100);

            gpio_set_value(stk3855_context.reset_pads, 1);
            mdelay(100);

            Receiver_config(stk3855_client);
            mdelay(10);
            enable_status = 1;
        }
        else if (tmp == 2)
        {
            gpio_set_value(stk3855_context.reset_pads, 1);
            mdelay(100);

            gpio_set_value(stk3855_context.reset_pads, 0);
            mdelay(100);

            gpio_set_value(stk3855_context.reset_pads, 1);
            mdelay(100);

            fnCsbdStartBypassMode(stk3855_client,SENSOR1);
            mdelay(10);
            enable_status = 2;
        }
        else
        {
            printk("%s : %d value invaild\n", __func__, tmp);
        }
    }
	return count;
}

static struct class_attribute stk3855_class_attrs[] = {
	__ATTR(enable, 0644, stk3855_enable_show, stk3855_enable_store),
	__ATTR_NULL
};

static struct class stk3855_class = {
	.name = "stk3855-debug",
	.class_attrs = stk3855_class_attrs,
};
#endif

static int stk3855_probe(struct i2c_client *client, 
        const struct i2c_device_id *pid)
{
    int ret = 0;
    unsigned char ucValue = 0x02;
	unsigned int regvalue;
    if (item_exist("sensor.senrst.pads")) 
    {
        stk3855_context.reset_pads = item_integer("sensor.senrst.pads", 0);
    }

    ret = gpio_request(stk3855_context.reset_pads, "senrst_pin");
    if(ret != 0 )
    {
        printk("sensor reset gpio_request error!\n");
        return ret;
    }

    gpio_direction_output(stk3855_context.reset_pads, 0);
    mdelay(5);

    gpio_set_value(stk3855_context.reset_pads, 1);
    mdelay(100);

    gpio_set_value(stk3855_context.reset_pads, 0);
    mdelay(100);

    gpio_set_value(stk3855_context.reset_pads, 1);
    mdelay(100);

//    Receiver_config(client);
//    mdelay(10);

    //fnCsbdStartBypassMode(client,SENSOR0);
    //mdelay(10);

    ret = stk3855_i2c_read(client,sizeof(regvalue),0x00,(u8 *)&regvalue);
    printk("1029-stk3855 reg 0x00 = 0x%02x\n", regvalue);
    printk("1029-stk3855 reg 0x00 size =%d \r\n", sizeof(regvalue));
    printk("------------------27----------------\n");
    printk("-----------------------------------\n");
    printk("-----------------------------------\n");
#ifdef STK3855_DEBUG_IF
    class_register(&stk3855_class);
    enable_status = 1;
#endif

    return 0;
}

static int stk3855_remove(struct i2c_client *client)
{

    gpio_set_value(stk3855_context.reset_pads, 0);

    gpio_free(stk3855_context.reset_pads);

#ifdef STK3855_DEBUG_IF
	class_unregister(&stk3855_class);
#endif

    return 0;
}

static struct i2c_driver stk3855_driver = {
    .driver = {
        .owner = THIS_MODULE,
        .name = STK3855_I2C_NAME,
    },
    .probe = stk3855_probe,
    .remove = stk3855_remove,
    .id_table = stk3855_id,
};

static int __init stk3855_driver_init(void)
{
    int ret; 

    stk3855_adapter = i2c_get_adapter(STK3855_ADAPTER_ID);
    if (!stk3855_adapter) {
        printk("stk3855 get_adapter error!\n");
        return -ENODEV;
    }

    stk3855_client = i2c_new_device(stk3855_adapter, &stk3855_info);
    if (!stk3855_client) {
        printk("stk3855 i2c_new_device error!\n"); 
        return -EINVAL;
    }

    ret = i2c_add_driver(&stk3855_driver);
    if (ret) {
        printk("stk3855 i2c_add_driver error!\n");
        return ret;
    }

    return ret;
}

static void __exit stk3855_driver_exit(void)
{
    i2c_del_driver(&stk3855_driver);

    i2c_unregister_device(stk3855_client);

    i2c_put_adapter(stk3855_adapter);

    return;
}

module_init(stk3855_driver_init);

module_exit(stk3855_driver_exit);

MODULE_AUTHOR("jiawen");
MODULE_DESCRIPTION("STK3855 camera sensor brigde controller driver.");
MODULE_LICENSE("GPL");
