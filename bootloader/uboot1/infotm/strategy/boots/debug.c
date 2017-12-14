/***************************************************************************** 
** debug.c
** 
** Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved. 
** 
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** Description: iROM common debug & configuration interface
**
** Author:
**     Warits   <warits.wang@infotm.com>
**      
** Revision History: 
** ----------------- 
** 1.0  XXX 06/18/2011 XXX	Initialized by warits
*****************************************************************************/

#include <common.h>
#include <cdbg.h>
#include <isi.h>
#include <hash.h>
#include <crypto.h>
#include <nand.h>
#include <flash.h>
#include <lowlevel_api.h>
#include <bootlist.h>
#include <jtag.h>
#include <irerrno.h>
#include <led.h>

extern int dram_enabled; 
static int secure_burn = 0, bloffset = 0, bldev = -1;
static uint8_t secure_key[32];
uint8_t __irom_ver[32] = "123";

static int cdbg_check_table(struct cdbg_cfg_table *t)
{
	uint32_t crc_ori, crc_calc;
	uint8_t *data = (uint8_t *)t + sizeof(*t);

	/* check magic */
	if(t->magic != IR_CFG_TLB_MAGIC) {
		printf("magic do not match4. 0x%x\n", t->magic);
		return -1;
	}

	/* check hcrc */
	crc_ori = t->hcrc;
	t->hcrc = 0;

	hash_init(IUW_HASH_CRC, sizeof(struct cdbg_cfg_table));
	hash_data(t, sizeof(struct cdbg_cfg_table));
	hash_value(&crc_calc);

	/* restore hcrc */
	t->hcrc = crc_ori;

	if(crc_ori != crc_calc) {
		printf("hcrc do not match. ori=0x%x, calc=0x%x\n",
					crc_ori, crc_calc);
		return -2;
	}

	printf(" table header check passed.\n");

	hash_init(IUW_HASH_CRC, t->length);
	hash_data(data, t->length);
	hash_value(&crc_calc);

	if(t->dcrc != crc_calc) {
		printf("dcrc do not match. ori=0x%x, calc=0x%x\n",
					t->dcrc, crc_calc);
		return -3;
	}

	return 0;
}

#if 1
static int cdbg_config_clks(void *t)
{
//	set_clk(t);
	printf("CLKs configured.\n");
	return 0;
}
#endif

static int cdbg_config_dram(void *t)
{
	dram_enabled = 1;
	printf("DRAM configured.\n");
	return 0;
}

static int cdbg_config_nand(void * t, int count)
{
#if 0
	int ret;

	ret = nand_rescan_config(t, count);
	if(ret < 0)
	  return -1;
#endif

	printf("NAND configured.\n");
	return 0;
}

/* c: do verification, this para id mostly deprecated and should be set to 1
 */
int cdbg_do_config(void *_t, int c)
{
	int ret;
	uint8_t *data;
	struct cdbg_cfg_table *t = _t;

	if(c) {
	  ret = cdbg_check_table(t);
	  if(ret < 0)
		return ret;
	}

	if(t->spibytes)
	  spi_set_bytes(t->spibytes);

	if(t->blinfo != -1)
	  set_xom(t->blinfo);

	if(t->iomux != -1)
	  set_mux(t->iomux);

	if(t->bldevice != -1) {
		bldev = t->bldevice;
		bloffset = t->bloffset;
	}

	if(t->clksen) {
		data = (uint8_t *)t + t->clks_offset;
		cdbg_config_clks(data);
	}

	if(t->dramen) {
		data = (uint8_t *)t + t->dram_offset;
		cdbg_config_dram(data);
	}
	if(t->nanden) {
		data = (uint8_t *)t + t->nand_offset;
		cdbg_config_nand(data, t->nandcount);
	}

	return 0;
}

int cdbg_config_isi(void *data)
{
	struct isi_hdr *t = data;
	int ret;

	ret = isi_check(t);
	if(ret) {
		printf("verify config failed (%d)\n", ret);
		return ret;
	}

	if(isi_get_type_image(t) != ISI_TYPE_CFG) {
		printf("wrong config type\n");
		return -EWRONGTYPE;
	}

	/* do the configuration */
	return cdbg_do_config(isi_get_data(t), 1);
}

#if 0
void cdbg_log_toggle(int en) {
	__printf = en? printf_on: printf_off;
}
#endif

void cdbg_get_pp(void)
{
	char pp[8] = {'b', 'i', 'u', 0,
		0, 0, 0, 0}, *p;

	for(p = pp; *p;)
	  if(getc() != *p++) p = pp;
}

void cdbg_wipe_bootarea(void)
{
	uint32_t *p = (uint32_t *)IRAM_BASE_PA;
	while(p < (uint32_t *)(IRAM_BASE_PA + BL_SIZE_FIXED))
	  *p++ = 0xeafffffe;
}

#if 0
void cdbg_shell(void)
{
	/* lock ISK */
	ss_lock_isk();

	/* wipe boot area */
	cdbg_wipe_bootarea();

	/* enable JTAG */
	ss_jtag_en(1);

	/* enable output */
	cdbg_log_toggle(1);

	/* show debug shell */
	printf("->: ");

	/* check pass phrase */
	cdbg_get_pp();

	/* show debug shell */
	printf("debug shell :<-\n");
	for(;;)
	  main_loop();
}
#endif

int cdbg_dram_enabled(void) {
	return dram_enabled;
}

int cdbg_jump(void * addr)
{
	__attribute__((noreturn)) void (*jd)(void);

	if(addr_is_dram(addr)) {
		if(!dram_enabled) {
			printf("DRAM is not initialized.\n");
			return -1;
		}
	}

	jd = (void *)addr;
	led_pulse(10);
	ss_lock_isk();

	/* jump */
	printf("i: jump (0x%p)\n", addr);

	led_pulse(11);
	/* wait 3ms to make sure everything is ready */
	udelay(3000);
	led_pulse(12);
	jd();
	for(;;);
}

#if 0
void cdbg_restore_os(void)
{
	void * entry = get_wakeup_entry();

	printf("restore OS ..\n");
//	reset_core(1);
	cdbg_jump(entry);
}
#endif

void cdbg_verify_burn_enable(int en, const uint8_t *key) {
	secure_burn = !!en;

	if(en)
	  memcpy(secure_key, key, 32);
}

inline int cdbg_verify_burn(void) {
	return secure_burn;
}

uint8_t * cdbg_verify_burn_key(void) {
	return secure_key;
}

void cdbg_boot_redirect(int *id, loff_t *offs)
{
	if(bldev == -1) return ;

	*id = bldev;
	*offs = bloffset;
}

