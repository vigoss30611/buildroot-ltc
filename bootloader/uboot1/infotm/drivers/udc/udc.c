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

#include <common.h>
#include <command.h>
#include <udc.h>
#include <asm/io.h>
#include <malloc.h>


static struct udc_dev udc = {
	.link_ok = 0,
	.tx_en = 0,
	.rx_en = 0,
};

const static struct usb_device_descriptor
device_descriptor = {
	.bLength = USB_DT_DEVICE_SIZE,
	.bDescriptorType = USB_DT_DEVICE,
	.bcdUSB = 0x200, /* usb version 2.0 */
	.bDeviceClass = 0xff,
	.bDeviceSubClass = 0x00,
	.bDeviceProtocol = 0x00,
	.bMaxPacketSize0 = UDC_MAX_PACKET_EP0,
	.idVendor = 0x5345,
	.idProduct = 0x1234,
	.bcdDevice = 0,
	.iManufacturer = 0,
	.iProduct = 0,
	.iSerialNumber = 0,
	.bNumConfigurations = 1,
};

const static struct udc_settings_descriptor
settings_descriptor = {
	.cdesc = {
		.bLength = USB_DT_CONFIG_SIZE,
		.bDescriptorType = USB_DT_CONFIG,
		.wTotalLength = 0x20,	/* config + interface + ep0 + ep1 */
		.bNumInterfaces = 0x01,
		.bConfigurationValue = 0x01,
		.iConfiguration = 0x00,
		.bmAttributes = 0xc0,
		.bMaxPower = 0x19,
	},
	.idesc = {
		.bLength = USB_DT_INTERFACE_SIZE,
		.bDescriptorType = USB_DT_INTERFACE,
		.bInterfaceNumber = 0,
		.bAlternateSetting = 0,
		.bNumEndpoints = 2,
		.bInterfaceClass = 0xff,
		.bInterfaceSubClass = 0x00,
		.bInterfaceProtocol = 0x00,
		.iInterface = 0x00,
	},
	.epIN = {
		.bLength = USB_DT_ENDPOINT_SIZE,
		.bDescriptorType = USB_DT_ENDPOINT,
		.bEndpointAddress = UDC_EP_IN | USB_DIR_IN,
		.bmAttributes = USB_ENDPOINT_XFER_BULK,
		.wMaxPacketSize = UDC_MAX_PACKET,
		.bInterval = 0,
	},
	.epOUT = {
		.bLength = USB_DT_ENDPOINT_SIZE,
		.bDescriptorType = USB_DT_ENDPOINT,
		.bEndpointAddress = UDC_EP_OUT | USB_DIR_OUT,
		.bmAttributes = USB_ENDPOINT_XFER_BULK,
		.wMaxPacketSize = UDC_MAX_PACKET,
		.bInterval = 0,
	}
};

static void print_crq(struct usb_ctrlrequest *crq)
{
	printf("ep0 setup: bRequestType=%x, bRequrest=%x, wValue=%x"
				", wIndex=%x, wLength=%x\n", crq->bRequestType,
				crq->bRequest, crq->wValue,
				crq->wIndex, crq->wLength);

	return ;
}

static int udc_init(void)
{
	uint32_t tmp;
	uint64_t t;

	/* config epll for usb */
	tmp = readl(DIV_CFG2);
	tmp &= ~0x7f0000;
	tmp |=  0x260000;
	writel(tmp, DIV_CFG2);

	/* set usb gate */
	writel(readl(SCLK_MASK) & ~0x2030, SCLK_MASK);

	/* set usb power */
	tmp = readl(PAD_CFG);
	tmp &= ~0xe;
	writel(tmp, PAD_CFG);
	writel(0x0, USB_SRST);

	udelay(100);

	tmp |= 0xe;
	writel(tmp,PAD_CFG);

	udelay(4000);
	writel(0x5,USB_SRST);

	udelay(200);
	writel(0xf,USB_SRST);

	/* enable slave state */
	writel(readl(OTGL_BCWR) | (1 << 11), OTGL_BCWR);

	/* wait udc enter slave state */
	t = get_ticks();
	while(!(readl(OTGL_BCSR) & (1 << 4))) {
		if(get_ticks() > t + 200000) {
		  printf("udc enter slave state timeout.\n");
		  return -1;
		}
		udelay(10);
	}

	printf("udc enter slave state.\n");
	/* udc connect */
	writel(readl(OTGL_BCWR) & ~0x800, OTGL_BCWR);

	writel(0x000, OTGS_TBCR0);
	writel(0x010, OTGS_TBCR1);
	writel(0x110, OTGS_TBCR2);
	writel(0x190, OTGS_TBCR3);

	writel(0, OTGS_ACR);
	writel(0, OTGS_FNCR);
	writel(0x030f381f, OTGS_IER);
	writel(readl(OTGS_UDCR) | 0x70001, OTGS_UDCR);

	return 0;
}

static int __udc_dma_wait(void) {
	uint32_t acr;
	for(;;)
	{
		acr = readl(OTGS_ACR);
		if(acr &  0x800) {
			printf("error occured during udc dma transfer.\n");
			writel(acr, OTGS_ACR);
			writel(0x10000, OTGS_FHHR);
			writel(0x1, OTGS_BFCR);

			return -1;
		}
		else if(!(acr & 0x830))
		  break;
	}

	return 0;
}

static int udc_tx(void)
{
	int len, maxpacket, ret;

	if(!udc.tx_en) {
		printf("tx operation is not enabled.\n");
		return -1;
	}

	maxpacket = udc.tx_ep? UDC_MAX_PACKET: UDC_MAX_PACKET_EP0;
	len = min(maxpacket, udc.tx_len);

//	clean_cache(udc.tx_addr, udc.tx_addr + len);
	writel(udc.tx_addr, OTGS_MDAR);
	writel(0x22 | (udc.tx_ep << 8) | (len << 16), OTGS_ACR);

#if 0
	printf("launched dma, ep=%d, addr=0x%08x, len=0x%x\n", udc.tx_ep, udc.tx_addr, len);
	print_buffer(udc.tx_addr, udc.tx_addr, 4, len / 4, 1);
#endif

	/* wait dma to finish */
	ret =  __udc_dma_wait();
	if(ret < 0)
	  return ret;

	/* wait until no package left in tx buffer
	 * but this is not needed for ep0 
	 */
	if(udc.tx_ep)
	  while(readl(OTGS_TBCR1) & 0x7000);

	udc.tx_addr += len;
	udc.tx_len  -= len;
	if(!udc.tx_len)
	  udc.tx_en = 0;

	return ret;
}

static int udc_rx(void)
{
	int len, ret;

	if(!udc.rx_en) {
		printf("rx operation is not enabled.\n");
		return -1;
	}

	len = min(UDC_MAX_PACKET, udc.rx_len);

	writel(udc.rx_addr, OTGS_MDAR);
	writel(0x12 | (len << 16), OTGS_ACR);

	/* wait dma to finish */
	ret =  __udc_dma_wait();
	if(ret < 0)
	  return ret;

//	inv_cache(udc.rx_addr, udc.rx_addr + udc.rx_len);
	udc.rx_addr += len;
	udc.rx_len  -= len;
	if(!udc.rx_len)
	  udc.rx_en = 0;

	return ret;
}

static void ep0_setup(void)
{
	void * buf = NULL;

	if(udc.crq.bRequest != 0x6)
	  return ;

	buf = malloc(UDC_MAX_PACKET_EP0);
	if(!buf) {
		printf("alloc transfer buffer failed.\n");
		return ;
	}

	/* get descriptor */
	switch(udc.crq.wValue) {
		case 0x100:
			memcpy(buf, &device_descriptor, USB_DT_DEVICE_SIZE);
			udc.tx_len  = USB_DT_DEVICE_SIZE;
			break;
		case 0x200:
			memcpy(buf, &settings_descriptor, sizeof(settings_descriptor));
			udc.tx_len  = sizeof(settings_descriptor);
			if(udc.crq.wLength > 0x19) {
				printf("udc: interact ok with PC.\n");
				udc.link_ok = 1;
			}
			break;
		case 0x300:
		case 0x301:
		case 0x302:
		default:
			printf("unsupported type: \n");
			writel(0x10000, OTGS_FHHR);
			writel(0x00001, OTGS_BFCR);
			print_crq(&udc.crq);
			goto __exit__;
	}

	udc.tx_ep   = 0;
	udc.tx_en   = 1;
	udc.tx_addr = (uint32_t) buf;
	udc.tx_len  = min(udc.crq.wLength, udc.tx_len);
	udc_tx();

__exit__:
	if(buf)
	  free(buf);

	return ;
}

static void udc_event_handle(void)
{
	uint32_t isr = readl(OTGS_ISR) & readl(OTGS_IER);

	printf("ier:%x, idr:%x, isr:%x\n",
				readl(OTGS_IER), readl(OTGS_IDR), readl(OTGS_ISR));

	/* do not handle irq that is not enabled. */
	if(!isr){
		printf("No udc irq\n");
	  return ;
	}

	/* reset event */
	else if(isr & 0x4) {
		/* config endpoints, this also clears halt bit */
		writel(UDC_MAX_PACKET_EP0 << 16, OTGS_EDR0);
		writel(0x10000019 | (UDC_MAX_PACKET << 16), OTGS_EDR1);
		writel(0x00000021 | (UDC_MAX_PACKET << 16), OTGS_EDR2);

		writel(0x4, OTGS_ISR);
		printf("udc: reseted.\n");
	}

	/* ep0 setup */
	else if(isr & 0x10000) {
		*(((uint32_t *)&udc.crq) + 0) = readl(OTGS_STR0);
		*(((uint32_t *)&udc.crq) + 1) = readl(OTGS_STR1);

		print_crq(&udc.crq);
		writel(0x10000, OTGS_ISR);
	}

	/* ep0 IN transfer */
	else if(isr & 0x40000) {
		/* TODO */
		ep0_setup();
		writel(0x40000, OTGS_ISR);
	}

	/* epX OUT transfer */
	else if(isr & 0x2000000) {
		udc_rx();
		writel(0x2000000, OTGS_ISR);
	}

	/* epX IN transfer */
	else if(isr & 0x1000000) {
		udc_tx();
		writel(0x1000000, OTGS_ISR);
	}
	else
	  writel(isr, OTGS_ISR);

#if 0
	if(isr)
	  printf("leave isr undeal with 0x%x\n", isr);
#endif
}



/* external APIs */

/* Connect. */
int udc_connect(void)
{
	int ret;
	uint64_t t;
ooooooooooo
	printf("%s\n", __func__);
	ret = udc_init();
	if(ret < 0) {
		printf("initialize udc failed. ret=%d\n", ret);
		return ret;
	}

	printf("Here try to connect otg\n");
	t = get_ticks();
	/* poll udc event handle until configuration set */
	for(;;) {
		udc_event_handle();
		if(udc.link_ok)
		  break;
		if(get_ticks() > t + 200000) {
			printf("interact with PC failed.\n");
			return -2;
		}
	}

	return 0;
}

/* Read. */
int udc_read(uint8_t *buf, int len)
{
//	printf("%s: %x@%x\n", __func__, len, (uint32_t)buf);
	if(udc.tx_en || udc.rx_en) {
		printf("can not receive data while transfering.\n");
		return -1;
	}

	udc.rx_len  = len;
	udc.rx_addr = (uint32_t) buf;
	udc.rx_en   = 1;

	while(udc.rx_en)
	  udc_event_handle();

	return len;
}

/* Write. */
int udc_write(uint8_t *buf, int len)
{
//	printf("%s: %x@%x\n", __func__, len, (uint32_t)buf);
	if(udc.tx_en || udc.rx_en) {
		printf("can not send data while transfering.\n");
		return -1;
	}

	udc.tx_ep   = 1;
	udc.tx_len  = len;
	udc.tx_addr = (uint32_t) buf;
	udc.tx_en   = 1;

	while(udc.tx_en)
	  udc_event_handle();

	return len;
}

