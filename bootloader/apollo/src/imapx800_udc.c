/*
 * udc.c
 * 
 * Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved. 
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 *
 * spz <spz.c@gmail.com>.
 *      
 * Revision History: 
 * -----------------
 * 06/22/2011
 */

//#include <common.h>
#include <command.h>
#include <asm-arm/arch-imapx800/imapx_base_reg.h>
#include <udc.h>
#include <imapx_udc.h>
#include <asm-arm/io.h>
#include <malloc.h>
#include <lowlevel_api.h>
#include <preloader.h>
extern size_t strlen(const char *s);

#define min(X, Y)				\
	({ typeof (X) __x = (X), __y = (Y);	\
		(__x < __y) ? __x : __y; })

#define max(X, Y)				\
	({ typeof (X) __x = (X), __y = (Y);	\
		(__x > __y) ? __x : __y; })

#define MIN(x, y)  min(x, y)
#define MAX(x, y)  max(x, y)
/* USE Google's VID */ 
#define DEVICE_VENDOR_ID  0x18d1 
/* This is just made up.. */
#define DEVICE_PRODUCT_ID 0xcafe
/* This is just made up.. */
#define DEVICE_BCD        0x0311

struct udc_dev udc;
const static struct usb_device_descriptor
device_descriptor = {
	.bLength = USB_DT_DEVICE_SIZE,
	.bDescriptorType = USB_DT_DEVICE,
	.bcdUSB = 0x200, /* usb version 2.0 */
	.bDeviceClass = 0xff,
	.bDeviceSubClass = 0xff,
	.bDeviceProtocol = 0xff,
	.bMaxPacketSize0 = UDC_MAX_PACKET_EP0,
	.idVendor           = DEVICE_VENDOR_ID,
	.idProduct          = DEVICE_PRODUCT_ID,
	.bcdDevice          = DEVICE_BCD,
	.iManufacturer      = DEVICE_STRING_MANUFACTURER_INDEX,
	.iProduct           = DEVICE_STRING_PRODUCT_INDEX,
	.iSerialNumber      = DEVICE_STRING_SERIAL_NUMBER_INDEX,
	.bNumConfigurations = 1,
};
#define FASTBOOT_INTERFACE_CLASS     0xff
#define FASTBOOT_INTERFACE_SUB_CLASS 0x42
#define FASTBOOT_INTERFACE_PROTOCOL  0x03
#define CONFIGURATION_NORMAL      1
static struct udc_settings_descriptor
settings_descriptor = {
	.cdesc = {
		.bLength = USB_DT_CONFIG_SIZE,
		.bDescriptorType = USB_DT_CONFIG,
		/* Set this to the total we want */                                  
		.bNumInterfaces      = 1,                                           
		.bConfigurationValue = CONFIGURATION_NORMAL,                        
		.iConfiguration      = DEVICE_STRING_CONFIG_INDEX,                  
		.bmAttributes        = 0xc0,                                        
		.bMaxPower           = 0x32,                                        

	},
	.idesc = {
		.bLength = USB_DT_INTERFACE_SIZE,
		.bDescriptorType     = USB_DT_INTERFACE,
		.bInterfaceNumber    = 0x00,
		.bAlternateSetting   = 0x00,
		.bNumEndpoints       = 0x02,
		.bInterfaceClass     = FASTBOOT_INTERFACE_CLASS,
		.bInterfaceSubClass  = FASTBOOT_INTERFACE_SUB_CLASS,
		.bInterfaceProtocol  = FASTBOOT_INTERFACE_PROTOCOL,
		.iInterface          = DEVICE_STRING_INTERFACE_INDEX,
	},
	.epIN = {
		.bLength = USB_DT_ENDPOINT_SIZE,
		.bDescriptorType    = USB_DT_ENDPOINT,                 
		.bEndpointAddress   = 0x80 | UDC_EP_IN, /* IN */   
		.bmAttributes       = USB_ENDPOINT_XFER_BULK,          
		.wMaxPacketSize     = UDC_MAX_PACKET, 
		.bInterval          = 0x00,                            
	},
	.epOUT = {
		.bLength = USB_DT_ENDPOINT_SIZE,
		.bDescriptorType    = USB_DT_ENDPOINT,        
		.bEndpointAddress   = UDC_EP_OUT,/* OUT */
		.bmAttributes       = USB_ENDPOINT_XFER_BULK, 
		.wMaxPacketSize = UDC_MAX_PACKET,
		.bInterval = 0,
	}
};
static struct usb_qualifier_descriptor q_descriptor = {
	.bDescriptorType    = USB_DT_DEVICE_QUALIFIER,
	.bcdUSB             = 0x200,
	.bDeviceClass       = 0xff,
	.bDeviceSubClass    = 0xff,
	.bDeviceProtocol    = 0xff,
	.bMaxPacketSize0    = 0x40,
	.bNumConfigurations = 1,
	.bRESERVED          = 0,
};
char *device_strings[DEVICE_STRING_MANUFACTURER_INDEX+1];
struct usb_string_descriptor string_descriptor={
	.bLength = 4,      
	.bDescriptorType = USB_DT_STRING ,
	.wData[0]= DEVICE_STRING_LANGUAGE_ID ,          
};
static u8 faddr = 0xff;
char udc_serial[32] = "IROM12345678";

//static char board_name[64];
int udc_write(const uint8_t *buf, int len);
//#define UDC_DEBUG
#ifdef UDC_DEBUG
#define udc_log(fmt,args...) irf->printf(fmt,##args)
static void print_crq(struct usb_ctrlrequest *crq)
{
	udc_log("ep0 setup: bRequestType=%x, bRequrest=%x, wValue=%x"
			", wIndex=%x, wLength=%x\n", crq->bRequestType,
			crq->bRequest, crq->wValue,
			crq->wIndex, crq->wLength);

	return ;
}
#else
#define udc_log(fmt,args...)
#define print_crq(x) 
#endif

static void dwc_otg_flush_all_fifo(void)
{
    uint32_t val;

	val = readl(GRSTCTL);
	val &=~(0x1f<<6);
	val |= (0x10<<6);
	val |= (3<<4);
	writel(val, GRSTCTL);

	while((readl(GRSTCTL)&(3<<4)) != 0){
		irf->udelay(1);
	}
}

static int udc_rx(void)
{
	uint32_t intr_state, val;

	intr_state = readl(DOEPINT(UDC_EP_OUT));
	udc_log("udc_rx:0x%x\n", intr_state);	
	if(intr_state & 0x1) {
		/* xfer complete */
		val = readl(DOEPTSIZ(UDC_EP_OUT));
		udc_log("doeptsiz:0x%x\n", val);
		if((val&0x7ffff) == UDC_MAX_PACKET)
		{
			/* start ep transfer */

			writel(0x80000|udc.rx_ram, DOEPTSIZ(UDC_EP_OUT));
			val = readl(DOEPCTL(UDC_EP_OUT));
			val |= 0x84000000;
			writel(val, DOEPCTL(UDC_EP_OUT));
		}else {
			udc.rx_en = 0;
		}
	}
	writel(intr_state&0x5, DOEPINT(2));
	return 0;
}

static int udc_tx(void)
{
	uint32_t intr_state, val;

	intr_state = readl(DIEPINT(UDC_EP_IN));
	udc_log("udc_tx:0x%x\n",intr_state);
	udc_log("udc_tx:0x%x\n",intr_state);
	if(intr_state & 0x1) {
		val = readl(DIEPTSIZ(UDC_EP_IN));
		udc_log("dieptsiz1:0x%x\n", val);
		if((val&0xfffffff) == 0)
			udc.tx_en = 0;
	}
	writel(intr_state&0x5, DIEPINT(UDC_EP_IN));
	return 0;
}

static void otg_phy_configure(void)
{
	uint32_t val;

	/* set OTG default values */
	writel(4, OTG_SYSM_ADDR + 0x3c);
	writel(3, OTG_SYSM_ADDR + 0x40);
	writel(4, OTG_SYSM_ADDR + 0x44);
	writel(3, OTG_SYSM_ADDR + 0x48);
	writel(3, OTG_SYSM_ADDR + 0x4c);
	writel(3, OTG_SYSM_ADDR + 0x50);
	writel(1, OTG_SYSM_ADDR + 0x54);
	writel(1, OTG_SYSM_ADDR + 0x58);
	writel(0, OTG_SYSM_ADDR + 0x5c);
	writel(0, OTG_SYSM_ADDR + 0x60);

	/* configure phy rf clock*/
	val = readl(OTG_SYSM_ADDR + 0x20);
	val &= ~0x7;
	val |= 0x5;
	writel(val, OTG_SYSM_ADDR + 0x20);
	/* configure usb utmi+ data_len */
	val = readl(OTG_SYSM_ADDR + 0x24);
	val |= 0x3;
	writel(val, OTG_SYSM_ADDR + 0x24);
	/* configure port0 sleep mode to resume */
	val = readl(OTG_SYSM_ADDR + 0x2c);
	val |= 0x1;
	writel(val, OTG_SYSM_ADDR + 0x2c);

	/* reset usb-host module */
	val = readl(OTG_SYSM_ADDR);
	val |= 0xf;
	writel(val, OTG_SYSM_ADDR);
	irf->udelay(1);
	val &=~0xe;
	writel(val, OTG_SYSM_ADDR);
	irf->udelay(1);
	val &=~0x1;
	writel(val, OTG_SYSM_ADDR);
	irf->udelay(1);

}
static void init_string(void)
{
	device_strings[DEVICE_STRING_MANUFACTURER_INDEX]  = "InfoTMIC";
	device_strings[DEVICE_STRING_PRODUCT_INDEX]       = "iROM:LinkPC";
	device_strings[DEVICE_STRING_SERIAL_NUMBER_INDEX] = udc_serial;
	device_strings[DEVICE_STRING_CONFIG_INDEX]        = "Android Fastboot";
	device_strings[DEVICE_STRING_INTERFACE_INDEX]     = "Android Fastboot";
	return;
}
static void udc_bulk_enable(void)
{
/* enable endpoint_1_in & endpoint_2_out*/
//	writel(UDC_MAX_PACKET|0x18088000, DIEPCTL(1));
	writel(UDC_IN_MAX_PACKET|0x18088000, DIEPCTL(UDC_EP_IN));
	writel(UDC_MAX_PACKET|0x18088000, DOEPCTL(UDC_EP_OUT));
	//writel(0x40|0x18088000, DOEPCTL(2));
	int val = readl(DAINTMSK);
	val |= 0x40002;
	writel(val, DAINTMSK);

}

static int udc_init(void)
{
	uint32_t val;

	faddr = 0xff;

	/* reset udc values */
	irf->memset(&udc, 0, sizeof(struct udc_dev));
	//udc.link_ok = 0;
	//udc.tx_en = 0;
	//udc.rx_en = 0;
	//udc.buff_setup = 0;

	if(!udc.buff_setup)
	  udc.buff_setup = irf->malloc(sizeof(udc.crq)*5);

	//irf->printf("buf_setup: 0x%p\n", udc.buff_setup);
	udc.ep0_sts = EP0_IDLE;

	irf->module_enable(OTG_SYSM_ADDR);
	writel(0xff, OTG_SYSM_ADDR);
	irf->udelay(10);
	writel(0x0, OTG_SYSM_ADDR);
	
	otg_phy_configure();

	/* program GUSBCFG: phy interface = 16bit && srp = 0 && hnp = 0 
	   && usb turnaroud time = 5 && force device mode = 1*/
	writel(0x4C001408, GUSBCFG);

	do{
		val = readl(GRSTCTL);
		irf->udelay(40);
	}while(!(val&0x80000000));
	/* udc core reset */
	val = readl(GRSTCTL);
	val |= 0x1;
	writel(val, GRSTCTL);
	irf->udelay(10);
	do {
		val = readl(GRSTCTL);
		irf->udelay(10);
	}
	while(val&0x1);		

	/* program GAHBCFG: global interrupt mask = 1 && burst lenght = INCR */
	writel(0x27, GAHBCFG);

	udc_log("buf_setup: 0x%p\n", udc.buff_setup);
	/* check_out GUID: GUID value */
	val = readl(GUID);
	if(val != 0x12345678) {
		udc_log("warning! imapx800 otg control guid=0x%x\n", val);
	}

	/* check_out GINTSTS: otg control mode status */
	val = readl(GINTSTS);
	if(val & 0x1) {
		udc_log("warning! imapx800 otg control is host mode\n");
		return -1;
	}

	/* program DCFG: device speed = 0 && non-zero-len status 
	   OUT handshake = 0 && desc-DMA = 0  */
	writel(0x4000000, DCFG);

	/* program DTHRCTL: non-ISO endpoint threshold = 1 && transmit threshold len = 8 
	   && AHB threshold ratio = 0 && rcv trh = 1 && rcv trh len = 8*/

	/* program GINTMSK: usb reset = 1 && enumation done = 1 && early suspend = 1 && 
	   usb suspend = 1 */
	writel(0xc3c00, GINTMSK);


	init_string();


	return 0;
}
static void req_set_addr(void)
{
	faddr = (uint8_t) (udc.crq.wValue & 0x7f);
	uint32_t val;
	val = readl(DCFG);
	val &=~0x7f0;
	val |= ((udc.crq.wValue&0x7f)<<4);
	writel(val, DCFG);

}
static void ack_req(void)
{
	/* send zero-len status packet */
	udc_log("ack_req\n");
	writel(0x80000, DIEPTSIZ(0));
	writel((uint32_t)udc.buff_setup, DIEPDMA(0));
	writel(0x84000000, DIEPCTL(0));

}
static int req_get_descriptor(void)
{
	unsigned char* buf = irf->malloc(UDC_MAX_PACKET_EP0);
	//irf->printf("buf:0x%p\n", buf);
	if(!buf) {
		udc_log("alloc transfer buffer failed.\n");
		return -1;
	}

	switch(udc.crq.wValue>>8) {
		case USB_DT_DEVICE:
			udc_log("USB_DT_DEVICE\n");
			irf->memcpy(buf, &device_descriptor, USB_DT_DEVICE_SIZE);
			udc.tx_len  = USB_DT_DEVICE_SIZE;
			break;
		case USB_DT_CONFIG:
			udc_log("USB_DT_CONFIG\n");
			settings_descriptor.cdesc.wTotalLength = sizeof(struct udc_settings_descriptor);
			irf->memcpy(buf, &settings_descriptor, sizeof(struct udc_settings_descriptor));
			udc.tx_len  = sizeof(struct udc_settings_descriptor);
			break;
		case USB_DT_DEVICE_QUALIFIER:
			udc_log("get descriptor wvalue 0x600\n");
			q_descriptor.bLength = MIN(udc.crq.wLength, sizeof(struct usb_qualifier_descriptor));
			irf->memcpy(buf, &q_descriptor, sizeof(struct usb_qualifier_descriptor));
			udc.tx_len  = sizeof(struct usb_qualifier_descriptor);                     
			break;                                                         
		case USB_DT_STRING:
			udc_log("get string descriptor");
			int s_index = udc.crq.wValue&0xff;
#if 1 
			udc_log(" index:%d\n",s_index);
			if(s_index == 0 ||DEVICE_STRING_MAX_INDEX < s_index){
				string_descriptor.bLength =  MIN(udc.crq.wLength, 
						sizeof(struct usb_string_descriptor ));
				irf->memcpy(buf, &string_descriptor, sizeof(struct usb_string_descriptor));
				udc.tx_len  = sizeof(struct usb_string_descriptor);
			}else{ 
				/* Size of string in chars */                                
				unsigned char s;                                             
				unsigned char sl = strlen (device_strings[s_index]);
				/* Size of descriptor                                        
				 *    1    : header                                             
				 *       2    : type                                               
				 *          2*sl : string */                                          
				unsigned char bLength = 2 + (2 * sl);                        
				bLength = MIN(bLength, udc.crq.wLength);                         
				//bLength  =   udc.crq.wLength ;
				char *string = (char*) buf;
				string[0] = bLength;
				string[1] = USB_DT_STRING;

				udc_log("%s\n", device_strings[s_index]);

				for (s = 1; s < (sl+1);)                                         
				{                                                                
					string[s*2] = device_strings[s_index][s-1]; 
					string[s*2 + 1]= 0;
					s += 1;
				} 

				//irf->memcpy(buf,device_strings[s_index], sl);
				udc.tx_len  =  bLength;
				udc_log("length: %d\n",bLength);
				print_crq(&udc.crq);
			}
#endif
			break;
		default:
			ack_req();
			print_crq(&udc.crq);
			udc_log("__________get descriptor not respond____________\n");
			goto __exit__;
	}
	udc.tx_ep   = 0; 
	udc.tx_addr = (uint32_t) buf;
	udc.tx_len  = min(udc.crq.wLength, udc.tx_len);
	/* start ep0 transfer */
	writel(0x80000|udc.tx_len, DIEPTSIZ(0));
	writel(udc.tx_addr, DIEPDMA(0));
	writel(0x84000000, DIEPCTL(0));
__exit__:
	if(buf)
		irf->free(buf);
	return 0;
}
static void req_get_status(void)
{
	udc_log("USB_REQ_GET_STATUS: recipent device\n");
	if (0 == udc.crq.wLength)                          
	{                                              
		ack_req();                                 
	}                                              
	else                                           
	{                                              
		/* See 9.4.5 */                            
		unsigned char bLength;                     
		char fastboot_fifo[2];
		bLength = MIN (udc.crq.wValue, 2);             

		fastboot_fifo[0] = USB_STATUS_SELFPOWERED; 
		fastboot_fifo[1] = 0; 

		udc.tx_ep   = 0;                               
		udc.tx_addr = (uint32_t) fastboot_fifo;                  
		//		udc.tx_len  = 2;//min(udc.crq.wLength, bLength);
		udc.tx_len  = min(udc.crq.wLength, bLength);
		/* start ep0 transfer */                       
		writel(0x80000|udc.tx_len, DIEPTSIZ(0));       
		writel(udc.tx_addr, DIEPDMA(0));               
		writel(0x84000000, DIEPCTL(0));                
		udc_log("req get status length: %d\n",bLength);
	}
}
static void req_set_config(void)
{
	if (0xff == faddr) {
	} else {
		if (0 == udc.crq.wValue) {
			/* spec says to go to address state.. */                         
			udc_log("spec says to go to address state.. \n");
			faddr = 0xff;
			ack_req();
		} else if (CONFIGURATION_NORMAL == udc.crq.wValue) {
			/* This is the one! */
			/* Bulk endpoint fifo */
			udc_bulk_enable();
			ack_req();
		} else {
			/* Only support 1 configuration so nak anything else */          
			udc_log("Only support 1 configuration so nak anything else\n");
		}
	}       
	return ;
}
static void ep0_setup(void)
{
	if(udc.crq.bRequestType & 0x80)
	{
		udc.ep0_sts = EP0_IN_DATA_PHASE;
	}
	else {
		udc.ep0_sts = EP0_OUT_DATA_PHASE;
	}
	if(udc.crq.wLength == 0) {
		udc.ep0_sts = EP0_IN_STATUS_PHASE;
	}
	switch(udc.crq.bRequest)
	{
		/* do_get_descriptor */
		case 0x6:
			udc_log("do_get_descriptor\n");
			req_get_descriptor();
			break;
			/* do_set_address */
		case 0x5:
			udc_log("do_set_address\n");
			if(faddr == 0xff){
				req_set_addr();
				ack_req();
			}
			break;
			/* do_set_configure */
		case 0x9:
			udc_log("do_set_configure\n");
			udc_log("%d\n", udc.crq.wValue);
			req_set_config();
			break;
		case USB_REQ_GET_STATUS:
			udc_log("USB_REQ_GET_STATUS\n");
			if (USB_RECIP_DEVICE == (udc.crq.bRequestType & USB_RECIP_MASK)) {
				req_get_status();	 
			}else{
				udc_log("%x\n",udc.crq.bRequestType);
				udc_log("\n\n\n((((((((((((((((((((())))))))))))))\\n\n\n");
			}
			break;
		default:
			print_crq(&udc.crq);
			break;

	}

	return ;
}

static void ep0_out_intr(void)
{
	uint32_t doepint, val;

	doepint = readl(DOEPINT(0));
	udc_log("dopt:0x%x %d 0x%x\n", doepint, udc.ep0_sts, readl(DOEPTSIZ(0)));
	/* xfer complete */
	if(doepint & 0x1) {
		/* clear xfer-complete interrupt source */
		switch(udc.ep0_sts)
		{
			case EP0_OUT_DATA_PHASE:                 
				/* send zero-len in-status packet */ 
				udc_log("************EP0_OUT_DATA_PHASE**********\n");
				writel(0x80000, DIEPTSIZ(0));        
				writel(udc.tx_addr, DIEPDMA(0));     
				writel(0x84000000, DIEPCTL(0));      
				udc.ep0_sts = EP0_IN_STATUS_PHASE;   
				break;                               
			case EP0_IN_DATA_PHASE:                  
				/* send zero-len out-status packet */
				udc_log("EP0_IN_DATA_PHASE:**********\n");
				val = readl(DOEPTSIZ(0));            
				val &=~0x7f;                         
				val |= 0x80000;                      
				writel(val, DOEPTSIZ(0));            
				writel(udc.tx_addr, DOEPDMA(0));     
				writel(0x84000000, DOEPCTL(0));      
				udc.ep0_sts = EP0_OUT_STATUS_PHASE;  
				break;                               

			case EP0_IN_STATUS_PHASE:
			case EP0_OUT_STATUS_PHASE:
				udc.ep0_sts = EP0_IDLE;
				/* configure out-endpoint for next setup packet */
				udc_log("EP0_OUT_STATUS_PHASE: configure out-endpoint for next setup packet\n");
				writel(0x60080040, DOEPTSIZ(0));
				writel((uint32_t)udc.buff_setup, DOEPDMA(0));
				writel(0x84000000, DOEPCTL(0));
				break;
			case EP0_IDLE:
				udc_log("ep0_out_intr: Idle\n");
				break;
			default:
				udc_log("ep0_out_intr: xfer complete status unknow %d\n",udc.ep0_sts);
				break;
		}
	}
	/* ahb error */
	if(doepint & 0x4) {
		/* clear ahb error interrupt source */
		writel(0x4, DOEPINT(0));

		udc_log("ep0_out_intr: ahb error!\n");
	}
	/* setup done */
	if(doepint & 0x8) {
		/* back2back setup detect */
		if(doepint & 0x40) {
			val = readl(DOEPDMA(0));
			*(((uint32_t *)&udc.crq) + 0) = val-8;
			*(((uint32_t *)&udc.crq) + 1) = val-4;
		}
		else {
			/* get remnant packet size */
			*(((uint32_t *)&udc.crq) + 0) = *(((uint32_t *)udc.buff_setup)+0);
			*(((uint32_t *)&udc.crq) + 1) = *(((uint32_t *)udc.buff_setup)+1);
		}
		ep0_setup();
	}

	writel(doepint&(0x209), DOEPINT(0));
}

static void ep0_in_intr(void)
{
	uint32_t diepint, val;

	diepint = readl(DIEPINT(0));
	udc_log("dipt:0x%x %d 0x%x\n",diepint, udc.ep0_sts, readl(DOEPCTL(0)));
	/* xfer complete */
	if(diepint & 0x1) {
		switch(udc.ep0_sts)
		{
			case EP0_OUT_DATA_PHASE:                
				/* send zero-len in-status packet */
				udc_log("EP0_OUT_DATA_PHASE:\n");
				val = readl(DIEPTSIZ(0));           
				val |= 0x80000;                     
				writel(val, DIEPTSIZ(0));           
				writel(udc.tx_addr, DIEPDMA(0));    
				writel(0x84000000, DIEPCTL(0));     
				udc.ep0_sts = EP0_IN_STATUS_PHASE;  
				break;                              

			case EP0_IN_DATA_PHASE:
				/* send zero-len out-status packet */
				udc_log("doepctl:0x%x 0x%x\n", readl(DOEPCTL(0)), readl(DIEPTSIZ(0)));
				val = readl(DOEPTSIZ(0));
				val &=~0x7f;
				val |= 0x80000;
				writel(val, DOEPTSIZ(0));
				writel((uint32_t)udc.buff_setup, DOEPDMA(0));
				writel(0x84000000, DOEPCTL(0));	
				udc.ep0_sts = EP0_OUT_STATUS_PHASE;	
				break;
			case EP0_IN_STATUS_PHASE:
			case EP0_OUT_STATUS_PHASE:

				if(udc.crq.bRequest == 9){
					udc_log("udc.crq.bRequest == 9");
					udc.link_ok = 1;

					writel(UDC_MAX_PACKET|0x18088000, DIEPCTL(1));
					writel(0x40|0x18088000, DOEPCTL(2));
					val = readl(DAINTMSK);
					val |= 0x40002;
					writel(val, DAINTMSK);
				}
				udc.ep0_sts = EP0_IDLE;
				/* configure out-endpoint for next setup packet */
				udc_log(" configure out-endpoint for next setup packet\n");
				writel(0x60080040, DOEPTSIZ(0));
				writel((uint32_t)udc.buff_setup, DOEPDMA(0));
				writel(0x84000000, DOEPCTL(0));

				break;
			default:
				udc_log("ep0_in_intr: xfer complete status unknow!\n");
				break;

		}

	}

	writel(diepint&0x20D,DIEPINT(0));
}
bool enumerated = false;
int previos = 0;
static void udc_event_handle(void)
{
	uint32_t val,int_sts;


	int_sts = readl(GINTSTS);
	//if(0== (int_sts&0xc3c00)) return;
	if(previos != int_sts){
		previos = int_sts;
		udc_log("int_sts:0x%x\n", int_sts);	
	}

	/* usb reset intr handle */
	writel(int_sts&0xc00,GINTSTS);
	switch(int_sts & 0x3000){
		case 0x1000:
			/* ep0 out set NAK */
			writel(0x8000000, DOEPCTL(0));
			/* ep2 out set NAK&mps&active&bulk*/
			writel(0x8000000, DOEPCTL(UDC_EP_OUT));
			/* unmask the following interrupt */
			writel(0x10001, DAINTMSK);
			writel(0x9, DOEPMSK);
			writel(0x9, DIEPMSK);
			/* user dma mode */
			/* program GRXFSIZ = 2048 && GNPTXSIZ = 1024 && DIEPTXF(1) = 1024*/
			writel(0x800, GRXFSIZ);
			writel((0x400<<16)|0x800, GNPTXFSIZ);
			writel((0x400<<16)|0xc00, DIEPTXF(1));
			writel(0x0c001780,GDFIFOCFG);
			/* flush all fifo after configure dynamic fifo */
			dwc_otg_flush_all_fifo();
			/* program the following for receive setup page */
			writel(0x60080040, DOEPTSIZ(0));
			writel((uint32_t)udc.buff_setup, DOEPDMA(0));
			writel(0x1000, GINTSTS);
			irf->udelay(10);
			udc_log("rst\n");
			break;
		case 0x2000: /* usb enumeration done intr handle*/
			val = readl(DSTS);
			udc.speed = (val>>1)&0x3;
			//	udc_log("speed: 0x%x\n", udc.speed);
			writel(0x84000000, DOEPCTL(0));
			/* umask sof interrupt */
			val = readl(GINTMSK);
			val |= 0x8;
			writel(val, GINTMSK); 
			writel(0x2000, GINTSTS);
			udc_log("enum\n");
			enumerated = true;
			break;
		default:
			break;
	}
	val = readl(DAINT);
	//	udc_log("0x%8x\n",val);
	if(int_sts & 0x40000){
		if(val & 0x1)
			ep0_in_intr();
		if(val & 0x2)
			udc_tx();

	}
	if(int_sts & 0x80000){
		if(val & 0x10000)
			ep0_out_intr();
		if(val & 0x40000)
			udc_rx();
	}

}
int udc_get_cable_state(void)
{
	return 1;
}
int udc_disconnect(void)
{
	udc.link_ok = 0;
	writel(readl(GRSTCTL)|0x1,GRSTCTL);
	while(readl(GRSTCTL)&0x1)irf->udelay(10);
	udc_log("soft disconnected\n");
	return 0;
}
int udc_connect(void)
{
	int ret;
	int 	t = 0;
	udc_log("%s\n",__func__);	
	//	cfg_mmu_table();
	ret = udc_init();
	if(ret < 0) {
		udc_log("initialize udc failed. ret=%d\n", ret);
		return ret;
	}
	//retry:
	/* poll udc event handle until configuration set */
	t = get_timer(0);
	for(;;){
		udc_event_handle();
		if(udc.link_ok)
		  break;
		else if(get_timer(t) > 1000) // can not connect in 1s
		//else if(get_timer(t) > 100000) // can not connect in 1s
		{
			udc_log("interact with PC failed.\n");
			return -1;
		}
	}

	udc_bulk_enable();
	return 0;
}
int cable_in = 1;
/* len < 512kB */
int udc_read(uint8_t *buf, int len)
{
	uint32_t val, pkt_num;

	udc_log("udc_read: 0x%x 0x%x ..\n", (unsigned int)buf, len);

	udc.rx_addr = (uint32_t) buf;
	while(len){
		udc.rx_len  = len>0x40000? 0x40000:len;
		udc.rx_en   = 1;
		/* prepare configure for read data */
		pkt_num = (udc.rx_len + (UDC_MAX_PACKET-1))>>9;
		writel((pkt_num<<19)|udc.rx_len, DOEPTSIZ(UDC_EP_OUT));
		writel(udc.rx_addr, DOEPDMA(UDC_EP_OUT));
		val = readl(DOEPCTL(UDC_EP_OUT));
		val |= 0x84000000;
		writel(val, DOEPCTL(UDC_EP_OUT));
		//	int timemax = t+0x4000;
		while(udc.rx_en&& cable_in)
		{
			udc_event_handle();
			irf->udelay(1);
		}
		if(udc.rx_en){
			udc_log("read time out\n");
			return -1;
		}
		udc_log("%s\n",buf);
		len -= udc.rx_len;
		udc.rx_addr += udc.rx_len;
	}
	udc_log("OK\n");
	return len;
}


/* len < 512kB */
int udc_write(const uint8_t *buf, int len)
{
	uint32_t val, pkt_num;
	udc_log("udc_write: 0x%x 0x%x %s ..", (unsigned int)buf, len,(char*) buf);
	
	udc.tx_ep   = 1;
	udc.tx_addr = (uint32_t) buf;
	while(len){
		udc.tx_len  = len>0x40000? 0x40000:len;
		udc.tx_en   = 1;
		/* prepare configure for read data */
		pkt_num = (udc.tx_len + (UDC_IN_MAX_PACKET-1))/UDC_IN_MAX_PACKET;
		writel((pkt_num<<19)|udc.tx_len, DIEPTSIZ(UDC_EP_IN));
		udc_log("0x%x 0x%x %s ..", (unsigned int)udc.tx_addr, udc.tx_en ,(char*) udc.tx_addr);
		writel(udc.tx_addr, DIEPDMA(UDC_EP_IN));
		val = readl(DIEPCTL(UDC_EP_IN));
		val |= 0x84000000;
		writel(val, DIEPCTL(UDC_EP_IN));
		while(udc.tx_en && cable_in)
			udc_event_handle();
		len -= udc.tx_len;
		udc.tx_addr += udc.tx_len;
	}
	udc_log("OK\n");
	return len;
}


