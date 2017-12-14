/*
 * ILI9806s LCD Module  driver
 * LCD Driver IC : ota5812a-c2.
 *
 * Copyright (c) 2015 
 * Author: shuyu.li  <394056202@qq.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 */

#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/kernel.h>
#include <linux/lcd.h>
#include <linux/module.h>
#include <mach/items.h>
#include <linux/spi/spi.h>
#include <linux/wait.h>
#include <dss_common.h>
#include <mach/pad.h>
#include <linux/gpio.h>

#define LCD_SPI_CS    167   
#define SPI_CLK       94
#define SPI_TXD       93      

struct ili9806s *ili9806s_spidev;
struct ili9806s {
	struct device			*dev;
	struct spi_device		*spi;
	unsigned int			power;

	struct lcd_platform_data	*lcd_platdata;

	struct mutex			lock;
	bool  enabled;
};

static int rgb_bpp = 32;

static int ili9806s_spi_write_command(unsigned char command)
{
	int i;
	gpio_set_value(LCD_SPI_CS,  0);   //cs low
	udelay(8);
	gpio_set_value(SPI_TXD,  0);      //command out 0 first 
	gpio_set_value(SPI_CLK,  0);
	udelay(8);
	gpio_set_value(SPI_CLK,  1);
	udelay(8);
	for(i=0; i<8; i++){ 
		if( (command & (1<< (7-i)) ) ){
			gpio_set_value(SPI_TXD,  1);
		}else{
			gpio_set_value(SPI_TXD,  0);
		}
	   	gpio_set_value(SPI_CLK,  0);
	   	udelay(8);
       	gpio_set_value(SPI_CLK,  1);
		udelay(8);
   }

   gpio_set_value(LCD_SPI_CS,  1);    //cs high
   return 0;
}

static int ili9806s_spi_write_data(unsigned char data)
{
	int i;
   	gpio_set_value(LCD_SPI_CS,  0);   //cs low
    udelay(8);
   	gpio_set_value(SPI_TXD,  1);      //data out 1 first
   	gpio_set_value(SPI_CLK,  0);
    udelay(8);
   	gpio_set_value(SPI_CLK,  1);
   	udelay(8);
   	for(i=0;i<8;i++){ 
		if( (data & (1<< (7-i)) ) ){
          	gpio_set_value(SPI_TXD,  1);
        }else{
		  	gpio_set_value(SPI_TXD,  0);
		}
	   	gpio_set_value(SPI_CLK,  0);
	   	udelay(8);
       	gpio_set_value(SPI_CLK,  1);
	   	udelay(8);
   }

   gpio_set_value(LCD_SPI_CS,  1);   //cs high
   return 0;
}

int ili9806s_init_by_gpio()
{
	int ret = 0;
	int is_9806e = 0;
	imapx_pad_set_mode(LCD_SPI_CS, "output");
	imapx_pad_set_mode(SPI_CLK, "output");       //set spi gpio mode
	imapx_pad_set_mode(SPI_TXD, "output");	     //set spi gpio mode
	
	ret = gpio_request(LCD_SPI_CS, "lcd_spi_cs");
	if(ret){
		printk(KERN_ERR "gpio request lcd_spi_cs failed\n");
	}else{
		gpio_set_value(LCD_SPI_CS,  1);
	}
	
	ret = gpio_request(SPI_CLK, "lcd_spi_clk");
	if(ret){
		printk(KERN_ERR "gpio request lcd_spi_clk failed\n");
	}else{
		gpio_set_value(SPI_CLK,  0);
	}
	
	ret = gpio_request(SPI_TXD, "lcd_spi_txd");
	if(ret){
		printk(KERN_ERR "gpio request lcd_spi_txd failed\n");
	}else{
		gpio_set_value(SPI_TXD,  0);
	}
	if (item_exist("lcd.model")) {
		is_9806e = item_integer("lcd.model", 0);
	}

	if (is_9806e == 0) {
		udelay(100);
		ili9806s_spi_write_command(0xFF);
		ili9806s_spi_write_data(0xFF);
		ili9806s_spi_write_data(0x98);
		ili9806s_spi_write_data(0x06);

		ili9806s_spi_write_command(0xBA);
		ili9806s_spi_write_data(0x60);

		// GIP1
		ili9806s_spi_write_command(0xBC);
		ili9806s_spi_write_data(0x01);
		ili9806s_spi_write_data(0x12);
		ili9806s_spi_write_data(0x61);
		ili9806s_spi_write_data(0xFF);
		ili9806s_spi_write_data(0x10);
		ili9806s_spi_write_data(0x10);
		ili9806s_spi_write_data(0x0B);
		ili9806s_spi_write_data(0x13);
		ili9806s_spi_write_data(0x32);
		ili9806s_spi_write_data(0x73);
		ili9806s_spi_write_data(0xFF);
		ili9806s_spi_write_data(0xFF);
		ili9806s_spi_write_data(0x0E);
		ili9806s_spi_write_data(0x0E);
		ili9806s_spi_write_data(0x00);
		ili9806s_spi_write_data(0x03);
		ili9806s_spi_write_data(0x66);
		ili9806s_spi_write_data(0x63);
		ili9806s_spi_write_data(0x01);
		ili9806s_spi_write_data(0x00);
		ili9806s_spi_write_data(0x00);

		// GIP2
		ili9806s_spi_write_command(0xBD);
		ili9806s_spi_write_data(0x01);
		ili9806s_spi_write_data(0x23);
		ili9806s_spi_write_data(0x45);
		ili9806s_spi_write_data(0x67);
		ili9806s_spi_write_data(0x01);
		ili9806s_spi_write_data(0x23);
		ili9806s_spi_write_data(0x45);
		ili9806s_spi_write_data(0x67);

		// GIP3
		ili9806s_spi_write_command(0xBE);
		ili9806s_spi_write_data(0x00);
		ili9806s_spi_write_data(0x21);
		ili9806s_spi_write_data(0xAB);
		ili9806s_spi_write_data(0x60);
		ili9806s_spi_write_data(0x22);
		ili9806s_spi_write_data(0x22);
		ili9806s_spi_write_data(0x22);
		ili9806s_spi_write_data(0x22);
		ili9806s_spi_write_data(0x22);
		udelay(10);

		//******** Initializing Seqvence 2 ************
		ili9806s_spi_write_command(0xED);
		ili9806s_spi_write_data(0x7F);
		ili9806s_spi_write_data(0x0F);
		ili9806s_spi_write_data(0x00);


		ili9806s_spi_write_command(0xB6);
		ili9806s_spi_write_data(0x22);

		ili9806s_spi_write_command(0xB0);
		ili9806s_spi_write_data(0x0F);

#if 0
		ili9806s_spi_write_command(0x2A);
		ili9806s_spi_write_data(0x00);
		ili9806s_spi_write_data(0x00);
		ili9806s_spi_write_data(0x03);
		ili9806s_spi_write_data(0x1F);

		ili9806s_spi_write_command(0x2B);
		ili9806s_spi_write_data(0x00);
		ili9806s_spi_write_data(0x00);
		ili9806s_spi_write_data(0x01);
		ili9806s_spi_write_data(0xDF);
#else
		ili9806s_spi_write_command(0x2A);
		ili9806s_spi_write_data(0x00);
		ili9806s_spi_write_data(0x00);
		ili9806s_spi_write_data(0x01);
		ili9806s_spi_write_data(0xDF);

		ili9806s_spi_write_command(0x2B);
		ili9806s_spi_write_data(0x00);
		ili9806s_spi_write_data(0x00);
		ili9806s_spi_write_data(0x03);
		ili9806s_spi_write_data(0x1F);
#endif	

		ili9806s_spi_write_command(0x3A);
		ili9806s_spi_write_data(0x55);

		ili9806s_spi_write_command(0x36);
#if 0	
		ili9806s_spi_write_data(0x21); 
#else
		ili9806s_spi_write_data(0x00);
#endif	

		ili9806s_spi_write_command(0xB5);
		ili9806s_spi_write_data(0x3e); //3e
		ili9806s_spi_write_data(0x18);


		ili9806s_spi_write_command(0xC0);
		ili9806s_spi_write_data(0xAB);
		ili9806s_spi_write_data(0x0B);
		ili9806s_spi_write_data(0x0A);


		ili9806s_spi_write_command(0xFC);
		ili9806s_spi_write_data(0x09);


		ili9806s_spi_write_command(0xDF);
		ili9806s_spi_write_data(0x00);
		ili9806s_spi_write_data(0x00);
		ili9806s_spi_write_data(0x00);
		ili9806s_spi_write_data(0x00);
		ili9806s_spi_write_data(0x00);
		ili9806s_spi_write_data(0x20);


		ili9806s_spi_write_command(0xF3);
		ili9806s_spi_write_data(0x74);


		ili9806s_spi_write_command(0xB4);
		ili9806s_spi_write_data(0x00);
		ili9806s_spi_write_data(0x00);
		ili9806s_spi_write_data(0x00);


		ili9806s_spi_write_command(0xF7);
		ili9806s_spi_write_data(0x82);    //480*800


		ili9806s_spi_write_command(0xB1);
		ili9806s_spi_write_data(0x00);
		ili9806s_spi_write_data(0x12);
		ili9806s_spi_write_data(0x12);


		ili9806s_spi_write_command(0xF2);
		ili9806s_spi_write_data(0x00);
		ili9806s_spi_write_data(0x59);
		ili9806s_spi_write_data(0x40);
		ili9806s_spi_write_data(0x28);


		ili9806s_spi_write_command(0xC1);
		ili9806s_spi_write_data(0x07);
		ili9806s_spi_write_data(0x61);
		ili9806s_spi_write_data(0x61);
		ili9806s_spi_write_data(0x20);

		ili9806s_spi_write_command(0xEA);
		ili9806s_spi_write_data(0x03);


		ili9806s_spi_write_command(0x35);
		ili9806s_spi_write_data(0x00);	

		//------ GAMMA SETTING --------	
		ili9806s_spi_write_command(0xE0);
		ili9806s_spi_write_data(0x04);
		ili9806s_spi_write_data(0x03);
		ili9806s_spi_write_data(0x09);
		ili9806s_spi_write_data(0x08);
		ili9806s_spi_write_data(0x01);
		ili9806s_spi_write_data(0x05);
		ili9806s_spi_write_data(0x05);
		ili9806s_spi_write_data(0x03);
		ili9806s_spi_write_data(0x09);
		ili9806s_spi_write_data(0x0C);
		ili9806s_spi_write_data(0x12);
		ili9806s_spi_write_data(0x04);
		ili9806s_spi_write_data(0x07);
		ili9806s_spi_write_data(0x1C);
		ili9806s_spi_write_data(0x15);
		ili9806s_spi_write_data(0x00);  //GAMMA 1

		ili9806s_spi_write_command(0xE1);
		ili9806s_spi_write_data(0x00);
		ili9806s_spi_write_data(0x03);
		ili9806s_spi_write_data(0x08);
		ili9806s_spi_write_data(0x08);
		ili9806s_spi_write_data(0x01);
		ili9806s_spi_write_data(0x04);
		ili9806s_spi_write_data(0x06);
		ili9806s_spi_write_data(0x04);
		ili9806s_spi_write_data(0x0A);
		ili9806s_spi_write_data(0x0D);
		ili9806s_spi_write_data(0x13);
		ili9806s_spi_write_data(0x05);
		ili9806s_spi_write_data(0x07);
		ili9806s_spi_write_data(0x1C);
		ili9806s_spi_write_data(0x13);
		ili9806s_spi_write_data(0x00);  //GAMMA 2	

		ili9806s_spi_write_command(0x11);
		udelay(10);

		ili9806s_spi_write_command(0x29);
		udelay(10);
	} else {
		udelay(300);
		ili9806s_spi_write_command(0xFF);
		ili9806s_spi_write_data(0xFF);
		ili9806s_spi_write_data(0x98);
		ili9806s_spi_write_data(0x06);
		ili9806s_spi_write_data(0x04);
		ili9806s_spi_write_data(0x01);

		ili9806s_spi_write_command(0x08);
		ili9806s_spi_write_data(0x10);

		ili9806s_spi_write_command(0x20);
		ili9806s_spi_write_data(0x01);

		ili9806s_spi_write_command(0x21);
		ili9806s_spi_write_data(0x03);
		ili9806s_spi_write_command(0x30);
		ili9806s_spi_write_data(0x02);
		ili9806s_spi_write_command(0x31);
		ili9806s_spi_write_data(0x02);
		ili9806s_spi_write_command(0x40);
		ili9806s_spi_write_data(0x14);
		ili9806s_spi_write_command(0x41);
		ili9806s_spi_write_data(0x33);
		ili9806s_spi_write_command(0x42);
		ili9806s_spi_write_data(0x01);
		ili9806s_spi_write_command(0x43);
		ili9806s_spi_write_data(0x03);
		ili9806s_spi_write_command(0x44);
		ili9806s_spi_write_data(0x09);
		ili9806s_spi_write_command(0x50);
		ili9806s_spi_write_data(0x60);
		ili9806s_spi_write_command(0x51);
		ili9806s_spi_write_data(0x60);
		ili9806s_spi_write_command(0x52);
		ili9806s_spi_write_data(0x00);
		ili9806s_spi_write_command(0x53);
		ili9806s_spi_write_data(0x53);
		ili9806s_spi_write_command(0x60);
		ili9806s_spi_write_data(0x07);
		ili9806s_spi_write_command(0x61);
		ili9806s_spi_write_data(0x00);
		ili9806s_spi_write_command(0x62);
		ili9806s_spi_write_data(0x09);
		ili9806s_spi_write_command(0x63);
		ili9806s_spi_write_data(0x00);
		ili9806s_spi_write_command(0xA0);
		ili9806s_spi_write_data(0x00);
		ili9806s_spi_write_command(0xA1);
		ili9806s_spi_write_data(0x0C);
		ili9806s_spi_write_command(0xA2);
		ili9806s_spi_write_data(0x13);
		ili9806s_spi_write_command(0xA3);
		ili9806s_spi_write_data(0x0C);
		ili9806s_spi_write_command(0xA4);
		ili9806s_spi_write_data(0x04);
		ili9806s_spi_write_command(0xA5);
		ili9806s_spi_write_data(0x09);
		ili9806s_spi_write_command(0xA6);
		ili9806s_spi_write_data(0x07);
		ili9806s_spi_write_command(0xA7);
		ili9806s_spi_write_data(0x06);
		ili9806s_spi_write_command(0xA8);
		ili9806s_spi_write_data(0x06);
		ili9806s_spi_write_command(0xA9);
		ili9806s_spi_write_data(0x0A);
		ili9806s_spi_write_command(0xAA);
		ili9806s_spi_write_data(0x11);
		ili9806s_spi_write_command(0xAB);
		ili9806s_spi_write_data(0x08);
		ili9806s_spi_write_command(0xAC);
		ili9806s_spi_write_data(0x0C);
		ili9806s_spi_write_command(0xAD);
		ili9806s_spi_write_data(0x1B);
		ili9806s_spi_write_command(0xAE);
		ili9806s_spi_write_data(0x0D);
		ili9806s_spi_write_command(0xAF);
		ili9806s_spi_write_data(0x00);
		ili9806s_spi_write_command(0xC0);
		ili9806s_spi_write_data(0x00);
		ili9806s_spi_write_command(0xC1);
		ili9806s_spi_write_data(0x0C);
		ili9806s_spi_write_command(0xC2);
		ili9806s_spi_write_data(0x13);
		ili9806s_spi_write_command(0xC3);
		ili9806s_spi_write_data(0x0C);
		ili9806s_spi_write_command(0xC4);
		ili9806s_spi_write_data(0x04);
		ili9806s_spi_write_command(0xC5);
		ili9806s_spi_write_data(0x09);
		ili9806s_spi_write_command(0xC6);
		ili9806s_spi_write_data(0x07);
		ili9806s_spi_write_command(0xC7);
		ili9806s_spi_write_data(0x06);
		ili9806s_spi_write_command(0xC8);
		ili9806s_spi_write_data(0x06);
		ili9806s_spi_write_command(0xC9);
		ili9806s_spi_write_data(0x0A);
		ili9806s_spi_write_command(0xCA);
		ili9806s_spi_write_data(0x11);
		ili9806s_spi_write_command(0xCB);
		ili9806s_spi_write_data(0x08);
		ili9806s_spi_write_command(0xCC);
		ili9806s_spi_write_data(0x0C);
		ili9806s_spi_write_command(0xCD);
		ili9806s_spi_write_data(0x1B);
		ili9806s_spi_write_command(0xCE);
		ili9806s_spi_write_data(0x0D);
		ili9806s_spi_write_command(0xCF);
		ili9806s_spi_write_data(0x00);

		ili9806s_spi_write_command(0xFF);
		ili9806s_spi_write_data(0xFF);
		ili9806s_spi_write_data(0x98);
		ili9806s_spi_write_data(0x06);
		ili9806s_spi_write_data(0x04);
		ili9806s_spi_write_data(0x06);

		ili9806s_spi_write_command(0x00);
		ili9806s_spi_write_data(0xA0);
		ili9806s_spi_write_command(0x01);
		ili9806s_spi_write_data(0x05);
		ili9806s_spi_write_command(0x02);
		ili9806s_spi_write_data(0x00);
		ili9806s_spi_write_command(0x03);
		ili9806s_spi_write_data(0x00);
		ili9806s_spi_write_command(0x04);
		ili9806s_spi_write_data(0x01);
		ili9806s_spi_write_command(0x05);
		ili9806s_spi_write_data(0x01);
		ili9806s_spi_write_command(0x06);
		ili9806s_spi_write_data(0x88);
		ili9806s_spi_write_command(0x07);
		ili9806s_spi_write_data(0x04);
		ili9806s_spi_write_command(0x08);
		ili9806s_spi_write_data(0x01);
		ili9806s_spi_write_command(0x09);
		ili9806s_spi_write_data(0x90);
		ili9806s_spi_write_command(0x0A);
		ili9806s_spi_write_data(0x04);
		ili9806s_spi_write_command(0x0B);
		ili9806s_spi_write_data(0x01);
		ili9806s_spi_write_command(0x0C);
		ili9806s_spi_write_data(0x01);
		ili9806s_spi_write_command(0x0D);
		ili9806s_spi_write_data(0x01);
		ili9806s_spi_write_command(0x0E);
		ili9806s_spi_write_data(0x00);
		ili9806s_spi_write_command(0x0F);
		ili9806s_spi_write_data(0x00);
		ili9806s_spi_write_command(0x10);
		ili9806s_spi_write_data(0x55);
		ili9806s_spi_write_command(0x11);
		ili9806s_spi_write_data(0x50);
		ili9806s_spi_write_command(0x12);
		ili9806s_spi_write_data(0x01);
		ili9806s_spi_write_command(0x13);
		ili9806s_spi_write_data(0x85);
		ili9806s_spi_write_command(0x14);
		ili9806s_spi_write_data(0x85);
		ili9806s_spi_write_command(0x15);
		ili9806s_spi_write_data(0xC0);
		ili9806s_spi_write_command(0x16);
		ili9806s_spi_write_data(0x0B);
		ili9806s_spi_write_command(0x17);
		ili9806s_spi_write_data(0x00);
		ili9806s_spi_write_command(0x18);
		ili9806s_spi_write_data(0x00);
		ili9806s_spi_write_command(0x19);
		ili9806s_spi_write_data(0x00);
		ili9806s_spi_write_command(0x1A);
		ili9806s_spi_write_data(0x00);
		ili9806s_spi_write_command(0x1B);
		ili9806s_spi_write_data(0x00);
		ili9806s_spi_write_command(0x1C);
		ili9806s_spi_write_data(0x00);
		ili9806s_spi_write_command(0x1D);
		ili9806s_spi_write_data(0x00);
		ili9806s_spi_write_command(0x20);
		ili9806s_spi_write_data(0x01);
		ili9806s_spi_write_command(0x21);
		ili9806s_spi_write_data(0x23);
		ili9806s_spi_write_command(0x22);
		ili9806s_spi_write_data(0x45);
		ili9806s_spi_write_command(0x23);
		ili9806s_spi_write_data(0x67);
		ili9806s_spi_write_command(0x24);
		ili9806s_spi_write_data(0x01);
		ili9806s_spi_write_command(0x25);
		ili9806s_spi_write_data(0x23);
		ili9806s_spi_write_command(0x26);
		ili9806s_spi_write_data(0x45);
		ili9806s_spi_write_command(0x27);
		ili9806s_spi_write_data(0x67);
		ili9806s_spi_write_command(0x30);
		ili9806s_spi_write_data(0x02);
		ili9806s_spi_write_command(0x31);
		ili9806s_spi_write_data(0x22);
		ili9806s_spi_write_command(0x32);
		ili9806s_spi_write_data(0x11);
		ili9806s_spi_write_command(0x33);
		ili9806s_spi_write_data(0xAA);
		ili9806s_spi_write_command(0x34);
		ili9806s_spi_write_data(0xBB);
		ili9806s_spi_write_command(0x35);
		ili9806s_spi_write_data(0x66);
		ili9806s_spi_write_command(0x36);
		ili9806s_spi_write_data(0x00);
		ili9806s_spi_write_command(0x37);
		ili9806s_spi_write_data(0x22);
		ili9806s_spi_write_command(0x38);
		ili9806s_spi_write_data(0x22);
		ili9806s_spi_write_command(0x39);
		ili9806s_spi_write_data(0x22);
		ili9806s_spi_write_command(0x3A);
		ili9806s_spi_write_data(0x22);
		ili9806s_spi_write_command(0x3B);
		ili9806s_spi_write_data(0x22);
		ili9806s_spi_write_command(0x3C);
		ili9806s_spi_write_data(0x22);
		ili9806s_spi_write_command(0x3D);
		ili9806s_spi_write_data(0x22);
		ili9806s_spi_write_command(0x3E);
		ili9806s_spi_write_data(0x22);
		ili9806s_spi_write_command(0x3F);
		ili9806s_spi_write_data(0x22);
		ili9806s_spi_write_command(0x40);
		ili9806s_spi_write_data(0x22);
		ili9806s_spi_write_command(0x52);
		ili9806s_spi_write_data(0x10);
		ili9806s_spi_write_command(0x53);
		ili9806s_spi_write_data(0x12);

		ili9806s_spi_write_command(0xFF);
		ili9806s_spi_write_data(0xFF);
		ili9806s_spi_write_data(0x98);
		ili9806s_spi_write_data(0x06);
		ili9806s_spi_write_data(0x04);
		ili9806s_spi_write_data(0x07);

		ili9806s_spi_write_command(0x18);
		ili9806s_spi_write_data(0x1D);
		ili9806s_spi_write_command(0x17);
		ili9806s_spi_write_data(0x32);
		ili9806s_spi_write_command(0x02);
		ili9806s_spi_write_data(0x77);
		ili9806s_spi_write_command(0xE1);
		ili9806s_spi_write_data(0x79);
		ili9806s_spi_write_command(0x26);
		ili9806s_spi_write_data(0xB2);
		ili9806s_spi_write_command(0xB3);
		ili9806s_spi_write_data(0x10);

		ili9806s_spi_write_command(0xFF);
		ili9806s_spi_write_data(0xFF);
		ili9806s_spi_write_data(0x98);
		ili9806s_spi_write_data(0x06);
		ili9806s_spi_write_data(0x04);
		ili9806s_spi_write_data(0x00);
		mdelay(1);

		ili9806s_spi_write_command(0x36);
		ili9806s_spi_write_data(0x00);

		ili9806s_spi_write_command(0x3A);
		ili9806s_spi_write_data(0x55);

		ili9806s_spi_write_command(0x29);
		ili9806s_spi_write_command(0x11);
	}

	gpio_free(SPI_CLK);
	gpio_free(SPI_TXD);
	gpio_free(LCD_SPI_CS);

	imapx_pad_set_mode(SPI_CLK, "function");   //set spi mode
	imapx_pad_set_mode(SPI_TXD, "function");   //set spi mode

	return 0;
}
EXPORT_SYMBOL(ili9806s_init_by_gpio);

static int ili9806s_spi_write(struct spi_device *spidev, unsigned short data)
{
	u16 buf[1];
	struct spi_message msg;

	struct spi_transfer xfer = {
		.len		= 2,
		.tx_buf		= buf,
	};

	buf[0] = data;

	spi_message_init(&msg);
	spi_message_add_tail(&xfer, &msg);

	return spi_sync(spidev, &msg);
}


int ili9806s_init(struct ili9806s *spidev)
{
	int ret=0;
    int i=0;
    struct spi_device *spi;
	spi = spidev->spi;
   
   ili9806s_spi_write(spi, 0x055f);
   ili9806s_spi_write(spi, 0x051f);
   ili9806s_spi_write(spi, 0x055f);
   ili9806s_spi_write(spi, 0x2B01);
   ili9806s_spi_write(spi, 0x0007);
   ili9806s_spi_write(spi, 0x01ac);
   ili9806s_spi_write(spi, 0x2f69);
   if (rgb_bpp == 8)
	   ili9806s_spi_write(spi, 0x042B);
   else
	   ili9806s_spi_write(spi, 0x044B);
   ili9806s_spi_write(spi, 0x1604);
   ili9806s_spi_write(spi, 0x0340);
   ili9806s_spi_write(spi, 0x0D40);
   ili9806s_spi_write(spi, 0x0e40);
   ili9806s_spi_write(spi, 0x0f40);
   ili9806s_spi_write(spi, 0x1040);
   ili9806s_spi_write(spi, 0x1140);
   dev_info(&spi->dev, "Init LCD module(ILI9806s) .\n");
   mdelay(20);
	
   return ret;
}
EXPORT_SYMBOL(ili9806s_init);

static int ili9806s_power_on(struct ili9806s *spidev)
{
	int ret = 0;

    dev_err(spidev->dev, "lcd setting failed.\n");
	
	return 0;
}

static int ili9806s_power_off(struct ili9806s *spidev)
{
	int ret;

    dev_err(spidev->dev, "lcd setting failed.\n");

	return 0;
}


static int ili9806s_probe(struct spi_device *spi)
{
	int ret = 0;
	struct ili9806s *spidev = NULL;

	spidev = devm_kzalloc(&spi->dev, sizeof(struct ili9806s), GFP_KERNEL);
	if (!spidev)
		return -ENOMEM;

	/* ili9806s lcd panel uses 3-wire 9bits SPI Mode. */
	spi->bits_per_word = 16;

	ret = spi_setup(spi);
	if (ret < 0) {
		dev_err(&spi->dev, "spi setup failed.\n");
		return ret;
	}

	spidev->spi = spi;
	spidev->dev = &spi->dev;

	spidev->lcd_platdata = spi->dev.platform_data;
	if (!spidev->lcd_platdata) {
		dev_err(&spi->dev, "platform data is NULL.\n");
		//return -EINVAL;
	}

	/*
	 * because lcd panel was powered on automaticlly,
	 * so donnot need to power on it manually using software.
	 */


//    ili9806s_init(spidev);
    spi_set_drvdata(spi, spidev);

	ili9806s_spidev = spidev;

	if (item_exist(P_LCD_RGBBPP))
		rgb_bpp = item_integer(P_LCD_RGBBPP, 0);
	else
		rgb_bpp = 32;

	dev_info(&spi->dev, "ili9806s lcd spi driver has been probed.\n");

	return ret;
}

static int ili9806s_remove(struct spi_device *spi)
{
	struct ili9806s *spidev = spi_get_drvdata(spi);

	ili9806s_power_off(spidev);

	return 0;
}

#ifdef CONFIG_PM_SLEEP
static int ili9806s_suspend(struct device *dev)
{
	struct ili9806s *spidev = dev_get_drvdata(dev);

	dev_dbg(dev, "lcd suspend \n");

	/*
	 * when lcd panel is suspend, lcd panel becomes off
	 * regardless of status.
	 */
	return 0;
}

static int ili9806s_resume(struct device *dev)
{

	struct ili9806s *spidev = dev_get_drvdata(dev);

	dev_dbg(dev, "lcd suspend \n");

	/*
	 * when lcd panel is suspend, lcd panel becomes off
	 * regardless of status.
	 */
	return 0;
}
#endif

static SIMPLE_DEV_PM_OPS(ili9806s_pm_ops, ili9806s_suspend, ili9806s_resume);

/* Power down all displays on reboot, poweroff or halt. */
static void ili9806s_shutdown(struct spi_device *spi)
{

	struct ili9806s *spidev = spi_get_drvdata(spi);

	ili9806s_power_off(spidev);

	return 0;
}

static struct spi_driver ili9806s_driver = {
	.driver = {
		.name	= "ili9806s",
		.owner	= THIS_MODULE,
		.pm	= &ili9806s_pm_ops,
	},
	.probe		= ili9806s_probe,
	.remove		= ili9806s_remove,
	.shutdown	= ili9806s_shutdown,
};

module_spi_driver(ili9806s_driver);

MODULE_AUTHOR("shuyu.li	<394056202@qq.com>");
MODULE_DESCRIPTION("ili9806s LCD Driver");
MODULE_LICENSE("GPL");
