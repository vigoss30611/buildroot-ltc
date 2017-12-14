#include <common.h>
#include <asm/io.h>

#define CONFIG_SRAM_BASE 0x90000000
#define CONFIG_NOR_BLOCK_SIZE 0x8000
#define CONFIG_NOR_SECTOR_SIZE 0x1000
#define CONFIG_NOR_BUF_PROG_MAX 16
#define CONFIG_NOR_BW_SHIFT		1		

enum nor_erase_type {
	NOR_ERASE_SECTOR = 0,
	NOR_ERASE_BLOCK,
	NOR_ERASE_CHIP,
};

/* FIXME: This two command may need cache manage!!!
 */
static void inline nor_cmd(uint32_t addr, uint16_t val)
{
	uint32_t b_addr = (CONFIG_SRAM_BASE + (addr << CONFIG_NOR_BW_SHIFT));
	*(volatile uint16_t *)b_addr = val;
//	clean_cache(b_addr, b_addr + 2);
}

uint16_t nor_get(uint32_t addr)
{
	uint32_t b_addr = (CONFIG_SRAM_BASE + (addr << CONFIG_NOR_BW_SHIFT));
//	inv_cache(b_addr, b_addr + 2);
	return *(volatile uint16_t *)b_addr;
}

#if 0
static void inline nor_wait(uint32_t addr)
{
	int a = 0;

	return ;
	/* wait erase finish */
	for(;;)
	{
		uint32_t dq6_1 = nor_get(addr) & 0x40;
		uint32_t dq6_2 = nor_get(addr) & 0x40;

		if(dq6_1 == dq6_2)
		  if(nor_get(addr) & 0x80)
			break;

		a++;
	}

	if(a)
		printf("waited %d cycles...\n", a);
}
#endif

static int nor_bit_toggling(uint32_t addr)
{
	uint16_t q6, t = 4;

	/* check if Q6 is toggling */
	q6 = nor_get(addr) & 0x40;
	while(t--)
	  if((nor_get(addr) & 0x40) != q6)
		/* bit is toggling */
		return 1;

	return 0;
}

static int nor_erase(uint32_t addr, enum nor_erase_type type)
{
	nor_cmd(0x5555, 0xaa);
	nor_cmd(0x2aaa, 0x55);
	nor_cmd(0x5555, 0x80);
	nor_cmd(0x5555, 0xaa);
	nor_cmd(0x2aaa, 0x55);

	switch(type)
	{
		case NOR_ERASE_SECTOR:
			nor_cmd(addr, 0x50);
			break;
		case NOR_ERASE_BLOCK:
			nor_cmd(addr, 0x30);
			break;
		case NOR_ERASE_CHIP:
			nor_cmd(0x5555, 0x10);
			break;
	}

	printf("Erasing(type:%d): 0x%08x ...", type, addr);
	/* wait erase finish */
	while(nor_bit_toggling(addr));
	printf("OK.\n");

	return 0;
}

static int nor_word_prog(uint32_t addr, uint16_t word)
{
//	printf("prog. ad=%x, wd=%x\n", addr, (uint32_t)word);

#if 0
	if(addr >= 0x4000)
	  printf("prog. ad=%x, wd=%x\n", addr, (uint32_t)word);
#endif

	nor_cmd(0x5555, 0xaa);
	nor_cmd(0x2aaa, 0x55);
	nor_cmd(0x5555, 0xa0);
	nor_cmd(addr, word);

	while(nor_bit_toggling(addr));

	return 0;
}

#if 0
static int nor_buf_prog(uint32_t addr, uint16_t *buf, uint16_t len)
{
	int i;
	uint32_t ba = addr & (CONFIG_NOR_BLOCK_SIZE - 1);

	if(len > CONFIG_NOR_BUF_PROG_MAX)
	{
		printf("%s can only write %d words at a time\n", __func__,
		   CONFIG_NOR_BUF_PROG_MAX);
		return -1;
	}

	nor_cmd(0x555, 0xaa);
	nor_cmd(0x2aa, 0x55);
	nor_cmd(ba, 0x25);
	nor_cmd(ba, len);

	for(i = 0; i < len; i++)
	  nor_cmd(addr, buf[i]);

	nor_cmd(ba, 0x29);

	/* wait program finish */
	nor_wait(addr + (len << 1));

	return 0;
}
#endif

int nor_id(int entry)
{
	nor_cmd(0x5555, 0xaa);
	nor_cmd(0x2aaa, 0x55);

	if(entry)
	  nor_cmd(0x5555, 0x90);
	else
	  nor_cmd(0x5555, 0xf0);

	udelay(2);
	return 0;
}

int nor_erase_chip(void)
{
	return nor_erase(0, NOR_ERASE_CHIP);
}

int nor_erase_block(uint32_t addr)
{
	/* turn byte address to word address */
	addr >>= CONFIG_NOR_BW_SHIFT;
	return nor_erase(addr, NOR_ERASE_BLOCK);
}


int nor_program(uint32_t addr, uint8_t *buf, uint32_t len)
{
	uint32_t pos;

	/* bic the high bit of addr, thus both
	 * addr to cpu (0x10000000) or addr to 
	 * nor (0x00000000) is usable.
	 */
	addr &= ~(0xf << 28);

#if 0
	if(addr & (CONFIG_NOR_BLOCK_SIZE - 1))
	{
		printf("%s do not handle not aligned address.\n", __func__);
		return -1;
	}
#endif

	len = (len + 1) & ~1;

	/* if SST6401 turn this, use sector erase */
#if 0
	/* erase blocks before write */
	for(pos = addr; pos < addr + len; pos += CONFIG_NOR_SECTOR_SIZE)
	  nor_erase((pos >> CONFIG_NOR_BW_SHIFT), NOR_ERASE_SECTOR);
#else
//	nor_erase_chip();
#endif

	printf("Norflash program: addr=%x, buf=%p, len=%x\n", addr, buf, len);
	/* write data to nor */
	for(pos = 0; pos < len; pos += 2)
	  nor_word_prog(((addr + pos) >> CONFIG_NOR_BW_SHIFT), *(uint16_t *)(buf + pos));

	return 0;
}

void nor_hw_init(void)
{
	printf("Initing nor flash...\n");

#if 0
	flush_cache(CONFIG_SYS_SDRAM_BASE, CONFIG_SYS_SDRAM_END);

	/* turn off I/D-cache */
	icache_disable();
	dcache_disable();
#endif
	writel(0xaa, GPHCON);
	writel(0xaaaaaaa, GPICON);
	writel(0x2aaaa, GPJCON);

	nor_id(1);
	printf("Maf: %x\n", (uint32_t)nor_get(0));
	printf("Did: %x\n", (uint32_t)nor_get(1));
	nor_id(0);
	return ;
}

