/***************************************************************************** 
** infotm/drivers/gpio/gpio_imapx800.c 
** 
** Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved. 
** 
** Use of Infotm's code is governed by terms and conditions 
** stated in the accompanying licensing statement. 
** 
** Description: File used for the standard function interface of output.
**
** Author:
**     Lei Zhang   <jack.zhang@infotmic.com.cn>
**      
** Revision History: 
** ----------------- 
** 1.0  11/26/2012  Jcak Zhang   only can be use in CPU x15
*****************************************************************************/ 

#include <common.h>
#include <asm/io.h>
#include <lowlevel_api.h>
#include <efuse.h>
#include <preloader.h>
#include <items.h>
#include "imapx_gpio.h"

void gpio_reset(void)
{
	unsigned int val;

	writel(0xff, GPIO_SYSM_ADDR);
	writel(0xff, GPIO_SYSM_ADDR+0x4);
	writel(0x11, GPIO_SYSM_ADDR+0x8);

	while(1)
	{
		val = readl(GPIO_SYSM_ADDR + 0x08);
		if(val & 0x2)
			break;
	}

	val = readl(GPIO_SYSM_ADDR + 0x08);
	val &= ~0x10;
	writel(val, GPIO_SYSM_ADDR + 0x08);
	writel(0xff, GPIO_SYSM_ADDR + 0x18);
	writel(0, GPIO_SYSM_ADDR);
}

/* 0-intput,  1-output*/
void gpio_dir_set(int dir, int pin)
{
	if(item_exist("board.cpu") == 0 ||
			item_equal("board.cpu", "x820", 0)){
		int index, num, tmp;
		index = pin / 8;
		num = pin % 8;

		if(dir){
			tmp = readl(PAD_SYSM_ADDR+0xB4+4*index);
			tmp &= ~(1<<num);
			writel(tmp, PAD_SYSM_ADDR+0xB4+4*index);
		}else{
			tmp = readl(PAD_SYSM_ADDR+0xB4+4*index);
			tmp |= (1<<num);
			writel(tmp, PAD_SYSM_ADDR+0xB4+4*index);
		}

	}else if(item_equal("board.cpu", "x15", 0) 
			|| item_equal("board.cpu", "x9", 0)
			|| item_equal("board.cpu", "apollo2", 0)){

		if(dir)
		{
			writel(1, GPIO_BASE_ADDR+0x40*pin+8);/*dir output*/
		}
		else
		{
			writel(0, GPIO_BASE_ADDR+0x40*pin+8);/*dir input*/
		}
		//printf("dir reg=0x%x, val=0x%x\n", GPIO_BASE_ADDR+0x40*pin+8, readl(GPIO_BASE_ADDR+0x40*pin+8));
	 }
}

void gpio_mode_set(int mode, int pin)
{
	int index, num, val, off;
	index = pin / 8;
	num = pin % 8;

	if (item_equal("board.cpu", "apollo2", 0))
		off = 0x4;
	else
		off = 0x64;
	
	if(mode == 0)
	{
		val = readl(PAD_SYSM_ADDR+off+4*index);
		val &= ~(1<<num);
		writel(val, PAD_SYSM_ADDR+off+4*index);/*func mode*/

	}
	else
	{
		val = readl(PAD_SYSM_ADDR+off+4*index);
		val |= 1<<num;
		writel(val, PAD_SYSM_ADDR+off+4*index);/*gpio mode*/
	}
	//printf("mode reg=0x%x, bit=%d, val=0x%x\n", PAD_SYSM_ADDR+off+4*index, num, readl(PAD_SYSM_ADDR+off+4*index));
}

void gpio_output_set(int val, int pin)
{
	if(item_exist("board.cpu") == 0 ||
			item_equal("board.cpu", "x820", 0)){
		int index, num, tmp;
		index = pin/8;
		num = pin%8;

		if(val){
			tmp = readl(PAD_SYSM_ADDR+0x104+4*index);
			val |= (1<<num);
			writel(tmp, PAD_SYSM_ADDR+0x104+4*index);
		}else{
			tmp = readl(PAD_SYSM_ADDR+0x104+4*index);
			val &= ~(1<<num);
			writel(tmp, PAD_SYSM_ADDR+0x104+4*index);
		}
	}else if(item_equal("board.cpu", "x15", 0) 
			|| item_equal("board.cpu", "x9", 0)
			|| item_equal("board.cpu", "apollo2", 0)){

		if(val)
		{
			writel(1, GPIO_BASE_ADDR+0x40*pin+4);/*output 1*/
		}
		else{
			writel(0, GPIO_BASE_ADDR+0x40*pin+4);/*output 0*/
		}
		//printf("output reg=0x%x, val=0x%x\n", GPIO_BASE_ADDR+0x40*pin+4, readl(GPIO_BASE_ADDR+0x40*pin+4));
	}
}

void gpio_pull_en(int en, int pin)
{
	if (item_exist("board.cpu") == 0
			|| item_equal("board.cpu", "x820", 0)
			|| item_equal("board.cpu", "x15", 0)
			|| item_equal("board.cpu", "x9", 0)) {
		int index, num, val;

		index = pin/8;
		num = pin%8;

		if(en == 0)
		{
			val = readl(PAD_SYSM_ADDR + 0x14 + 4*index);
			val &= ~(1 << num);
			writel(val, PAD_SYSM_ADDR+0x14+4*index);
		}
		else
		{
			val = readl(PAD_SYSM_ADDR + 0x14 + 4*index);
			val |= 1 << num;
			writel(val, PAD_SYSM_ADDR+0x14+4*index);
		}
	} else if (item_equal("board.cpu", "apollo2", 0)) {
		if (en) {
			writel(1, GPIO_BASE_ADDR + 0xC + 0x40*pin);
		} else {
			writel(0, GPIO_BASE_ADDR + 0xC + 0x40*pin);
		}
		//printf("pullen reg=0x%x, val=0x%x\n", GPIO_BASE_ADDR + 0xC + 0x40*pin, readl(GPIO_BASE_ADDR + 0xC + 0x40*pin));
	}
}

void gpio_status(int pin, int *mode, int * dir, int *val, int *pull)
{
	int index, num;
	index = pin / 8;
	num = pin % 8;

	if (item_equal("board.cpu", "apollo2", 0)) {
		*mode = readl(PAD_SYSM_ADDR+0x4+4*index);
		*mode = ((*mode) & (1<<num)) ? 1 : 0;
		*dir = readl(GPIO_BASE_ADDR + 0x40*pin + 0x8);
		if (*dir == 0)
			*val = readl(GPIO_BASE_ADDR + 0x40*pin);
		else
			*val = readl(GPIO_BASE_ADDR + 0x40*pin + 0x4);
		*pull = readl(GPIO_BASE_ADDR + 0x40*pin + 0xC);
		printf("gpio-%d: mode=%s, dir=%s, val=%d, pull=%s\n",
				pin, *mode?"gpio":"func", *dir?"out":"in", *val, *pull?"enable":"disable");
	}
}

void gpio_help(void)
{
	printf("|\t\t<Usage>\t\t\n");
	printf("|\n");
	printf("| gpio func xxx\n");
	printf("| gpio input xxx\n");
	printf("| gpio high xxx\n");
	printf("| gpio low xxx\n");
	printf("| gpio pull xxx enable/disable\n");
	printf("| gpio stat xxx\n");
	printf("|\n");
	printf("| Note: xxx is gpio index.");
	printf("\n");
}
