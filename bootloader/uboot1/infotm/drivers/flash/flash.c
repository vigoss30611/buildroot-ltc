#include <linux/types.h>
#include <common.h>
#include <vstorage.h>
#include <asm/io.h>
#include <bootlist.h>

#define PAGE_BLOCK_LEN 256

struct spi_transfer {
	uint32_t spi_addr;
	uint32_t spi_addition_addr;
	uint32_t spi_erase;
	uint32_t spi_current_len;
	uint32_t spi_addition;
	uint32_t spi_len;
	uint8_t spi_current_addr[4];
	uint8_t spi_command[5];
};

int  spi_cs(int direction)
{
	int gpedat;

	gpedat = readl(GPEDAT);
	if (direction)
		gpedat |= (0x1 << 7);
	else
		gpedat &= ~(0x1 << 7);
	writel(gpedat, GPEDAT);

	return 0;
}

int spi_tranfer(uint8_t *dout, uint8_t *din, uint32_t len, uint8_t *addr, uint32_t addr_len, uint8_t *command, uint32_t com_len)
{
	int i,status;
	int sum = len + addr_len + com_len;
	uint8_t none = 0;

	spi_cs(0);
	
	status = readl(SSI_MST0_BASE_REG_PA +  rSSI_SR_M);
	while (!(status & (0x1 << 2))) {
		status = readl(SSI_MST0_BASE_REG_PA +  rSSI_SR_M);
		printf("........transmit fifo is full\n");
	}
	status = readl(SSI_MST0_BASE_REG_PA +  rSSI_SR_M);
	while(status & (0x1 << 3)) {
		status = readl(SSI_MST0_BASE_REG_PA +  rSSI_SR_M);
		printf("........transmit fifo is not empty\n");
	}

	for (i = 0;i < sum;i++) {
	    	if (com_len > i) {
			writel(*command, SSI_MST0_BASE_REG_PA + rSSI_DR_M);
			command++;
		} else if (addr_len > (i - com_len)) {
		    	writel(*addr, SSI_MST0_BASE_REG_PA + rSSI_DR_M);
			addr++;
		} else if (len > (i - com_len - addr_len)) {
		    	writel(*din, SSI_MST0_BASE_REG_PA + rSSI_DR_M);
			din++;
		}
		status = readl(SSI_MST0_BASE_REG_PA +  rSSI_SR_M);
		while (!(status & (0x1 << 2))) {
			status = readl(SSI_MST0_BASE_REG_PA +  rSSI_SR_M);
			//printf("........transmit1 fifo is full\n");
		}
		status = readl(SSI_MST0_BASE_REG_PA + rSSI_RXFLR_M);
		while (status < 1) {
			status = readl(SSI_MST0_BASE_REG_PA + rSSI_RXFLR_M);
			//printf(".........you get nothing\n");
		}
		if (i >= (com_len + addr_len) && dout != NULL) 
			dout[i - com_len - addr_len] = readl(SSI_MST0_BASE_REG_PA + rSSI_DR_M);
		else
		    	none = readl(SSI_MST0_BASE_REG_PA + rSSI_DR_M);
	}

	spi_cs(1);
	
	return 0;
}

uint32_t spi_addition_func(loff_t offs)
{
	return offs % PAGE_BLOCK_LEN;
}

uint32_t spi_write_get_len(struct spi_transfer *spi) 
{
	spi->spi_addition_addr += spi->spi_current_len;
	if (spi->spi_addition_addr == PAGE_BLOCK_LEN) {
	        spi->spi_addition_addr = 0;
	        spi->spi_addr += spi->spi_current_len;
	}

	spi->spi_addition = PAGE_BLOCK_LEN - spi->spi_addition_addr;
	if (spi->spi_addition < spi->spi_len) {
	        spi->spi_current_len = spi->spi_addition;
	        spi->spi_len -= spi->spi_addition;
	        return 1;
	} else if (spi->spi_len > 0){
	        spi->spi_current_len = spi->spi_len;
	        spi->spi_len -= spi->spi_current_len;
	        return 1;
	} else
	        return 0;
}
		

int flash_vs_read(uint8_t *buf, loff_t offs, int len, uint32_t extra)
{
	struct spi_transfer addr;
	int i;
	addr.spi_addr = offs;
	addr.spi_command[0] = 0x0B;
	
	for (i =0;i<len / 4096;i++) {
		addr.spi_current_addr[0] = (addr.spi_addr >> 16) & 0xff;
		addr.spi_current_addr[1] = (addr.spi_addr >> 8) & 0xff;
		addr.spi_current_addr[2] = addr.spi_addr & 0xff;
		addr.spi_current_addr[3] = 0;

		spi_tranfer(buf, NULL, 4096, addr.spi_current_addr, 4, addr.spi_command, 1);
		addr.spi_addr += 4096;
		buf += 4096;
	}
	if (len % 4096) {
	        addr.spi_current_addr[0] = (addr.spi_addr >> 16) & 0xff;
		addr.spi_current_addr[1] = (addr.spi_addr >> 8) & 0xff;
		addr.spi_current_addr[2] = addr.spi_addr & 0xff;
		addr.spi_current_addr[3] = 0;
		spi_tranfer(buf, NULL, len % 4096, addr.spi_current_addr, 4, addr.spi_command, 1);
	}

#if 0	
	printf("what you read is \n");
	for (i = 0;i < len;i++) {
	        printf("%c",buf[i]);
	        if (i!=0 && i%60 ==0)
	                printf("\n");
	}
	printf("\n");
#endif
	return len;
}

uint32_t spi_flash_start(void)
{
	struct spi_transfer addr;
	addr.spi_command[0] = 0x06;
	spi_tranfer(NULL, NULL, 0, addr.spi_current_addr, 0, addr.spi_command , 1);

	return 0;
}

uint32_t spi_flash_finish(void)
{
	struct spi_transfer addr;
	uint8_t buf[1];
	uint8_t s[1];
	addr.spi_command[0] = 0x05;
	spi_tranfer(buf, s, 1, addr.spi_current_addr, 0, addr.spi_command, 1);

	return buf[0];
}

uint32_t spi_flash_enable_write(void)
{
	int tmp;
	
	spi_flash_start();
	tmp = spi_flash_finish();
	while (!(tmp & (0x1 << 1))) {
			tmp = spi_flash_finish();
	        printf(".......you cannot start to write\n");
	}
	return 0;
}

uint32_t spi_flash_finish_write(void)
{
	int tmp;
	uint64_t to,tn;

	tmp = spi_flash_finish();
	to = get_ticks();
	while (tmp & (0x1)) {
			tn = get_ticks();
			if ((tn - to) > 5000000) {
				printf("spi works wrong\n");
				return -1;
			}
	        tmp = spi_flash_finish();
	}
	return 0;
}

uint32_t spi_flash_enable_register(void)
{
		struct spi_transfer addr;
		addr.spi_command[0] = 0x50;
		spi_tranfer(NULL, NULL, 0, addr.spi_current_addr, 0, addr.spi_command , 1);
		
		return 0;
}

uint32_t spi_flash_zero(void)
{
		struct spi_transfer addr;
		uint8_t s[1] = {0};

		spi_flash_enable_write();	
		addr.spi_command[0] = 0x01;
		spi_tranfer(NULL, s, 1, addr.spi_current_addr, 0, addr.spi_command, 1);
		spi_flash_finish_write();

		spi_flash_enable_register();
		addr.spi_command[0] = 0x01;
		spi_tranfer(NULL, s, 1, addr.spi_current_addr, 0, addr.spi_command, 1);
		spi_flash_finish_write();

		return 0;
}

int spi_flash_page(void)
{
	uint16_t info;

	boot_info("flash", &info);
	return (info & 0x1);
}

int flash_vs_write(uint8_t *buf, loff_t offs, int len, uint32_t extra)
{
	struct spi_transfer addr;
	int i;
	addr.spi_addr = offs;
	addr.spi_command[0] = 0x02;
	addr.spi_len = len;
	
	if (!spi_flash_page()) {
		addr.spi_addition_addr = spi_addition_func(offs);
		addr.spi_current_len = 0;
		while (spi_write_get_len(&addr)) {
			spi_flash_enable_write();
			
			addr.spi_current_addr[0] = (addr.spi_addr >> 16) & 0xff;
			addr.spi_current_addr[1] = (addr.spi_addr >> 8) & 0xff;
			addr.spi_current_addr[2] = addr.spi_addr & 0xff;
	//		printf("addr.spi_current_addr[0] is %d, addr.spi_current_addr[1] is %d, addr.spi_current_addr[2] is %d,addr.spi_current_len is %d\n",
	//				addr.spi_current_addr[0], addr.spi_current_addr[1], addr.spi_current_addr[2], addr.spi_current_len);

			spi_tranfer(NULL, buf, addr.spi_current_len, addr.spi_current_addr, 3, addr.spi_command , 1);

			spi_flash_finish_write();
			buf += addr.spi_current_len;
		}
    } else {
		for (i = 0;i < len;i++) {
			spi_flash_enable_write();

			addr.spi_current_addr[0] = (addr.spi_addr >> 16) & 0xff;
			addr.spi_current_addr[1] = (addr.spi_addr >> 8) & 0xff;
			addr.spi_current_addr[2] = addr.spi_addr & 0xff;

			spi_tranfer(NULL, buf, 1, addr.spi_current_addr, 3, addr.spi_command , 1);
			
			spi_flash_finish_write();
			buf += 1;
			addr.spi_addr += 1;
		}
	}
	
	return len;
}

int flash_vs_erase(uint64_t offs, size_t len)
{
	struct spi_transfer addr;

	addr.spi_command[0] = 0xC7;
	spi_flash_enable_write();
	spi_tranfer(NULL, NULL, 0, addr.spi_current_addr, 0, addr.spi_command , 1);
	spi_flash_finish_write();

	return 0;
}

int spi_init(void)
{
	int gpecon;
	
	gpecon = readl(GPECON);
	gpecon &= ~((0x3 << 14) | (0x3 << 12) | (0x3 << 10) | (0x3 << 8));
	gpecon |= ((0x1 <<14) | (0x2 << 12) | (0x2 << 10) | (0x2 << 8));
	writel(gpecon,GPECON);

	writel(0, SSI_MST0_BASE_REG_PA + rSSI_ENR_M);
	writel(0, SSI_MST0_BASE_REG_PA + rSSI_IMR_M);
	writel(30, SSI_MST0_BASE_REG_PA + rSSI_BAUDR_M);
	writel(1, SSI_MST0_BASE_REG_PA + rSSI_TXFTLR_M);
	writel(0, SSI_MST0_BASE_REG_PA + rSSI_RXFTLR_M);
	writel(((0x0 << 8) | (0x0 << 7) | (0x0 << 6) | (0x0 << 4) | (0x7 << 0)), SSI_MST0_BASE_REG_PA + rSSI_CTLR0_M);
	writel(1, SSI_MST0_BASE_REG_PA + rSSI_SER_M);
	writel(1, SSI_MST0_BASE_REG_PA + rSSI_ENR_M);

	return 0;
}

int flash_vs_reset(void)
{
    	spi_init();
    	spi_flash_zero();
	return 0;
}

int flash_vs_align(void)
{
	return 256;
}

