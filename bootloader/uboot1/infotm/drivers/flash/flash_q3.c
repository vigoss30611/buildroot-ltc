#include <linux/types.h>
#include <common.h>
#include <vstorage.h>
#include <storage.h>
#include <asm/io.h>
#include <bootlist.h>
#include <lowlevel_api.h>
#include <linux/byteorder/swab.h>
#include <efuse.h>
#include <cpuid.h>
#include <ssp.h>

#define SPI_CLK 28000000//3000000

#define SPI_TFIFO_EMPTY  (1 << 0)
#define SPI_TFIFO_NFULL  (1 << 1)
#define SPI_RFIFO_NEMPTY (1 << 2)
#define SPI_BUSY         (1 << 4)
#define FLASH_CMD_RDSR  0x5

#define FLASH_READ_BLOCK 0x1000

#define FLASH_MOD_POLLREDY 0
#define FLASH_MOD_POLLWREN 1
#define FLASH_DUMMY   FLASH_CMD_RDSR

struct spi_transfer {
	uint8_t *in;
	uint8_t *out;
	int32_t len;
};

static int spi_log_state = 0;
#define spi_debug(format,args...)  \
	if(spi_log_state)\
		printf(format,##args)

static int spi_init_clk(void)
{
	int pclk = module_get_clock(APB_CLK_BASE),
		i, j;

	for(i = 2; i < 255; i += 2)
	  for(j = 0; j < 256; j++)
		if(pclk / (i * (j + 1)) <= SPI_CLK)
		  goto calc_finish;

calc_finish:
	if(i >= 255 || j >= 256) {
		printf("spi: calc clock failed(%d, %d), use default.\n", i, j);
		i = 2; j = 39;
	}

	printf("PCLK: %d, PS: %d, SCR: %d, Fout: %d\n",
				pclk, i, j, pclk / (i * (j + 1)));

	writel(0, SSP_CR1_0);
	writel(0, SSP_IMSC_0);
	writel(i, SSP_CPSR_0);
	writel((j << 8) | (3 << 6) | (0 << 4) | (7 << 0), SSP_CR0_0);
	writel((1 << 3) | (0 << 2) | (1 << 1) | (0 << 0), SSP_CR1_0);

	return 0;
}

static int spi_init_pads(void) {
	int xom = get_xom(1), index;

	//if(boot_device() != DEV_FLASH)
	  //printf("warning: not spi boot\n");

//	index = (xom == 4) ? 74:
//		((xom == 6)? 27: 121);
	if (IROM_IDENTITY==IX_CPUID_Q3F || IROM_IDENTITY==IX_CPUID_APOLLO3) {
		index = 70;
		printf("xom=%d\n", xom);

		pads_chmod(PADSRANGE(index, (index + 3)), PADS_MODE_CTRL, 0);
		pads_chmod(80, PADS_MODE_CTRL, 0);
	} else {
		index = 92;
		printf("xom=%d\n", xom);

		pads_chmod(PADSRANGE(index, (index + 3)), PADS_MODE_CTRL, 0);
		if(ecfg_check_flag(ECFG_ENABLE_PULL_SPI)) {
			pads_pull(PADSRANGE(index, (index + 3)), 1, 1);
			pads_pull(index + 2, 1, 0);		/* clk */
		}
	}

	return 0;
}

static void spi_fifo_clear(void)
{
	while(readl(SSP_SR_0) & SPI_BUSY);
	while(readl(SSP_SR_0) & SPI_RFIFO_NEMPTY) {
		readl(SSP_DR_0);
//		printf("c:0x%02x\n", readl(SSP_DR_0));
	}
}
static void spi_cs(int value)
{
	if (IROM_IDENTITY==IX_CPUID_Q3F || IROM_IDENTITY==IX_CPUID_APOLLO3)
		*(unsigned int *)(GPIO_BASE_ADDR+74*0x40+0x4) =!!value;
	else
		*(unsigned int *)(0x20f00000+95*0x40+0x4) =!!value;
}
/* exchange support:
 *		@tx
 *		@tx rx
 *		@tx tx
 * the first tx is limited to bellow 8 bytes
 */
static void spi_exchange(struct spi_transfer *td, int count)
{
	int32_t skip = 0, len, dummy = 0, stat;
	uint8_t *p, ex[8], exlen;

	spi_cs(0);
#if 0
	int a;
	for(a = 0; a < count; a++)
	  printf("spi: exc(%d): in=0x%p, out=0x%p, len=0x%x\n",
				  a, td[a].in, td[a].out, td[a].len);
#endif
	len = skip = td->len;
	p     = td->out;

	if(count > 1) {
		memcpy(ex, td->out, len);
		td++;
		exlen = min(8, len + td->len);

		if(td->out) {
			memcpy(ex + len, td->out, exlen - len);
			p     = td->out + exlen - len;
			len   = td->len + len - exlen;
			skip += td->len;
		} else {
			p = td->in;
			len   = td->len;
			dummy = td->len + len - exlen;
		}

		/* fill tfifo to enable pipe line */
		for(stat = 0; stat < exlen; stat++) {
			//printf("we: %02x\n", ex[stat]);
		  writel(ex[stat], SSP_DR_0);
		}
	}

	/* ok, imediately start the loop */
__loop:
	stat = readl(SSP_RIS_0);
	//printf("spi:stat %x \n", stat);
	if(stat & (1 << 2)) {
		if(skip > 3) { skip -= 4;
			readl(SSP_DR_0);
			readl(SSP_DR_0);
			readl(SSP_DR_0);
			readl(SSP_DR_0);
//			printf("rd4\n");
		} else if(skip) {
//			printf("rs:0x%02x\n", readl(SSP_DR_0));
			while(skip) { skip--;
//				printf("rs\n");
				readl(SSP_DR_0); }
		} else {

			if(!td->in)
			  printf("spi: unexpected dummy\n");
			else {
				if(len > 3) { len -= 4;
					*p++ = readl(SSP_DR_0);
					*p++ = readl(SSP_DR_0);
					*p++ = readl(SSP_DR_0);
					*p++ = readl(SSP_DR_0);
					//printf("r4: %02x %02x %02x %02x\n", *(p - 4), *(p - 3), *(p - 2), *(p - 1));
				} else if(len) {
					while(len) { len--;
						*p++ = readl(SSP_DR_0);
//						printf("r%02x\n", *(p - 1));
					}
//				printf("r:0x%02x\n", *(p-1));
				}
				if(!len) goto __finish;
				//					printf("r:0x%02x\n", a);
			}
		}
	}

	if(stat & (1 << 3)) {
		if(td->out) {
			if(len > 3) { len -= 4;
				writel(*p++, SSP_DR_0);
				writel(*p++, SSP_DR_0);
				writel(*p++, SSP_DR_0);
				writel(*p++, SSP_DR_0);
							//printf("w4: %02x %02x %02x %02x\n", *(p - 4), *(p - 3), *(p - 2), *(p - 1));
			} else if(len) {
				while(len) { len--;
									//printf("2%02x\n", *p);
					writel(*p++, SSP_DR_0); }
			}

			if(td->out && !len)
			  goto __finish;
		} else if(dummy > 3) { dummy -= 4;
//			printf("wd4\n");
			writel(FLASH_DUMMY, SSP_DR_0);
			writel(FLASH_DUMMY, SSP_DR_0);
			writel(FLASH_DUMMY, SSP_DR_0);
			writel(FLASH_DUMMY, SSP_DR_0);
		} else if(dummy) {
			while(dummy) { dummy--;
//				printf("wd\n");
				writel(FLASH_DUMMY, SSP_DR_0); }
		}
	}

	/* r time out */
	if(stat & (1 << 1)) {
		while(skip) { skip--;
			if(readl(SSP_SR_0) & SPI_RFIFO_NEMPTY) {
//				printf("rts\n");
				readl(SSP_DR_0);
			}
			else
			  printf("spi: data missed (s%d)\n", skip);
		}

		while(td->in && len) { len--;
			if(readl(SSP_SR_0) & SPI_RFIFO_NEMPTY) {
				*p++ = readl(SSP_DR_0);
//				printf("rt%02x\n", *(p-1));
			}
			else
			  printf("spi: data missed (r%d)\n", len);
		}

		/* clear SSPRTINTR */
		writel(3, SSP_ICR_0);
		goto __finish;
	}

	goto __loop;

__finish:
    //spi_cs(1);
	spi_fifo_clear();
	spi_cs(1);
}

static void spi_cs_init()
{
	if (IROM_IDENTITY==IX_CPUID_Q3F || IROM_IDENTITY==IX_CPUID_APOLLO3) {
	    *((unsigned int *)(0x2D009004 + 9*4)) |=1<<2;//GPIO function
	    *(unsigned int *)(GPIO_BASE_ADDR+74*0x40+0x8) =1;//direct:output
	    *(unsigned int *)(GPIO_BASE_ADDR+74*0x40+0x4) =1;//high
	} else {
	    //for Q3
	    *((unsigned int *)(0x21e0906c+11*4)) |=1<<7;//GPIO function
	    *(unsigned int *)(0x20f00000+95*0x40+0x8) =1;//direct:output
	    *(unsigned int *)(0x20f00000+95*0x40+0x4) =1;//high
	}
}

int otf_module_init()
	{
	module_enable(SSP_SYSM_ADDR);
	spi_init_clk();
	spi_init_pads();
	spi_fifo_clear();
       spi_cs_init();

	return 0;
}

int32_t spi_read_write_data(spi_protl_set_p  protocal_set)
{
    uint8_t tx[8];
    struct spi_transfer td[2];
    int i;

 	if (protocal_set->cmd_type == CMD_ONLY) {

		 for(i = 0; i < protocal_set->cmd_count; i++ )
		 {
		 	tx[i] = protocal_set->commands[i];
		 }
	        for(i = 0; i < protocal_set->addr_count; i++ )
		 {
		 	tx[i + protocal_set->cmd_count] = protocal_set->addr[i];
		 }

		td[0].in = NULL;
		td[0].out = &tx[0];
		td[0].len = protocal_set->cmd_count + protocal_set->addr_count;

		 spi_exchange(&td[0], 1);
	}
	else if( protocal_set->cmd_type == CMD_READ_DATA )
	{
		 for(i = 0; i < protocal_set->cmd_count; i++ )
		 {
		 	tx[i] = protocal_set->commands[i];
		 }
	        for(i = 0; i < protocal_set->addr_count; i++ )
		 {
		 	tx[i + protocal_set->cmd_count] = protocal_set->addr[i];
		 }

		td[0].in = NULL;
		td[0].out = &tx[0];
		td[0].len = protocal_set->cmd_count + protocal_set->addr_count + protocal_set->dummy_count;//

		td[1].in = protocal_set->read_buf;
		td[1].out = NULL;
		td[1].len = protocal_set->read_count;
		spi_exchange(td, ARRAY_SIZE(td));
		protocal_set->read_buf += protocal_set->read_count;//
	}
	else if ( protocal_set->cmd_type == CMD_WRITE_DATA )
	{
		 for(i = 0; i < protocal_set->cmd_count; i++ )
		 {
		 	tx[i] = protocal_set->commands[i];
		 }
	        for(i = 0; i < protocal_set->addr_count; i++ )
		 {
		 	tx[i + protocal_set->cmd_count] = protocal_set->addr[i];
		 }

		td[0].in = NULL;
		td[0].out = &tx[0];
		td[0].len = protocal_set->cmd_count + protocal_set->addr_count;

		td[1].in = NULL;
		td[1].out = protocal_set->write_buf;
		td[1].len = protocal_set->write_count;

		spi_exchange(td, ARRAY_SIZE(td));
		protocal_set->write_buf += protocal_set->write_count;
	}
	return 0;
}
