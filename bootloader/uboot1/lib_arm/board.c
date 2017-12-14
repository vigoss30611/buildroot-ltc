/*
 * (C) Copyright 2002-2006
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
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

/*
 * To match the U-Boot user interface on ARM platforms to the U-Boot
 * standard (as on PPC platforms), some messages with debug character
 * are removed from the default U-Boot build.
 *
 * Define DEBUG here if you want additional info as shown below
 * printed upon startup:
 *
 * U-Boot code: 00F00000 -> 00F3C774  BSS: -> 00FC3274
 * IRQ Stack: 00ebff7c
 * FIQ Stack: 00ebef7c
 */

#include <common.h>
#include <command.h>
#include <malloc.h>
#include <stdio_dev.h>
#include <timestamp.h>
#include <version.h>
#include <net.h>
#include <serial.h>
#include <nand.h>
#include <onenand_uboot.h>
#include <mmc.h>
#include <rtcbits.h>
#include <board.h>
#include <efuse.h>
#include <storage.h>

#ifdef CONFIG_DRIVER_SMC91111
#include "../drivers/net/smc91111.h"
#endif
#ifdef CONFIG_DRIVER_LAN91C96
#include "../drivers/net/lan91c96.h"
#endif

#include <items.h>
#include <bootlist.h>
#include <lowlevel_api.h>
#include <cpuid.h>

DECLARE_GLOBAL_DATA_PTR;

ulong monitor_flash_len;

#ifdef CONFIG_HAS_DATAFLASH
extern int  AT91F_DataflashInit(void);
extern void dataflash_print_info(void);
#endif

#ifndef CONFIG_IDENT_STRING
#define CONFIG_IDENT_STRING ""
#endif

const char version_string[] =
	U_BOOT_VERSION" (" U_BOOT_DATE " - " U_BOOT_TIME ")\n"CONFIG_IDENT_STRING;

#ifdef CONFIG_DRIVER_CS8900
extern void cs8900_get_enetaddr (void);
#endif

#ifdef CONFIG_DRIVER_GMAC
extern int gmac_initialize(bd_t *bis);
extern void set_descrpt_addr(unsigned int addr);
int board_eth_init(bd_t *bis)
{
	return gmac_initialize(bis);
}
#endif


#ifdef CONFIG_DRIVER_RTL8019
extern void rtl8019_get_enetaddr (uchar * addr);
#endif

#if defined(CONFIG_HARD_I2C) || \
	defined(CONFIG_SOFT_I2C)
#include <i2c.h>
#endif

/*
 * Begin and End of memory area for malloc(), and current "brk"
 */
static ulong mem_malloc_start = 0;
static ulong mem_malloc_end = 0;
static ulong mem_malloc_brk = 0;

static
void mem_malloc_init (ulong dest_addr)
{
	mem_malloc_start = dest_addr;
	mem_malloc_end = dest_addr + CONFIG_SYS_MALLOC_LEN;
	mem_malloc_brk = mem_malloc_start;

//	memset ((void *) mem_malloc_start, 0,
//			mem_malloc_end - mem_malloc_start);
}

void *sbrk (ptrdiff_t increment)
{
	ulong old = mem_malloc_brk;
	ulong new = old + increment;

	if ((new < mem_malloc_start) || (new > mem_malloc_end)) {
		return (NULL);
	}
	mem_malloc_brk = new;

	return ((void *) old);
}


/************************************************************************
 * Coloured LED functionality
 ************************************************************************
 * May be supplied by boards if desired
 */
#if 0
void inline __coloured_LED_init (void) {}
void inline coloured_LED_init (void) __attribute__((weak, alias("__coloured_LED_init")));
void inline __red_LED_on (void) {}
void inline red_LED_on (void) __attribute__((weak, alias("__red_LED_on")));
void inline __red_LED_off(void) {}
void inline red_LED_off(void)		 __attribute__((weak, alias("__red_LED_off")));
void inline __green_LED_on(void) {}
void inline green_LED_on(void) __attribute__((weak, alias("__green_LED_on")));
void inline __green_LED_off(void) {}
void inline green_LED_off(void)__attribute__((weak, alias("__green_LED_off")));
void inline __yellow_LED_on(void) {}
void inline yellow_LED_on(void)__attribute__((weak, alias("__yellow_LED_on")));
void inline __yellow_LED_off(void) {}
void inline yellow_LED_off(void)__attribute__((weak, alias("__yellow_LED_off")));
void inline __blue_LED_on(void) {}
void inline blue_LED_on(void)__attribute__((weak, alias("__blue_LED_on")));
void inline __blue_LED_off(void) {}
void inline blue_LED_off(void)__attribute__((weak, alias("__blue_LED_off")));
#endif

/************************************************************************
 * Init Utilities							*
 ************************************************************************
 * Some of this code should be moved into the core functions,
 * or dropped completely,
 * but let's get it working (again) first...
 */

#if defined(CONFIG_ARM_DCC) && !defined(CONFIG_BAUDRATE)
#define CONFIG_BAUDRATE 115200
#endif
static int init_baudrate (void)
{
	char tmp[64];	/* long enough for environment variables */
	int i = getenv_r ("baudrate", tmp, sizeof (tmp));
	gd->bd->bi_baudrate = gd->baudrate = (i > 0)
			? (int) simple_strtoul (tmp, NULL, 10)
			: CONFIG_BAUDRATE;

	return (0);
}

static int display_banner (void)
{
	printf ("\n\n%s\n\n", version_string);
	debug ("U-Boot code: %08lX -> %08lX  BSS: -> %08lX\n",
		   _armboot_start, _bss_start, _bss_end);
#ifdef CONFIG_MODEM_SUPPORT
	debug ("Modem Support enabled\n");
#endif
#ifdef CONFIG_USE_IRQ
	debug ("IRQ Stack: %08lx\n", IRQ_STACK_START);
	debug ("FIQ Stack: %08lx\n", FIQ_STACK_START);
#endif

	return (0);
}

/*
 * WARNING: this code looks "cleaner" than the PowerPC version, but
 * has the disadvantage that you either get nothing, or everything.
 * On PowerPC, you might see "DRAM: " before the system hangs - which
 * gives a simple yet clear indication which part of the
 * initialization if failing.
 */
static int display_dram_config (void)
{
	int i;

#ifdef DEBUG
	puts ("RAM Configuration:\n");

	for(i=0; i<CONFIG_NR_DRAM_BANKS; i++) {
		printf ("Bank #%d: %08lx ", i, gd->bd->bi_dram[i].start);
		print_size (gd->bd->bi_dram[i].size, "\n");
	}
#else
	ulong size = 0;

	for (i=0; i<CONFIG_NR_DRAM_BANKS; i++) {
		size += gd->bd->bi_dram[i].size;
	}
#if defined(CONFIG_SYS_MEMTYPE)
	puts("Memory type: " CONFIG_SYS_MEMTYPE);
#else
	puts("Memory type: DDRII ");
#endif
	print_size(size, "");

#ifdef CONFIG_IMAPX200
	printf(", ");
	oem_print_eye(0);
#else
	puts("\n");
#endif
#endif

	return (0);
}

#ifndef CONFIG_SYS_NO_FLASH
static void display_flash_config (ulong size)
{
	puts ("Flash: ");
	print_size (size, "\n");
}
#endif /* CONFIG_SYS_NO_FLASH */

#if defined(CONFIG_HARD_I2C) || defined(CONFIG_SOFT_I2C)
static int init_func_i2c (void)
{
	puts ("I2C:   ");
	i2c_init (CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
	puts ("ready\n");
	return (0);
}
#endif

#if defined(CONFIG_CMD_PCI) || defined (CONFIG_PCI)
#include <pci.h>
static int arm_pci_init(void)
{
	pci_init();
	return 0;
}
#endif /* CONFIG_CMD_PCI || CONFIG_PCI */

/*
 * Breathe some life into the board...
 *
 * Initialize a serial port as console, and carry out some hardware
 * tests.
 *
 * The first part of initialization is running from Flash memory;
 * its main purpose is to initialize the RAM so that we
 * can relocate the monitor code to RAM.
 */

/*
 * All attempts to come up with a "common" initialization sequence
 * that works for all boards and architectures failed: some of the
 * requirements are just _too_ different. To get rid of the resulting
 * mess of board dependent #ifdef'ed code we now make the whole
 * initialization sequence configurable to the user.
 *
 * The requirements for any new initalization function is simple: it
 * receives a pointer to the "global data" structure as it's only
 * argument, and returns an integer return code, where 0 means
 * "continue" and != 0 means "fatal error, hang the system".
 */
typedef int (init_fnc_t) (void);

int print_cpuinfo (void);

init_fnc_t *init_sequence[] = {
#if defined(CONFIG_ARCH_CPU_INIT)
	arch_cpu_init,		/* basic arch cpu dependent setup */
#endif
	board_init,		/* basic board dependent setup */
#if defined(CONFIG_USE_IRQ)
	interrupt_init,		/* set up exceptions */
#endif
	init_timer,		/* initialize timer */
	env_init,		/* initialize environment */
	init_baudrate,		/* initialze baudrate settings */
	//init_serial,		/* serial communications setup */
	console_init_f,		/* stage 1 init of console */
	display_banner,		/* say that we are here */
#if defined(CONFIG_DISPLAY_CPUINFO)
	print_cpuinfo,		/* display cpu info (and speed) */
#endif
#if defined(CONFIG_DISPLAY_BOARDINFO)
	checkboard,		/* display board info */
#endif
#if defined(CONFIG_HARD_I2C) || defined(CONFIG_SOFT_I2C)
	init_func_i2c,
#endif
	dram_init,		/* configure available RAM banks */
#if defined(CONFIG_CMD_PCI) || defined (CONFIG_PCI)
	arm_pci_init,
#endif
	display_dram_config,
	NULL,
};

void mute_codec(void);
extern void gpio_reset(void);
void start_armboot (void)
{
	init_fnc_t **init_fnc_ptr;
	char *s;
	unsigned char *p;
#if defined(CONFIG_VFD) || defined(CONFIG_LCD)
	unsigned long addr;
#endif
	/*
	*Author:summer
	*Date:2014-5-23
	*Function: uboot0 will check whether ius card exist?
	*IF exist,write 0xaaaaeeee to 0x90004000;
	*IF not,write 0xaaaabbbb to 0x90004000
	*/
	int ius_burned = 0;
	//memcpy(&ius_burned,CONFIG_SYS_SDRAM_END-0x1000000+0x4000,4);
	//set_ius_card_state(ius_burned);

	
	/* Pointer is writable since we allocated a register for it */
	gd = (gd_t*)(_armboot_start - CONFIG_SYS_MALLOC_LEN - sizeof(gd_t));
	/* compiler optimization barrier needed for GCC >= 3.4 */
	__asm__ __volatile__("": : :"memory");

	memset ((void*)gd, 0, sizeof (gd_t));
	gd->bd = (bd_t*)((char*)gd - sizeof(bd_t));
	memset (gd->bd, 0, sizeof (bd_t));

	gd->flags |= GD_FLG_RELOC;

	monitor_flash_len = _bss_start - _armboot_start;

	for (init_fnc_ptr = init_sequence; *init_fnc_ptr; ++init_fnc_ptr) {
		if ((*init_fnc_ptr)() != 0) {
			hang ();
		}
	}

	/* armboot_start is defined in the board-specific linker script */
//	mem_malloc_init (_armboot_start - CONFIG_SYS_MALLOC_LEN - (4 << 20));
	mem_malloc_init (_armboot_start - CONFIG_SYS_MALLOC_LEN - (10 << 20));

#ifndef CONFIG_SYS_NO_FLASH
	/* configure available FLASH banks */
	display_flash_config (flash_init ());
#endif /* CONFIG_SYS_NO_FLASH */

#ifdef CONFIG_VFD
#	ifndef PAGE_SIZE
#	  define PAGE_SIZE 4096
#	endif
	/*
	 * reserve memory for VFD display (always full pages)
	 */
	/* bss_end is defined in the board-specific linker script */
	addr = (_bss_end + (PAGE_SIZE - 1)) & ~(PAGE_SIZE - 1);
	vfd_setmem (addr);
	gd->fb_base = addr;
#endif /* CONFIG_VFD */

#ifdef CONFIG_LCD
	/* board init may have inited fb_base */
	if (!gd->fb_base) {
#		ifndef PAGE_SIZE
#		  define PAGE_SIZE 4096
#		endif
		/*
		 * reserve memory for LCD display (always full pages)
		 */
		/* bss_end is defined in the board-specific linker script */
		addr = (_bss_end + (PAGE_SIZE - 1)) & ~(PAGE_SIZE - 1);
		lcd_setmem (addr);
		gd->fb_base = addr;
	}
#endif /* CONFIG_LCD */

	rtcbit_init();

#if defined(CONFIG_CMD_NAND) && !defined(CONFIG_SYS_DISK_iNAND)
	puts("NAND: ");
	nand_init();		/* go init the NAND */
#endif
	//mute_codec();
	init_config();
	gpio_reset();
	//if(iuw_burned == 1)
		//display_logo(-1);
	//rtcbit_init();
	board_scan_arch();
	board_init_i2c();

	/* initialize environment */
	//env_relocate ();
#if defined(CONFIG_CMD_NAND) && !defined(CONFIG_SYS_DISK_iNAND)
	nftl_init();
#endif

#if defined(CONFIG_CMD_ONENAND)
	onenand_init();
#endif

#ifdef CONFIG_HAS_DATAFLASH
	AT91F_DataflashInit();
	dataflash_print_info();
#endif

#ifdef CONFIG_VFD
	/* must do this after the framebuffer is allocated */
	drv_vfd_init();
#endif /* CONFIG_VFD */

#ifdef CONFIG_SERIAL_MULTI
	//serial_initialize();
#endif

	/* IP Address */
//	gd->bd->bi_ip_addr = getenv_IPaddr ("ipaddr");
	 debug ("stdio_init\n");
	stdio_init ();	/* get the devices list going. */

	jumptable_init ();

#if defined(CONFIG_API)
	/* Initialize API */
	api_init ();
#endif

	console_init_r ();	/* fully init console as a device */

#if defined(CONFIG_ARCH_MISC_INIT)
	/* miscellaneous arch dependent initialisations */
	arch_misc_init ();
#endif
#if defined(CONFIG_MISC_INIT_R)
	/* miscellaneous platform dependent initialisations */
	misc_init_r ();
#endif

	/* enable exceptions */
	enable_interrupts ();

	/* Perform network card initialisation if necessary */
#ifdef CONFIG_DRIVER_TI_EMAC
	/* XXX: this needs to be moved to board init */
extern void davinci_eth_set_mac_addr (const u_int8_t *addr);
	if (getenv ("ethaddr")) {
		uchar enetaddr[6];
		eth_getenv_enetaddr("ethaddr", enetaddr);
		davinci_eth_set_mac_addr(enetaddr);
	}
#endif

#ifdef CONFIG_DRIVER_CS8900
	/* XXX: this needs to be moved to board init */
	cs8900_get_enetaddr ();
#endif

#if defined(CONFIG_DRIVER_SMC91111) || defined (CONFIG_DRIVER_LAN91C96)
	/* XXX: this needs to be moved to board init */
	if (getenv ("ethaddr")) {
		uchar enetaddr[6];
		eth_getenv_enetaddr("ethaddr", enetaddr);
		smc_set_mac_addr(enetaddr);
	}
#endif /* CONFIG_DRIVER_SMC91111 || CONFIG_DRIVER_LAN91C96 */

	/* Initialize from environment */
	if ((s = getenv ("loadaddr")) != NULL) {
		load_addr = simple_strtoul (s, NULL, 16);
	}
#if defined(CONFIG_CMD_NET)
	if ((s = getenv ("bootfile")) != NULL) {
		copy_filename (BootFile, s, sizeof (BootFile));
	}
#endif

#ifdef BOARD_LATE_INIT
	board_late_init ();
#endif

	init_efuse();

	init_xom();
	init_mux(0);

	init_mmc ();

	storage_init();
	env_relocate ();
	debug("***-------------------***\n");

#if defined(CONFIG_CMD_NET)
#if defined(CONFIG_NET_MULTI)
#endif
	eth_initialize(gd->bd);
#if defined(CONFIG_RESET_PHY_R)
	debug ("Reset Ethernet PHY\n");
	reset_phy();
#endif
#endif

	/* init config at this early moment to ensure that
	 * all drivers can get items correctly.
	 */
	//init_config();
	//init_mux(1);


	/* main_loop() can return to retry autoboot, if so just run it again. */
	for (;;) {
		main_loop ();
	}

	/* NOTREACHED - no way out of command loop except booting */
}

void hang (void)
{
	puts ("### ERROR ### Please RESET the board ###\n");
	for (;;);
}

//#define __UBOOT0__


#ifdef __UBOOT0__
#define LOG spl_printf
#else
#define LOG printf
#endif
#include "imapx_iic.h"

#define IIC_CLK		 100000
#define readl(addr)	*(volatile u_long *)(addr)
#define writel(b,addr) ((*(volatile u32 *) (addr)) = (b))

uint8_t ub_i2c_init(uint8_t bus_id, uint8_t * dev_addr, uint8_t isrestart, uint8_t ignoreack);
uint8_t ub_i2c_com_op(uint8_t bus_id,  uint8_t * dev_addr,uint8_t *wdata,uint32_t wlen,uint8_t *rdata, uint32_t rlen);
void x_udelay(uint32_t cnt);


void mute_codec(void)
{
	uint8_t buf[2], addr;
	//static  int dl=0;
#if 1
	addr=0x0;
	ub_i2c_init(1,&addr,1,0);//init bus 1 & reset i2c module
	addr=0x20;//codec
	buf[0]=0x0;
	buf[1]=0x02;
	ub_i2c_com_op(1,&addr,&buf[0],2,0,0);//disable refer in codec
	buf[1]=0x55;
	ub_i2c_com_op(1,&addr,&buf[0],1,&buf[1],1);//disable refer in codec
	LOG("codec %x=%x\n",buf[0],buf[1]);
#else
	LOG("dummy test codec %x=%x\n",buf[0],buf[1]);

#endif

	writel(0xff, I2C_SYSM_ADDR);
}

#define TRC_F(x)  LOG("%s-[%d]\n",__FUNCTION__,x)

uint8_t ub_i2c_module_init(uint8_t bus_id, uint8_t * dev_addr,uint8_t isrestart, uint8_t ignoreack)
{
	uint32_t temp_reg, clk;
	clk = 0x3ff;
	if(bus_id>4)
		return FALSE;
	writel(0x0,IC_ENABLE+bus_id*0x1000); //0x6c i2c channel enable ,0 disable
	writel((STANDARD | MASTER_MODE | SLAVE_DISABLE),IC_CON+bus_id*0x1000);//0x0, config i2c master mode. stander mode100kps
	writel(clk, IC_SS_SCL_HCNT+bus_id*0x1000);//config clk h
	writel(clk, IC_SS_SCL_LCNT+bus_id*0x1000);//config clk l
	writel(clk/2,IC_IGNORE_ACK0);  //0xa0,
	temp_reg = readl(IC_IGNORE_ACK0+bus_id*0x1000);
	temp_reg &= ~(0x1<<8);
	writel(temp_reg,IC_IGNORE_ACK0+bus_id*0x1000);//sdata config data hold. ack ignor
	temp_reg = readl(IC_CON+bus_id*0x1000);
	temp_reg |= IC_RESTART_EN+bus_id*0x1000;//0x0,cfg i2c restart
	writel(temp_reg,IC_CON+bus_id*0x1000);
	writel(0x0,IC_INTR_MASK+bus_id*0x1000);  //0x30,  no irq
	writel((*dev_addr)>>1,IC_TAR+bus_id*0x1000);//config i2c dev addr ,must disable first
	writel(0x1,IC_ENABLE+bus_id*0x1000); //enable i2c

	return TRUE;
}


/*
   master only.
   busy wait mode, no interrupt
   */

uint8_t ub_i2c_init(uint8_t bus_id, uint8_t * dev_addr, uint8_t isrestart, uint8_t ignoreack)
{
	int ret;
	uint32_t temp_reg=0;
	LOG("%s init i2c %d\n",__TIME__,bus_id);
	if(0==*dev_addr) {
		LOG("%x,reset i2c\n",*dev_addr);
#ifdef __UBOOT0__
		irf->module_enable(I2C_SYSM_ADDR);//i2c modeule enable reset,clk,io,etc.
#else
		module_enable(I2C_SYSM_ADDR);//i2c modeule enable reset,clk,io,etc.
#endif
	} else {
		LOG("dev_addr=0 will reset module\n");
	}

	//  LOG("-21e09078=%x\n",readl(0x21e09078));
	if(IROM_IDENTITY == IX_CPUID_X15  || IROM_IDENTITY == IX_CPUID_X15_NEW) {
		LOG("cpu is imapx15\n");
		switch(bus_id) {
			case 0:
				break;
			case 1:
				temp_reg=readl(0x21e09078);
				temp_reg&=0x9f;//clear i2c1 pad to function mode
				writel(temp_reg,0x21e09078);
				break;
			case 2:
				// gpio_mode_set(0, 47);
				// gpio_mode_set(0, 48);
			default:
				break;
		}
	} else {
		LOG("cpu is imapx820\n");
	}
	LOG("--21e09078=%lx\n",readl(0x21e09078));
	ret = ub_i2c_module_init(bus_id,dev_addr,isrestart,ignoreack);
	return ret;
}



/*
   this function is a combination operate function,
   like the i2c_msg.
   first write then read.
   */

#define  __I2C_LOG__ 1


void x_udelay(uint32_t cnt)
{
	while(cnt--);
}

uint8_t ub_i2c_com_op(uint8_t bus_id,  uint8_t * dev_addr,uint8_t *wdata,uint32_t wlen,uint8_t *rdata, uint32_t rlen)
{
	uint32_t temp_reg=0,i=0;
	uint32_t try_cnt=0;

	// LOG("input:id=%d,dev_addr=0x%x,wd=%x,wl=%d,rlen=%d\n",bus_id,*dev_addr,*wdata,wlen,rlen);
#if 1
	writel(0x0,IC_ENABLE+bus_id*0x1000); //0x6c i2c channel enable ,0 disable
	writel((*dev_addr)>>1,IC_TAR+bus_id*0x1000);//config i2c dev addr ,must disable first
	writel(0x1,IC_ENABLE+bus_id*0x1000); //0x6c i2c channel enable ,1 enable
#endif

	temp_reg = readl(IC_RAW_INTR_STAT+bus_id*0x1000);//0x34
	if(temp_reg & TX_ABORT)  //check TX_ABT=(0x1<<6)
	{
		readl(IC_TX_ABRT_SOURCE+bus_id*0x1000);//0x80
		readl(IC_CLR_TX_ABRT+bus_id*0x1000);//0x54
		return 0;
	}

	temp_reg = readl(IC_STATUS+bus_id*0x1000);//0x70
	if(temp_reg & (0x1<<2)) {//tfe  trans fifo complete empty
		for(i=0;i<wlen;i++) {//write data
#define TRY_MAX_CNT 100
			try_cnt=0;
			while(1 && try_cnt++< TRY_MAX_CNT) {

				temp_reg = readl(IC_STATUS+bus_id*0x1000);
				if(temp_reg & (0x1<<1)) {//tfnf :trans fifo not full,wait for write
					writel((*wdata)&(0xffUL), IC_DATA_CMD+bus_id*0x1000);
					wdata++;
					break;
				}
			}
			if(try_cnt >= TRY_MAX_CNT) {//timeout
				temp_reg = readl(IC_STATUS+bus_id*0x1000);
				LOG("IC_STAT=%x\n",temp_reg);
				temp_reg = readl(IC_RAW_INTR_STAT+bus_id*0x1000);//0x34
				LOG("RAW_INTR_STAT=%x\n",temp_reg);
				temp_reg = readl( IC_TX_ABRT_SOURCE+bus_id*0x1000);//0x80
				LOG("IC_TX_ABRT_SOURCE =%x\n",temp_reg);
				LOG("%d timeout for op\n",try_cnt);
				return 0;
			}
		}
		LOG("wt try_cnt=%d\n",try_cnt);
		try_cnt=0;
		while(1) {// wait for transfer finish ,if no rdata will stop
			try_cnt++;

			if(try_cnt > 10) {
				LOG("%d timeout ,so abort\n",try_cnt);
				return 0;
			}

			temp_reg = readl(IC_STATUS+bus_id*0x1000);
			if (temp_reg & (0x1<<2)) {//tfe  trans fifo complete empty
				temp_reg = readl(IC_RAW_INTR_STAT+bus_id*0x1000);//0x34
				if(temp_reg & 0x40) {//tx_abort
					temp_reg = readl( IC_TX_ABRT_SOURCE+bus_id*0x1000);//0x80
					if(temp_reg & 0x0f) {
						LOG("%x,no ACK,so abort\n",temp_reg);
						return 0;
					}
				}

				if (0==rlen) {
					try_cnt=0;
					while(1) {
						if(try_cnt>10) {
							LOG("%d timeout ,so abort\n",try_cnt);
							return 0;
						}
						temp_reg = readl(IC_RAW_INTR_STAT+bus_id*0x1000);
						LOG("RAW_INTR_STAT=%x\n",temp_reg);
						if (temp_reg & (0x1<<9)) {//find the stop bit
							readl(IC_CLR_STOP_DET+bus_id*0x1000);//read to clear stop raw interrupt
							x_udelay(10000);
							LOG("write op done\n");
							return 1;
						}
					}
				} else {
					break;//conitnuos to rdata
				}
			}
		}
	}
	LOG("wt done try_cnt=%d\n",try_cnt);
	//now read data
	temp_reg = readl(IC_STATUS+bus_id*0x1000);//0x70
	while(temp_reg & (0x1<<3)) {//rfne recieve fifo no empty
		temp_reg = readl(IC_DATA_CMD+bus_id*0x1000);
	}
	int read_op_cnt=0;
	read_op_cnt=rlen;
	try_cnt=0;
	while (1) {
		try_cnt++;
		if(try_cnt > 2500) {
			LOG("%d timeout ,so abort\n",try_cnt);
			return 0;
		}
		temp_reg = readl(IC_RAW_INTR_STAT+bus_id*0x1000);//0x34
		if(temp_reg & 0x40) {//tx_abort
			temp_reg = readl( IC_TX_ABRT_SOURCE+bus_id*0x1000);//0x80
			if(temp_reg & 0x0f) {
				LOG("%x,no ACK,so abort\n",temp_reg);
				return 0;
			}
		}
		temp_reg = readl(IC_STATUS+bus_id*0x1000);
		if (temp_reg & (0x1 << 1) && read_op_cnt) {//tfnf :trans fifo not full
			writel(0x1<<8,IC_DATA_CMD+bus_id*0x1000);//bit8=1 is read op
			read_op_cnt--;
		}
		if (temp_reg & (0x1 << 3)) {//rfne
			*rdata = readl(IC_DATA_CMD+bus_id*0x1000);
			rdata++;
			rlen--;
		}
		if (rlen == 0)
			break;
	}
	LOG("read try_cnt=%d\n",try_cnt);
	return 2;
}
