
#include <common.h>
#include <bootlist.h>
#include <lowlevel_api.h>
#include <asm/io.h>
#include <items.h>
#include <cpuid.h>
#include <rtcbits.h>

#if !defined(CONFIG_PRELOADER) 
/*
	@ according to imapx200, the following steps is needed:
	@ a. set peripheral registers address
	@ b. remap cpu 0 address
	@ c. enable ahb peripheral bus
	@ d. power on all the modules
	@ e. close watch dog(3 WDTs)
	@ f. mask all interrupts(GIC, A5)
	@ f. close InsP
	@ g. disable all debug options
	@ h. mask all wake up source(3 GPIO & RTC @ RTC registers)
	@ i. config gpio modes
	@ j. initialize system clocks divider
	@ k. initialize plls
	@ l. set cpu sync mode(deprecated)
	@ m. info0~3 @ RTC registers

	@ a. cortex a5 do not need this
	@ b. iROM is default remapped to 0

	@ FIXME: various resets need here??
	@ x800 modes?(32-bit)
*/

#if 0
void set_pll(int pll, uint16_t value)
{
	int i;

	if(pll < 0 || pll > 3)
	  return ;

	/* prepare: disable load, close gate */
	writeb(1, CLK_SYSM_ADDR + pll * 0x18 + 0xc);

	/* prepare: change parameters */
	writeb(value & 0xff, CLK_SYSM_ADDR + pll * 0x18);
	writeb(value >> 8, CLK_SYSM_ADDR + pll * 0x18 + 4);

	/* disable pll */
	writeb(0, CLK_SYSM_ADDR + pll * 0x18 + 0x10);

	/* out=pll, src=osc */
	writeb(2, CLK_SYSM_ADDR + pll * 0x18 + 0x8);

	/* enable load */
	writeb(3, CLK_SYSM_ADDR + pll * 0x18 + 0xc);

	/* enable pll */
	writeb(1, CLK_SYSM_ADDR + pll * 0x18 + 0x10);

	/* wait pll stable:
	 * according to RTL simulation, PLLs need 200us before stable
	 * the poll register cycle is about 0.75us, so 266 cycles
	 * is needed.
	 * we put 6666 cycle here to perform a 5ms timeout.
	 */
	for( i = 0; (i < 6666) &&
		!(readb(CLK_SYSM_ADDR + pll * 0x18 + 0x14) & 1);
		i++ );
	
	/* open gate */
	writel(2, CLK_SYSM_ADDR + pll * 0x18 + 0xc);
}

void set_clk_divider(int type, uint8_t *v)
{
	uint32_t addr, i;

	if(type < 0 || type > 5)
	  return;

	addr = CLK_SYSM_ADDR + 0x70 + 0x20 * type;
	for(i = 0; i < 8; i++)
	  writeb(*(v + i), addr + i * 4);
}
#endif

#if 0
void set_clk(struct cdbg_cfg_clks *clk)
{
	if(clk->apll) set_pll(0, clk->apll);
	if(clk->dpll) set_pll(1, clk->dpll);
	if(clk->epll) set_pll(2, clk->epll);
	if(clk->vpll) set_pll(3, clk->vpll);

	set_clk_divider(0, clk->cpu_clk);
	set_clk_divider(1, clk->axi_clk);
	set_clk_divider(2, clk->apb_clk);
	set_clk_divider(3, clk->axi_cpu);
	set_clk_divider(4, clk->apb_cpu);
	set_clk_divider(5, clk->apb_axi);

	/* set length and enable the divider */
	writeb((clk->div_len & 0x1f) | (1 << 7), CLK_SYSM_ADDR + 0x3c);

	/* set cpu to divide output */
	writeb(!clk->cpu_pll, CLK_SYSM_ADDR + 0x38);

	/* enable load table */
	writeb(1, CLK_SYSM_ADDR + 0x34);
}
#endif

void module_reset(uint32_t base){
	/* reset: high */
	writel(0xff, base);

	/* reset: low */
	udelay(10);
	writel(0, base);
}

void module_enable(uint32_t base) {

	/* disable bus output */
	writel(0, base + 0xc);

    writel(0xffffffff, base);

	/* enable clock */
    writel(0xff, base + 0x4);

	/* power on */
	writel(0x11, base + 0x8);

	/*dram power on*/
	writel(0x0, base + 0x14);

	while(!(readl(base + 0x8) & 2));

	/* enable module output */
	writel(readl(base + 0x8) & ~0x10, base + 8);

	/* disable IO mask */
	writel(0xff, base + 0x18);

	/* pull down reset */
	writel(0, base);

	/* enable bus output */
	writel(0xff, base + 0xc);

    return 0;
}

#if 0
void module_set_clock(uint32_t base, int srctype,
			int divtype, uint8_t divvalue)
{
	if(srctype > 5 || srctype < 0)
	  return ;
	if(!!divtype != divtype)
	  return ;

	writeb(srctype, base);
	(divtype)?
		writeb(divvalue, base + 8):
		writeb(divvalue, base + 4);
	writeb(divtype, base + 0xc);
}

void init_lowlevel(void) {
	uint16_t mux;
	int bdev, i;
//	struct cdbg_cfg_clks clk = IR_DEFAULT_CLKS;

	/* XXX deprecated by warits XXX
	 * according to frank.lin x800 do not need
	 * to shutdown external WDT
	 */
#if 0
	/* disable WDT */
	module_enable(WDT_SYSM_ADDR);
	writel(0, PWDT_BASE_ADDR);
#endif

	/* disable interrupt */
	for(i = 0; i < 8; i++)
	  writel(0xffffffff,
		GIC_DISTRIBUTOR_ADDR + 0x180 + (i << 2));

	/*
	 * initilize IO MUX, iMAPx800 config IO modes by setting
	 * IO MUX register. Totally 32 modes was  supported, IO
	 * state is fixed after IO MUX value is set.
	 *
	 * we will use the following default MUX configurations
	 * for booting state.
	 * 
	 * @mmc0: 22 (MID High End 10" iNAND)
	 * @mmc1: 22 (MID High End 10" iNAND)
	 * @mmc2: 22 (MID High End 10" iNAND)
	 * @nand: 23 (MID High End 10")
	 * @sata: 23 (MID High End 10")
	 * @eprm: 23 (MID High End 10")
	 * @udsk: 23 (MID High End 10")
	 */
	bdev = boot_device();
	switch(bdev) {
		case DEV_MMC(0):
		case DEV_MMC(1):
		case DEV_MMC(2):
			mux = 23;
			break;
		case DEV_FLASH:
			mux = boot_info() & 0x1f;
			break;
		default:
			mux = 22;
	}

	set_mux(mux);

	/* init clocks */
//	set_clk(&clk);
}
#endif

#if 0
#define SUPERSECTION 0x1000000
void mmu_map_area(uint32_t *entry, uint32_t pa,
			uint32_t len, uint32_t attr)
{
	/* alignment */
	pa  &= ~(SUPERSECTION - 1);
	len &= ~(SUPERSECTION - 1);

	for( ; len; len -= SUPERSECTION, pa += SUPERSECTION)
	  *entry++ = pa | attr;
}

void init_mmu_table(void)
{
	extern uint32_t _tstab[];
	uint32_t *p = _tstab;

	/* translate table attribute for supersections:
	 * [31:24] |base PA
	 * [23:20] |extend PA
	 * [19]    |NS (non-secure: 1)
	 * [18]    |SB1
	 * [17]    |nG (not-Global: 0)
	 * [16]    |S  (shared: 0)
	 * [15]    |AP[2]
	 * [14:12] |TEX[2:0]
	 * [11:10] |AP[1:0] (ap[2:0] set to 3b'011 to enable r/w access)
	 * [9]     |IMP
	 * [8:5]   |extend PA
	 * [4]     |XN (execute never)
	 * [3]     |C
	 * [2]     |B  SCTLR.TRE must be disabled, TEX[2:0]|C|B should be set to:
	 *         |   5b'00100: for Normal (S bit)
	 *         |   5b'01000: for Device (non-shared)
	 * [1]     |SB1
	 * [0]     |SBZ
	 */

	/* boot memory: 64M@0 */
	mmu_map_area(p, BOOT_BASE_PA, BOOT_BASE_SIZE, 0xc1c02);
	p += BOOT_BASE_SIZE >> 24;

	/* irom memory: 64M@64MB (read-only access) */
	mmu_map_area(p, IROM_BASE_PA, IROM_BASE_SIZE, 0xc9c02);
	p += IROM_BASE_SIZE >> 24;

	/* sram memory: 64M@256MB */
	mmu_map_area(p, SRAM_BASE_PA, SRAM_BASE_SIZE, 0xc1c02);
	p += SRAM_BASE_SIZE >> 24;

	/* peri memory: 192M@512MB (device) */
	mmu_map_area(p, PERI_BASE_PA, PERI_BASE_SIZE, 0xc2c02);
	p += PERI_BASE_SIZE >> 24;

	/* iram memory: 64M@960MB */
	mmu_map_area(p, IRAM_BASE_PA, IRAM_BASE_SIZE, 0xc1c02);
	p += IRAM_BASE_SIZE >> 24;

	/* dram memory: 2G@1GB */
	mmu_map_area(p, DRAM_BASE_PA, DRAM_BASE_SIZE, 0xc1c02);
}
#endif

#if !defined(CONFIG_PRELOADER)
int pads_states(int index)
{
	int group = index / 8, pin = index & 0x7, st;

	st = !!(readl(PADS_SOFT(group)) & (1 << pin));

	if(st)
	  st += !(readl(PADS_OUT_EN(group)) & (1 << pin));

	return st;
}
#endif

void pads_chmod(int index, int mode, int val)
{
	int a = index & 0xffff, b = index >> 16;

	/* invalid range */
	if( b && b < a)  return ;
	if(!b  ) b = a;

	for(index = a; index <= b; index++) {
		int group = index / 8,
		    pin = index & 0x7;

		if(index > 30 && index < 38)
		  writel(readl(PADS_SDIO_IN_EN) | (1 << (index - 31)),
					  PADS_SDIO_IN_EN);

		switch(mode)
		{
			case PADS_MODE_CTRL:
				writel(readl(PADS_SOFT(group)) &
						~(1 << pin), PADS_SOFT(group));
				break;
			case PADS_MODE_IN:
				writel(readl(PADS_SOFT(group)) |
						(1 << pin), PADS_SOFT(group));
				writel(readl(PADS_OUT_EN(group)) |
						(1 << pin), PADS_OUT_EN(group));
				break;
			case PADS_MODE_OUT:
				writel(readl(PADS_SOFT(group)) |
						(1 << pin), PADS_SOFT(group));
				writel(readl(PADS_OUT_EN(group)) &
						~(1 << pin), PADS_OUT_EN(group));
				if(val)
					writel(readl(PADS_OUT_GROUP(group)) |
							(1 << pin), PADS_OUT_GROUP(group));
				else
					writel(readl(PADS_OUT_GROUP(group)) &
							~(1 << pin), PADS_OUT_GROUP(group));
				break;
			default:
				printf("unknown pads mode. %d\n", mode);
		}
	}
}

void pads_pull(int index, int en, int high)
{
	int a = index & 0xffff, b = index >> 16;

	/* invalid range */
	if( b && b < a)  return ;
	if(!b  ) b = a;

	for(index = a; index <= b; index++) {
		int group = index / 8,
		    pin = index & 0x7;

		if(!en) 
		  writel(readl(PADS_PULL(group))
			  & ~(1 << pin), PADS_PULL(group));
		else {
			uint32_t addr = PADS_PULL_UP(group), _pin = pin;

			if(index > 30 && index < 38) {
				addr = PADS_SDIO_PULL;
				_pin = index - 31;
			}

			if(high) 
			  writel(readl(addr) |  (1 << _pin), addr);
			else
			  writel(readl(addr) & ~(1 << _pin), addr);

			writel(readl(PADS_PULL(group))
			  | (1 << pin), PADS_PULL(group));
		}
	}
}

void pads_setpin(int index, int val)
{
	int a = index & 0xffff, b = index >> 16;

	/* invalid range */
	if( b && b < a)  return ;
	if(!b  ) b = a;

	for(index = a; index <= b; index++) {

		int group = index / 8,
		    pin = index & 0x7;

		if(val)
			writel(readl(PADS_OUT_GROUP(group)) |
					(1 << pin), PADS_OUT_GROUP(group));
		else
			writel(readl(PADS_OUT_GROUP(group)) &
					~(1 << pin), PADS_OUT_GROUP(group));
	}
}

#if !defined(CONFIG_PRELOADER)
int pads_getpin(int index)
{
	int group, pin;

	index &= 0xffff;
	group = index / 8;
        pin = index & 0x7;

        if(IROM_IDENTITY != IX_CPUID_820_0 && IROM_IDENTITY != IX_CPUID_820_1)
            return !!readl(GPIO_INDAT(index));
        else
            return !!(readl(PADS_IN_GROUP(group)) & (1 << pin));
}
#endif

void init_igps_ram(void) {
	writel(1, SYS_SYSM_ADDR + 0x10);
}
#endif

#define CONFIG_CORE_NUMBER 2
void disable_core(int index)
{
	uint32_t tmp;
	                                           
	if(index < 0 || index >= CONFIG_CORE_NUMBER)
	  return ;

	/* disable core */
	tmp = readl(CPU_CORE_RESET);
	writel(tmp & ~(1 << (4 + index)), CPU_CORE_RESET);
}

void reset_core(int index, uint32_t hold_base)
{
	uint32_t tmp;
	uint32_t *_core_hold_base = (uint32_t *)hold_base;
	                                           
	if(index < 0 || index >= CONFIG_CORE_NUMBER)
	  return ;

	if(hold_base & 0xff) {
		printf("core cold base must be 256B aligned.\n");
		return ;
	}

	/* set hold base address */
        rtcbit_set("holdbase",  hold_base >> 8);
        rtcbit_set("resetflag", 0x3b);

	/* set hold base init */
	_core_hold_base[index] = 0;

	/* reset core */
	tmp = readl(CPU_CORE_RESET);
	writel(tmp & ~(1 << (4 + index)), CPU_CORE_RESET);
	udelay(20);
	writel(tmp |  (1 << (4 + index)), CPU_CORE_RESET);

	/* wait core ready */
	while(!readl(_core_hold_base + index));

	/* clear hold base */
        rtcbit_set("holdbase",  0);
}

#if 0
extern void mmu_enable(void);
void init_mmu(void) {
	mmu_enable();
}
#endif

void init_mux(int high)
{
	int xom, mux;

	if(high && item_exist("board.case"))
	  mux = item_integer("board.case", 0);
	else if(IROM_IDENTITY != IX_CPUID_820_0 && IROM_IDENTITY != IX_CPUID_820_1)
		return ;
	else {
		/*
		 * initilize IO MUX, iMAPx800 config IO modes by setting
		 * IO MUX register. Totally 32 modes was  supported, IO
		 * state is fixed after IO MUX value is set.
		 *
		 * we will use the following default MUX configurations
		 * for booting state.
		 * 
		 * @mmc0: 22 (MID High End 10" iNAND)
		 * @mmc1: 22 (MID High End 10" iNAND)
		 * @mmc2: 22 (MID High End 10" iNAND)
		 * @nand: 23 (MID High End 10")
		 * @sata: 23 (MID High End 10")
		 * @eprm: 23 (MID High End 10")
		 * @udsk: 23 (MID High End 10")
		 */
		xom = get_xom(1);
		if     (xom > 0 && xom < 4)
			mux = 23;						// mmc
		else if(xom > 3 && xom < 7)
			mux = boot_info() & 0x1f;		// spi
		else if(xom > 19 && xom < 28)
			mux = 11;						// nand: 16
		else
			mux = 27;						// nand: 8
	}

	set_mux(mux);
}

