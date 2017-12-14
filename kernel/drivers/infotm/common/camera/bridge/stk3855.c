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
#include <mach/pad.h>
#include <linux/camera/csi/csi_reg.h>
#include <linux/camera/csi/csi_core.h>
#include <linux/device.h>
#include <linux/regulator/consumer.h>
#include <linux/regulator/of_regulator.h>
#include <mach/items.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/ioctl.h>
#include <linux/errno.h>
#include <mach/power-gate.h>
#include <mach/imap-iomap.h>
#include <linux/sysfs.h>
#include <linux/device.h>

#include "../sensors/stk3855.h"
#include "bridge.h"

#define STK3855_RESET_PIN (44)
#define STK3855_ADAPTER_ID (3)

#define STK3855_NAME "stk3855"
#define STK3855_DEBUG_IF

static struct infotm_bridge_dev* dev = NULL;
static struct i2c_adapter *stk3855_adapter = NULL;
static struct i2c_client  *stk3855_client  = NULL;
static struct sensor_drv_context stk3855_context = {STK3855_RESET_PIN};

static int stk3855_i2c_write(struct i2c_client *client, int datalen, u8 addr, u8 data)
{
	int ret = 0;
	u8 tmp_buf[128];
	unsigned int bytelen = 0;
	tmp_buf[0] = addr;
	bytelen+=1;

	if (datalen != 0/* && data != 0*/) {
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
	if (ret < 0) {
		bridge_log(B_LOG_ERROR, "stk3855 set data address fail!\n");
		return ret;
	}

	ret =  i2c_master_recv(client, pdata, datalen);
	if (ret < 0) {
		bridge_log(B_LOG_ERROR, "stk3855 set data address fail!\n");
		return ret; 
	}
	return ret;
}

unsigned short usTG_WD_Width;
unsigned short usTG_HI_Height;

void fnCsbdSetTG(struct i2c_client *client);
void fnCsbdEnableSbu(struct i2c_client *client);

void fnCsbdResetSbu(struct i2c_client *client)
{
	bridge_log(B_LOG_DEBUG,"=>\n");
	stk3855_i2c_write(client,1,0x00,0x02);	//reset SBU
	stk3855_i2c_write(client,1,0x06,0x00);	//reset SBU
	bridge_log(B_LOG_DEBUG,"<=\n");
}

void fnCsbdRestartTG(struct i2c_client *client)
{
	bridge_log(B_LOG_DEBUG,"=>\n");
	fnCsbdResetSbu(client);
	fnCsbdSetTG(client);
	fnCsbdEnableSbu(client);
	bridge_log(B_LOG_DEBUG,"<=\n");
}

void fnCsbdEnableSbu(struct i2c_client *client)
{
	bridge_log(B_LOG_DEBUG,"=>\n");
	stk3855_i2c_write(client,1,0x00,0x01);
	bridge_log(B_LOG_DEBUG,"<=\n");
}

void fnCsbdDisableSbu(struct i2c_client *client)
{
	bridge_log(B_LOG_DEBUG,"=>\n");
	stk3855_i2c_write(client,1,0x00,0x00);
	bridge_log(B_LOG_DEBUG,"<=\n");
}

/*---------------for CSI_bridge PIO setting---------------------*/
/*ucPIO_HCO: true = set clock inversion from host , false= don't invert*/ 
/*invert clock, reg A4H[2]*/
/*C1I: true = invert internal phase clock of SBU to latch sensor 0 , false= don't invert*/
/*C0I: true = invert internal phase clock of SBU to latch sensor 1 , false= don't invert*/

void fnCsbdInvClk(struct i2c_client *client, bool ucPIO_HCO, bool C1I, bool C0I)
{
	unsigned char ucValue;

	bridge_log(B_LOG_DEBUG,"=>\n");
	/*set clock inversion register A4H*/
	ucValue = C0I | (C1I << 1) | (ucPIO_HCO << 2);
	stk3855_i2c_write(client,1,0xa4,ucValue);
	bridge_log(B_LOG_DEBUG,"<=\n");
}

/*fnCsbdSetClkClkmSel: select SBU clock source: 
0: sensor 0, 1:sensor 1, 2:host clock 3:divided PLL clock*/
void fnCsbdSetClkClkmSrc(struct i2c_client *client, unsigned char ucClkmSrc)
{
	unsigned char ucValue;

	bridge_log(B_LOG_DEBUG,"=>\n");
	stk3855_i2c_read(client,1,0xa3,&ucValue);
	ucValue = (ucValue & 0xE7) | (ucClkmSrc << 3);
	stk3855_i2c_write(client,1,0xa3,ucValue);
	bridge_log(B_LOG_DEBUG,"<=\n");
}

/*set PLL power down : 1: power down PLL, 0: PLL normal operation*/
void fnCsbdSetPllPwrDown(struct i2c_client *client, int PllPowerDown)
{
	unsigned char ucValue;

	bridge_log(B_LOG_DEBUG,"=>\n");
	stk3855_i2c_read(client,1,0xa2,&ucValue);
	ucValue = (ucValue & 0xFE) | (PllPowerDown);
	stk3855_i2c_write(client,1,0xa2,ucValue);
	bridge_log(B_LOG_DEBUG,"<=\n");
}

void fnCsbdSetPllSrc(struct i2c_client *client, unsigned char ucPllSrc)
{
	unsigned char ucValue;

	bridge_log(B_LOG_DEBUG,"=>\n");
	stk3855_i2c_read(client,1,0xa2,&ucValue);
	ucValue = (ucValue & 0x3F) | (ucPllSrc << 6);
	stk3855_i2c_write(client,1,0xa2,ucValue);
	bridge_log(B_LOG_DEBUG,"<=\n");
}

/*select clock (SBU outputclock =  sensor 0 pixel clock * 2) for side by side mode */
void fnCsbdSetClkSide(struct i2c_client *client)
{	
	bridge_log(B_LOG_DEBUG,"=>\n");
	//power up pll
	fnCsbdSetPllPwrDown(client,0);	
	//select clock source as 3:divided PLL clockl
	fnCsbdSetClkClkmSrc(client,3);
	fnCsbdSetPllSrc(client, 0);
	bridge_log(B_LOG_DEBUG,"<=\n");
}

void fnCsbdSetClkNonSideBySide(struct i2c_client *client, unsigned char ucSnrSel)
{
	bridge_log(B_LOG_DEBUG,"=>\n");
	//power down pll
	fnCsbdSetClkClkmSrc(client,ucSnrSel);
	fnCsbdSetPllPwrDown(client,1);
	bridge_log(B_LOG_DEBUG,"<=\n");
}

/*disable  sensor serial communications from host, select SENSOR0 or SENSOR1*/
void fnCsbdSetSnrDisable(struct i2c_client *client, unsigned char ucSnrSel)
{
	unsigned char ucValue;

	bridge_log(B_LOG_DEBUG,"=>\n");
	stk3855_i2c_read(client,1,0xb0,&ucValue);
	ucValue = ucValue | (1 << ucSnrSel); 
	stk3855_i2c_write(client,1,0xb0,ucValue);
	bridge_log(B_LOG_DEBUG,"<=\n");
}

/*Enable sensor serial communications from host, select SENSOR0 or SENSOR1*/
void fnCsbdSetSnrEnable(struct i2c_client *client, unsigned char ucSnrSel)
{
	unsigned char ucValue;

	bridge_log(B_LOG_DEBUG,"=>\n");
	stk3855_i2c_read(client,1,0xB0,&ucValue);
	ucValue = ucValue & (0 << ucSnrSel);
	stk3855_i2c_write(client,1,0xB0,ucValue);
	bridge_log(B_LOG_DEBUG,"<=\n");
}

/*
   ucSensortype can be set as:
   SENSORTYPE_HN_VN	SENSORTYPE_HN_VP	SENSORTYPE_HP_VN	SENSORTYPE_HP_VP	

   ucSnrSourceSel can be set as:
   SNRSRC_SNR0BUF0		SNRSRC_SNR0BUF1		SNRSRC_AUTO			
   */
void fnCsbdSetSnrType(struct i2c_client *client,
	unsigned char ucSensortype, unsigned char ucSnrSourceSel)
{
	bridge_log(B_LOG_DEBUG,"=>\n");
	/*set sensor type*/
	stk3855_i2c_write(client,1,0x04,(ucSensortype | (ucSnrSourceSel << 4)));
	bridge_log(B_LOG_DEBUG,"<=\n");
}

/*ucModeCIGBypass set bypass sensor as: SENSOR0 or SENSOR1  reg04h[15:12]*/
/*ucMatrixTypeMT set as: MT_BAYLOR_INPUT 	MT_YUV_INPUT */		
void fnCsbdSetTM(struct i2c_client *client, unsigned char ucBypassSensor)
{
	bridge_log(B_LOG_DEBUG,"=>\n");	
	stk3855_i2c_write(client,1,0xa8,(0x7e << 1)|ucBypassSensor);
	bridge_log(B_LOG_DEBUG,"<=\n");
}

/*ucModeCIGSidebySide: left side of each line is SENSOR0 or SENSOR1*/
/*ucMatrixTypeMT set as: MT_BAYLOR_INPUT 	MT_YUV_INPUT */	
void fnCsbdSetSbuMode(struct i2c_client *client,
	unsigned char ucSbuMode, unsigned char ucModeCIGSidebySide)
{
	unsigned char ucValue;

	bridge_log(B_LOG_DEBUG,"=>\n");
	ucValue = ucSbuMode | (ucModeCIGSidebySide << 4);
	stk3855_i2c_write(client,1,0x05,ucValue);
	bridge_log(B_LOG_DEBUG,"<=\n");
}

/*must set after fnCsbdSetSbuMdxxxxxx is set*/
/*must set fnCsbdSetTGSidebySide in side by side mode! , fnCsbdSetTG in other modes!*/
/* unsigned short usSnr0caphstart,unsigned short usSnr0capvstart) */
void fnCsbdSetTG(struct i2c_client *client)
{
	unsigned short ustmp;
	unsigned char ucValue;

	bridge_log(B_LOG_DEBUG,"=>\n");
	stk3855_i2c_write(client,1,0x06,0x11);	

	while(1){
		stk3855_i2c_read(client,1,0x02,&ucValue);
		if(ucValue & 0x1) break; /*detection success, end while*/
	}

	usTG_WD_Width = 0;
	usTG_HI_Height = 0;

	/*read out WD*/
	stk3855_i2c_read(client,1,0x09,&ucValue);
	bridge_log(B_LOG_INFO,"WD_Width MSB  = 0x%x\n", ucValue);
	usTG_WD_Width = (ucValue << 8);
	//SETWORDMSB(usTG_WD_Width,ucValue); /*store WB MSB*/

	stk3855_i2c_read(client,1,0x08,&ucValue);
	bridge_log(B_LOG_INFO,"WD_Width LSB  = 0x%x\n", ucValue);
	usTG_WD_Width = usTG_WD_Width + ucValue;
	//SETWORDLSB(usTG_WD_Width,ucValue); /*store WD LSB*/

	/*read out HI*/
	stk3855_i2c_read(client,1,0x0b,&ucValue);
	bridge_log(B_LOG_INFO,"HI_Height MSB = 0x%x\n", ucValue);
	usTG_HI_Height = (ucValue << 8);
	//SETWORDMSB(usTG_HI_Height,ucValue); /*store HI MSB*/

	stk3855_i2c_read(client,1,0x0a,&ucValue);
	bridge_log(B_LOG_INFO,"HI_Height LSB = 0x%x\n", ucValue);
	usTG_HI_Height = usTG_HI_Height + ucValue;
	//SETWORDLSB(usTG_HI_Height,ucValue); /*store HI LSB*/

	bridge_log(B_LOG_INFO,"WD_Width  = 0x%x\n", usTG_WD_Width);
	bridge_log(B_LOG_INFO,"HI_Height = 0x%x\n", usTG_HI_Height);

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
	bridge_log(B_LOG_DEBUG,"<=\n");
}

/*CSI_bridge set sensor 0 capture range*/

void fnCsbdSetSnr0Cap(struct i2c_client *client,
	STCsiEngineInfo* pstCsiEngineInfo, unsigned char ucSbuMode)
{
	unsigned short usSen0hstart, usSen0hend, usSen0vstart, usSen0vend;

	bridge_log(B_LOG_DEBUG,"=>\n");

	if (ucSbuMode == SBU_MODE_SIDEBYSIDE) {
		usSen0hstart = pstCsiEngineInfo->usSrcStartX + 626+120;
		usSen0hend = pstCsiEngineInfo->usSrcStartX + 626 +(pstCsiEngineInfo->usSrcWidth/2)-1-120; //99142
		//      usSen0hstart = pstCsiEngineInfo->usSrcStartX;
		//    usSen0hend = pstCsiEngineInfo->usSrcStartX +(pstCsiEngineInfo->usSrcWidth/2)-1;
		usSen0vstart = pstCsiEngineInfo->usSrcStartY;
		usSen0vend = pstCsiEngineInfo->usSrcEndY;
	} else {
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
	bridge_log(B_LOG_DEBUG,"<=\n");
}

/*CSI_bridge set sensor 1 capture range*/

void fnCsbdSetSnr1Cap(struct i2c_client *client,
	STCsiEngineInfo* pstCsiEngineInfo, unsigned char ucSbuMode)
{
	unsigned short usSen1hstart, usSen1hend, usSen1vstart, usSen1vend;

	if(ucSbuMode == SBU_MODE_SIDEBYSIDE) {
		usSen1hstart = pstCsiEngineInfo->usSrcStartX + 626+120;
		usSen1hend = pstCsiEngineInfo->usSrcStartX + 626 +(pstCsiEngineInfo->usSrcWidth/2)-1-120; //99141
		//      usSen1hstart = pstCsiEngineInfo->usSrcStartX;
		//    usSen1hend = pstCsiEngineInfo->usSrcStartX +(pstCsiEngineInfo->usSrcWidth/2)-1;
		usSen1vstart = pstCsiEngineInfo->usSrcStartY;
		usSen1vend = pstCsiEngineInfo->usSrcEndY;
	} else {
		usSen1hstart = 160; //pstCsiEngineInfo->usSrcStartX;
		usSen1hend = pstCsiEngineInfo->usSrcEndX;
		usSen1vstart = pstCsiEngineInfo->usSrcStartY;
		usSen1vend = pstCsiEngineInfo->usSrcEndY;
	}

	bridge_log(B_LOG_DEBUG,"=>\n");
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
	bridge_log(B_LOG_DEBUG,"<=\n");
}

/*if side by side mode, don't need parameter  usOuthend, 
  but need usSnr0caphend , usSnr0caphstart  , usSnr1caphend , usSnr1caphstart */

void fnCsbdSetOutWindow(struct i2c_client *client,
	STCsiEngineInfo* pstCsiEngineInfo)
{
	unsigned short ustmp;
	unsigned short usOuthstart, usOuthend, usOutvstart, usOutvend;

	bridge_log(B_LOG_DEBUG,"=>\n");

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
	if(usOuthstart == 0) {
		/*set iosync_h LSB*/
		stk3855_i2c_write(client,1,0x2c,GETWORDLSB(usTG_WD_Width));
		/*set iosync_h MSB*/
		stk3855_i2c_write(client,1,0x2d,GETWORDMSB(usTG_WD_Width));

		if(usOutvstart == 0) {
			/*set iosync_v LSB*/	
			stk3855_i2c_write(client,1,0x2e,0x00);
			/*set iosync_v MSB*/
			stk3855_i2c_write(client,1,0x2f,0x00);
		} else {
		/* output  from (0, h) , iosync from (WD, h-1)*/
		/*set iosync_v LSB*/	
		ustmp = usOutvstart - 1;
		stk3855_i2c_write(client,1,0x2e,GETWORDLSB(ustmp));
		/*set iosync_v MSB*/
		stk3855_i2c_write(client,1,0x2f,GETWORDMSB(ustmp));
		}
	} else {
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
	bridge_log(B_LOG_DEBUG,"<=\n");
}

void fnCsbdStartSideBySideMode(struct i2c_client *client,
	STCsiEngineInfo* pstCsiEngineInfo, unsigned char ucMasterSensor)
{
	unsigned char ucSensorType = 0;
	bool bSensorInvert = false;
	bridge_log(B_LOG_DEBUG,"=>\n");
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
	bridge_log(B_LOG_DEBUG,"<=\n");
}

void fnCsbdStartBypassMode(struct i2c_client *client, unsigned char ucMasterSensor)
{
	bridge_log(B_LOG_DEBUG,"=>\n");
	switch(ucMasterSensor) {
	case SENSOR0:
		fnCsbdSetTM(client,TM_MODE_SENSOR0);
		break;

	case SENSOR1:
		fnCsbdSetTM(client,TM_MODE_SENSOR1);
		break;
	}
	bridge_log(B_LOG_DEBUG,"<=\n");
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
	
	for(i = 0; i <= 0xFF; i++)  {
		stk3855_i2c_read(client,1,(unsigned char)i,&ucValue);
		if (i % 16 == 0)  {
			printk("\n[0x%02x] 0x%02x ", i, ucValue);
		} else {
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
	fnCsbdPrintRegs(client);
}

static int enable_status = 0;
static void stk3855_enable(int mode)
{
	if (mode != enable_status) {
		if (mode == 0) {
			gpio_set_value(stk3855_context.reset_pads, 0);
			mdelay(100);
			enable_status = 0;
		} else if (mode == 1) {
			gpio_set_value(stk3855_context.reset_pads, 1);
			mdelay(100);

			gpio_set_value(stk3855_context.reset_pads, 0);
			mdelay(100);

			gpio_set_value(stk3855_context.reset_pads, 1);
			mdelay(100);

			Receiver_config(stk3855_client);
			mdelay(10);
			enable_status = 1;
		} else if (mode  == 2) {
			gpio_set_value(stk3855_context.reset_pads, 1);
			mdelay(100);

			gpio_set_value(stk3855_context.reset_pads, 0);
			mdelay(100);

			gpio_set_value(stk3855_context.reset_pads, 1);
			mdelay(100);

			fnCsbdStartBypassMode(stk3855_client,SENSOR1);
			mdelay(10);
			enable_status = 2;
		} else {
			bridge_log(B_LOG_ERROR, "enable= %d value invaild\n", mode);
		}
	}
}

#ifdef STK3855_DEBUG_IF
static ssize_t stk3855_enable_show(struct class *class,
		struct class_attribute *attr, char *buf)
{
	fnCsbdPrintRegs(stk3855_client);
	return sprintf(buf, "%d\n", enable_status);
}

static ssize_t stk3855_enable_store(struct class *class,
				struct class_attribute *attr,
				const char *buf, size_t count)
{
	int tmp;
	tmp = simple_strtol(buf, NULL, 10);
	stk3855_enable(tmp);
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

int stk3855_control(unsigned int cmd,unsigned int arg)
{
	int mode;

	switch(cmd) {
	case BRIDGE_G_CFG:
		fnCsbdPrintRegs(stk3855_client);
		break;

	case BRIDGE_S_CFG:
		if (copy_from_user(&mode, (unsigned int*)arg, sizeof(int))) {
			bridge_log(B_LOG_ERROR,"BRIDGE_S_CFG copy from user error\n");
			return -EACCES;
		}
		stk3855_enable(mode);
		break;

	default:
		break;
	}

	return 0;
}

int stk3855_init(struct _infotm_bridge*bridge, unsigned int status)
{
	int ret = 0;
	unsigned int regvalue;
	struct sensor_info* info = NULL;

	if (!bridge)
		return -1;

	info = bridge->info;
	if (!info) {
		return -1;
	}

	stk3855_context.reset_pads = info->reset_pin;

	gpio_direction_output(stk3855_context.reset_pads, 0);
	mdelay(5);

	gpio_set_value(stk3855_context.reset_pads, 1);
	mdelay(100);

	gpio_set_value(stk3855_context.reset_pads, 0);
	mdelay(100);

	gpio_set_value(stk3855_context.reset_pads, 1);
	mdelay(100);

	if ( !info->client || !info->adapter)
		return -1;

	stk3855_client = info->client;
	stk3855_adapter = info->adapter;

	ret = stk3855_i2c_read(info->client,sizeof(regvalue),0x00,(u8 *)&regvalue);
	bridge_log(B_LOG_INFO,"1029-stk3855 reg 0x00 = 0x%02x\n", regvalue);
	bridge_log(B_LOG_INFO,"1029-stk3855 reg 0x00 size =%d \r\n", sizeof(regvalue));

#ifdef STK3855_DEBUG_IF
	class_register(&stk3855_class);
	enable_status = 1;
#endif

	return 0;

}

int stk3855_exit(unsigned int mode)
{
	(void)mode;
	gpio_set_value(stk3855_context.reset_pads, 0);
#ifdef STK3855_DEBUG_IF
	class_unregister(&stk3855_class);
#endif
	return 0;
}
extern int bridge_dev_register(struct infotm_bridge_dev* dev);
extern int bridge_dev_unregister(struct infotm_bridge_dev *udev);

static int __init stk3855_driver_init(void)
{
	int ret = 0;
	dev = kmalloc(sizeof(struct infotm_bridge_dev), GFP_KERNEL);
	if (!dev) {
		bridge_log(B_LOG_ERROR,"alloc infotm bridge dev failed\n");
	}

	strncpy(dev->name, STK3855_NAME, MAX_STR_LEN);
	dev->init = stk3855_init;
	dev->exit = stk3855_exit;
	dev->control = stk3855_control;

	bridge_dev_register(dev);
	return ret;
}

static void __exit stk3855_driver_exit(void)
{
	bridge_dev_unregister(dev);
	kfree(dev);
	return;
}

module_init(stk3855_driver_init);
module_exit(stk3855_driver_exit);

MODULE_AUTHOR("Davinci.Liang");
MODULE_DESCRIPTION("STK3855 brigde controller driver.");
MODULE_LICENSE("GPL/BSD");
