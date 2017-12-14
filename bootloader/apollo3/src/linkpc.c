/*
 * (C) Copyright 2008 - 2009
 * Windriver, <www.windriver.com>
 * Tom Rix <Tom.Rix@windriver.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

//#include <asm/byteorder.h>
//#include <common.h>
#include <malloc.h> 
#include <asm-arm/arch-apollo3/imapx_base_reg.h>
#include <vstorage.h>
#include <asm-arm/io.h>
#include <asm-arm/types.h>
#include <command.h>
#include <udc.h>
#include <lowlevel_api.h>
#include <cdbg.h>
#include <ius.h>
#include <burn.h>
#include <ramdisk.h>
#include <linkpc.h>
#include <preloader.h>
#include <dramc_init.h>
#include <image.h>
#include <items.h>
#include <rballoc.h>
#include <rtcbits.h>
#include <asm-arm/arch-apollo3/imapx_pads.h>

#define DEBUG_FASTBOOT
#ifdef DEBUG_FASTBOOT
#define debugp(fmt,args...) irf->printf(fmt,##args)
#else
#define debugp(fmt,args...)
#endif

//#define XFER_BUF_SIZE			0xc000  //64K in max
//#define CONFIG_LINKPC_MANUAL_CHECK 1
#define CONFIG_LINKPC_MANUAL_CHECK 0

#define FASTBOOT_VERSION "iROM linkPC 1.0"

static uint32_t glpdownload, glpbytes, glperror, glpdsize;
static uint8_t *glpmem = NULL;
static uint32_t glpbufsize = 0;
static char glpstage[16] = "uboot";

enum { 
	LD_ITEM = 0,
	LD_KERNEL,
	LD_RAMDISK,
	LD_IMAGE_NUM
};

enum {
	EN_ASSIGN_MASK_LD = 1,
	EN_ASSIGN_MASK_DRAM
};
#define ASSIGN_MASK_LD (EN_ASSIGN_MASK_LD<<4)
#define ASSIGN_MASK_DRAM (EN_ASSIGN_MASK_DRAM<<4)

typedef struct {
	uint8_t *start;
	uint32_t bufsize;
}ld_info_t;

static ld_info_t ld_info[LD_IMAGE_NUM];
#if 0
static struct infotm_dram_info dram;
#endif
static uint8_t ldstat = 0;
static uint8_t ld_cur_idx = 0;
static int8_t ld_dram_stat = 0;
uint8_t *item_rrtb;
static int dram_size;

extern char * device_strings[];
extern size_t strlen(const char *s);
extern char *strcpy(char *dst, const char *src);
extern int calc_dram_M(void);
extern int board_reset(void);
extern int loop_per_jiffies;
uint8_t* item_ldbuf;

int ld_init(void) {
	item_ldbuf = irf->malloc(ITEM_SIZE_EMBEDDED);
	if(item_ldbuf == NULL)	{
		spl_printf("%s,%d:alloc item ld buff failed, 0x%p\n", item_ldbuf);
		return -1;
	}
	return 0;
}

int ld_exit(void) {
	if (item_ldbuf) {
		irf->free(item_ldbuf);
	}
	return 0;
}


int ld_try_set(uint32_t idx, uint8_t* start, uint32_t bufsize) {
	int i;
	for (i = 0; i < LD_IMAGE_NUM; i++) {
		if (i == idx) continue;
		if (ld_info[i].start > (start+bufsize) || (ld_info[i].start + ld_info[i].bufsize) < start ) continue;
		irf->printf("overlay:[%d]->start=%08x, size=%d, [%d]->start=%08x, size=%d\n", 
			i, ld_info[i].start, ld_info[i].bufsize, idx, start, bufsize);
		return 0;
	}
	ld_info[idx].start = start;
	ld_info[idx].bufsize =bufsize;
	return 1;
}

int ld_dram_ready(void) {
	return ld_dram_stat;
}

void ld_update_stat(void) {
	ldstat |= (1<<ld_cur_idx);
}

int ld_get_stat(uint32_t e) {
	return ldstat&(1<<e);
}

int ld_finish(void) {
	int i = 0;
	for (i = 0; i < LD_IMAGE_NUM; i++) {
		if (ldstat&(1<<i)) {
			continue;
		}
		return 0;
	}
	return 1;
}

int ld_need_dram(void) {
	if (ld_cur_idx==LD_ITEM) {
		return 0;
	}
	return 1;
}
	
/*
 * getvar
 * get common variables
 * board has a chance to handle other variables 
 */
static int lp_get_value(const char *para , char *response)
{
	char *p = response + 4;

	debugp("%s, %s\n", __func__, para); 
	strcpy(response, "OKAY");

	if(!irf->strncmp(para, "stage", 5))
	  strcpy(p, glpstage);
	else if(!irf->strncmp(para, "serialno", 8))
	  strcpy(p, device_strings[DEVICE_STRING_SERIAL_NUMBER_INDEX]);
	else if(!irf->strncmp(para, "downloadsize", 12))
	  irf->sprintf(p, "0x%x", glpbufsize);
	else {
	}

	return 0;
}

/*
 * download
 * download something .. 
 * what happens to it depends on the next command after data 
 */
static int lp_prepare_download(const char *size , char *response)
{
	glpbytes = 0;
	glperror = 0;
	glpdownload = irf->simple_strtoul(size, NULL, 16);

	irf->printf("starting download of %d bytes\n", glpdownload);

	if (!glpdownload)
	  irf->sprintf(response, "FAILdata invalid size: 0");
	else if (glpdownload > glpbufsize) {
		irf->sprintf(response, "FAILdata too large: 0x%x", glpdownload);
		glpdownload = 0;
	}else if (!ld_dram_ready() && ld_need_dram()){
		irf->sprintf(response, "FAILdram not ready");
		glpdownload = 0;
	}
	else {
		/* The default case, the transfer fits
		   completely in the interface buffer */
		irf->sprintf(response, "DATA%08x", glpdownload);
	}

	return 0;
}

static int lp_download(const char *buffer, unsigned int buffer_size)
{
	char response[128];
	int transfer_size; 
	//aaaa
	debugp("%s\n", __func__);

	if (!buffer_size) {
		irf->printf("ignore empty buffer.\n");
		return 0;
	}

	/* Handle possible overflow */
	transfer_size = glpdownload - glpbytes;

	if (buffer_size < transfer_size)
	  transfer_size = buffer_size;

	glpbytes += transfer_size;

	/* Check if transfer is done */
	if (glpbytes >= glpdownload) {
		/* Reset global transfer variable,
		   Keep glpbytes because it will be
		   used in the next possible flashing command */
		glpdsize = glpdownload;
		glpdownload = 0;

		if (glperror) {
			/* There was an earlier error */
			irf->sprintf(response, "ERROR");
		} else {
			/* Everything has transferred,
			   send the OK response */
			irf->sprintf(response, "OKAY");
			ld_update_stat();
		}
		udc_write((uint8_t *)response, strlen(response));

		irf->printf("\ndownloading of %d bytes finished\n",
					glpbytes);
	}

	return 0;
}

static void __response_ok(void)
{
	char res[32];
	irf->sprintf(res, "OKAY");
	udc_write((uint8_t *)res, 4);

	irf->udelay(500000); /* delay 500ms */
}

/*
 * boot
 * boot the system to android in specified mode
 * if there is predownload image, boot it.
 */
static int lp_boot(const char *para, char *response) {
	if (ld_finish()) {

		//exit linkpc to boot kernel
		__response_ok();
		return 1;
	} else {
		irf->sprintf(response, "FAILboot image not ready");
	}
	return 0;
}

/*
 *reboot 
 *reboot the board
 */
static void lp_reboot(const char* para)
{
	__response_ok();
	board_reset();
	for(;;);
}	

/* exit
 * exit to shell
 */
static void lp_escape(void) {
	__response_ok();

	irf->cdbg_shell();

	/* here never reached */
	for(;;);
}

/* oem:assign
 * assign target device to burn
 */
static int lp_assign_ld(const char *s, char *r, uint32_t idx)
{
	char *next;
	uint8_t *start;
	uint32_t bufsize;

	s++; // skip space for oem commands
	idx = idx&0xf;
	if (idx >= LD_IMAGE_NUM) {
		irf->sprintf(r, "OKAYinvliad para ld_idx : 0x%x", idx);
		return 0;
	}
	
	if(idx == LD_KERNEL) {
		start = (uint8_t *)irf->simple_strtoul(s, &next, 16);
		bufsize = irf->simple_strtoul(next + 1, NULL, 16);
		if (((uint32_t)start<DRAM_BASE_PA) || bufsize==0 ) {
			irf->sprintf(r, "FAILinvalid para, idx:%d start: 0x%x, bufsize: 0x%x",
					idx, start, bufsize);
			return 0;
		}
		if (dram_size==0) {
			irf->sprintf(r,"FAILdram not assigned\n");
			return 0;
		}
	} else if(idx == LD_RAMDISK) {
		start = (uint8_t*)(RAMDISK_START-sizeof(image_header_t));
		bufsize = 0x800000;
		if (dram_size==0) {
			irf->sprintf(r,"FAILdram not assigned\n");
			return 0;
		}
	} else if(idx == LD_ITEM) {
		if (item_ldbuf==0) {
			irf->sprintf(r,"FAILitem buf not ready\n");
			return 0;
		}
		start = item_ldbuf;
		bufsize = ITEM_SIZE_EMBEDDED;
	} else {
		irf->sprintf(r, "FAILidx not support now%x", idx);
		return 0;
	}

	if (!ld_try_set(idx, start, bufsize)) {
		irf->sprintf(r, "FAILinvalid para, overlay idx:%d start: 0x%x, bufsize: 0x%x",
					idx, start, bufsize);
		return 0;
	}


	irf->printf("ld_info[%d], start: 0x%x, bufsize: 0x%x\n",
				idx, start, bufsize);
	irf->sprintf(r, "OKAY idx: %d, start: 0x%x, bufsize: 0x%x",
				start, bufsize);
	ld_cur_idx = idx;
	glpmem = start;
	glpbufsize = bufsize;
	return 0;
}

static int lp_assign_dram(const char *s, char *r) {
	s++; // skip space for oem commands
	if (ld_dram_stat) {
		irf->sprintf(r, "FAILdram inited in previous step");
		return 0;
	}
	if (ld_get_stat(LD_ITEM)&&item_ldbuf) {
		//init item for dram config
		item_init(item_ldbuf, ITEM_SIZE_EMBEDDED);
	} else {
		irf->sprintf(r, "FAIL item not ready");
		return 0;
	}
	
	if (!dramc_init_one_type()) {
		ld_dram_stat = 1;
		irf->sprintf(r, "OKAYdram init ok");
	} else {
		irf->sprintf(r, "FAILdram init failed");
		ld_dram_stat = 0;
		return 0;
	}
	
    rbinit();
    rtcbit_set_rbmem();
    rbsetint ("dramsize", dram_size_check());
    rbsetint ("bootmode", BOOT_RECOVERY_IUS);
	dram_size = calc_dram_M();
	irf->printf("dram_size=%d\n", dram_size);
	return 0;
}

static int lp_assign(const char *s, char *r)
{
	char *next;
	uint32_t idx;

	s++; // skip space for oem commands
	idx = irf->simple_strtoul(s, &next, 16);
	if (idx&ASSIGN_MASK_LD) {
		lp_assign_ld(next, r, idx);
	} else if (idx&ASSIGN_MASK_DRAM) {
		lp_assign_dram(next, r);
	} else {
		irf->sprintf(r, "FAILassign format not recognized");
	}

	return 0;
}


/* oem:setserial
 * setserial target device to burn
 */
static int lp_setserial(const char *s, char *r)
{
	s++; // skip space for oem commands

	strcpy(device_strings[DEVICE_STRING_SERIAL_NUMBER_INDEX], s);

	__response_ok();

	/* recall the loop to relink */
	linkpc_loop();
	return 0;
}

/*
 * flash
 * burn images into disk
 */
static int lp_flash(const char *s, char *r)
{
	//just fore response
	irf->sprintf(r, "OKAY");
	return 0;
}

static int rx_handler (const char *buffer, unsigned int buffer_size)
{
	char response[128];
	int ret = 1;

	if(glpdownload) {
		ret = lp_download(buffer, buffer_size);
	} else {
		/* this is a command */
		debugp("%s\n", buffer);

		irf->sprintf(response, "FAIL");

		if(irf->strncmp(buffer, "reboot", 6) == 0) {
			lp_reboot(buffer + 6);	
		} else if(irf->strncmp(buffer, "getvar:", 7) == 0) {
			ret = lp_get_value(buffer + 7, response);			
		} else if(irf->strncmp(buffer, "download:", 9) == 0) {
			ret = lp_prepare_download(buffer + 9, response);
		} else if(irf->strncmp(buffer, "flash:", 6) == 0) {
			ret = lp_flash(buffer + 6, response);

			/* here are the OEM functions */
		} else if(irf->strncmp(buffer, "oem boot", 8) == 0) {
			ret = lp_boot(buffer + 8, response);
			return ret;
		} else if(irf->strncmp(buffer, "oem assign", 10) == 0) {
			ret = lp_assign(buffer + 10, response);
		} else if(irf->strncmp(buffer, "oem setserial", 13) == 0) {
			ret = lp_setserial(buffer + 13, response);
		} else if(irf->strncmp(buffer, "oem esc", 7) == 0) {
			lp_escape();
		} else
		  irf->sprintf(response, "FAILno such command");

		if(udc_write((uint8_t *)response, irf->strnlen(response, 256))) { 
			debugp("error transfer status code failed\n");
			return -1;
		}
	}

	return 0;
}

int linkpc_loop(void)
{		
	int ret = 1, len = 0;
	uint8_t buf[64], *p = NULL;

	irf->printf("linking PC ...");

	/* initialize the board specific support */
	if (!udc_connect())
	{
		irf->printf ("OK\n");
		irf->printf ("communicating ...\n");


		/* if we got this far, we are a success */
		ret = 0;

		/* On disconnect or error, polling returns non zero */
		while (1) {
			len = !glpdownload? 64: glpdownload;
			p   = !glpdownload? buf: glpmem + glpbytes;

			/* read data from host and handle it */
			udc_read(p, len);	
			if(rx_handler((char *)p, len))
			  break;
		}
	}

	if (ret) {
		irf->printf("Failed\n");
	}
	return ret;
}

void linkpc_set_stage(const char * stage) {

	irf->strncpy(glpstage, stage, 16);
}

int linkpc_manual_forced(unsigned int gpio) {
	unsigned int gpdir;
	unsigned int gppin;
#if CONFIG_LINKPC_MANUAL_CHECK
	gpdir = readl(RTC_GPDIR);	
	
	//set gpx to input
	gpdir |= 0x2;
	writel(gpdir, RTC_GPDIR);

	//check if gpx is set
	gppin = readl(RTC_GPINPUT_ST);

	if(gppin&0x2) {
		irf->printf("linkpc manually forced\n");
		return 1;		//force linkpc
	}
#else
	if (gpio <= 124) {
		irf->pads_chmod(PADSRANGE(gpio, gpio), PADS_MODE_IN, 0);		
		gppin = irf->pads_getpin(gpio);
		if (gppin) {
			irf->printf("linkpc manually forced\n");
			return 1;	
		}
	}
#endif
	return 0;
}

int linkpc_necessary(void) {
	int boot_dev;
	boot_dev = irf->boot_device();

	// the final tried boot device will be snnd
	if (boot_dev!=DEV_SNND) {
		return 0;
	}
	irf->printf("linkpc necessary, boot_dev=%d\n", boot_dev);
	return 1;
}

void linkpc_boot(void)
{
	void (*theKernel)(int zero, int arch, uint params);
	ulong rd_start, rd_end;
	char buf_tmp[sizeof(image_header_t)];
	unsigned int size;

	image_header_t *p = (image_header_t*) ld_info[LD_KERNEL].start;
	if(p->ih_magic != KERNEL_MAGIC) {
		spl_printf ( "wrong kernel header:magic 0x%x\n", p->ih_magic );
		irf->cdbg_shell();
		while ( 1 );
	}

	int_endian_change((unsigned int *)&theKernel, p->ih_ep);
	int_endian_change(&size, p->ih_size);

	spl_printf("kernel( %d K)\n", size >> 10);
  	theKernel += KERNEL_INNER_OFFS;
	p = (image_header_t *)(ld_info[LD_RAMDISK].start);
	int_endian_change(&size, p->ih_size);
	rd_start = RAMDISK_START;
	rd_end = rd_start + size;
		
	item_rrtb = rballoc("itemrrtb", ITEM_SIZE_EMBEDDED);
	if (item_rrtb==0) {
		spl_printf("item_rrtb rballoc failed\n");
	}else {
		irf->memcpy(item_rrtb, item_ldbuf, ITEM_SIZE_EMBEDDED);
	}
	//tag
	setup_start_tag();
	if ( rd_start && rd_end)
		setup_initrd_tag(rd_start, rd_end);

	irf->sprintf(buf_tmp, CMDLINE_RECOVERY_LINKPC, loop_per_jiffies, dram_size, "linkpc", "1M");
	setup_commandline_tag ( buf_tmp );
	setup_end_tag();
	asm ( "dsb" );

	spl_printf("b kernel\n");
	theKernel(0, 0x08f9, TAG_START);
}

int linkpc_hook(void) {

	int ret;	
	if(ret = ld_init()) {
		return -1;
	}
	irf->module_set_clock(USBREF_CLK_BASE, PLL_D, 63);
	ret = linkpc_loop();
	if (ret==0 && ld_finish()) {
		linkpc_boot();	
	}
	ld_exit();
	irf->cdbg_shell();
	while(1);
	return 0;
}


