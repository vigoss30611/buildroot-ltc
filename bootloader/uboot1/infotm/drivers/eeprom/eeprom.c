#include <linux/types.h>
#include <common.h>
#include <vstorage.h>
#include "imapx_iic.h"

#define IIC_CHANNAL 0 
#define IIC_MODE 1

#define PAGE_BLOCK_LEN 16

#define loff_t long long
int iic_addr_const = 0;

struct iic_transfer {
	uint16_t iic_addr;
    uint16_t iic_word_addr;
    uint32_t iic_addition; 
	uint32_t iic_len;
	uint32_t iic_current_len;
	uint16_t flag;
	uint8_t iic_word_current_addr[2];
};

uint16_t iic_address_num(void)
{
	iic_addr_const = (boot_info() & 0x7f) << 1;

	return 1;
}

uint16_t iic_addr_func(loff_t offs)
{
	if (iic_address_num())
		return iic_addr_const;
	else
		return iic_addr_const | ((offs / 256) << 1);
}

uint16_t iic_word_addr_func(loff_t offs)
{
	if (iic_address_num())
		return offs;
	else
		return offs % 256;
}

uint16_t iic_write_get_address(struct iic_transfer *iic) {
	iic->iic_word_addr += iic->iic_current_len;
	if (!iic_address_num() && (iic->iic_word_addr == 256)) {
		iic->iic_word_addr = 0;
		iic->iic_addr += 0x2;
		iic->flag = 1;
	}
	iic->iic_addition = PAGE_BLOCK_LEN - iic->iic_word_addr % PAGE_BLOCK_LEN;
	if (iic->iic_addition < iic->iic_len) {
		iic->iic_current_len = iic->iic_addition;
		iic->iic_len -= iic->iic_addition;
		return 1;
	} else if (iic->iic_len > 0){
		iic->iic_current_len = iic->iic_len;
		iic->iic_len -= iic->iic_current_len;
		return 1;
	} else
		return 0;

}

uint16_t iic_read_get_address(struct iic_transfer *iic) {
	iic->iic_word_addr += iic->iic_current_len;
	if (!iic_address_num() && (iic->iic_word_addr == 256)) {
	        iic->iic_word_addr = 0;
	        iic->iic_addr += 0x2;
	        iic->flag = 1;
	}
	if (iic_address_num()) {
		if (iic->iic_len > 0) {
			iic->iic_current_len = iic->iic_len;
			iic->iic_len -= iic->iic_current_len;
			return 1;
		} else
			return 0;
	} else {
		iic->iic_addition = 256 - iic->iic_word_addr;
		if (iic->iic_addition < iic->iic_len) {
			iic->iic_current_len = iic->iic_addition;
			iic->iic_len -= iic->iic_current_len;
			iic->flag = 1;
			return 1;
		} else if (iic->iic_len > 0){
			iic->iic_current_len = iic->iic_len;
			iic->iic_len -= iic->iic_current_len;
			return 1;
		} else
			return 0;
	}
}

int e2p_vs_read(uint8_t *buf, loff_t offs, int len, int extra)
{
	//uint16_t i;
	__u32 to, tn;
	struct iic_transfer addr;
	//uint8_t *s = buf;
	int word_addr_num;
	
	addr.iic_addr = iic_addr_func(offs);
	addr.iic_word_addr = iic_word_addr_func(offs);
	addr.flag = 1;
	addr.iic_len = len;
	addr.iic_current_len = 0;

	while(iic_read_get_address(&addr)) {
		if (iic_address_num()) {
			addr.iic_word_current_addr[0] = addr.iic_word_addr >> 8;
			addr.iic_word_current_addr[1] = addr.iic_word_addr & 0xff;
			word_addr_num = 2;
		} else {
			addr.iic_word_current_addr[0] = addr.iic_word_addr & 0xff;
			word_addr_num = 1;
		}

		to = get_ticks();
		if (addr.flag) {
			addr.flag = 0;
			if(iic_init(IIC_CHANNAL, IIC_MODE, addr.iic_addr, 0, 1, 0)== FALSE)
				return 0;
		}
		while(iic_read_func(IIC_CHANNAL,addr.iic_word_current_addr,word_addr_num,buf,addr.iic_current_len) == 0)
		{       
		        tn = get_ticks();
		        if(tn > to + 500000) break;
		}
		buf += addr.iic_current_len;
	}

#if 0
	printf("what you read is \n");
	for (i = 0;i < len;i++) {
		printf("%c",s[i]);
		if (i!=0 && i%60 ==0)
			printf("\n");
	}
	printf("\n");
#endif
	return len; 
}

int e2p_vs_write(uint8_t *buf, loff_t offs, int len, int extra)
{
	uint16_t i = 0;
	__u32 to, tn;
	int word_addr_num = 0;
	struct iic_transfer addr;
	
	addr.iic_addr = iic_addr_func(offs);
	addr.iic_word_addr = iic_word_addr_func(offs);
	addr.iic_len = len;
	addr.iic_current_len = 0;
	addr.flag = 1;
	while(iic_write_get_address(&addr)) {
		if (iic_address_num()) {
			addr.iic_word_current_addr[0] = addr.iic_word_addr >> 8;
			addr.iic_word_current_addr[1] = addr.iic_word_addr & 0xff;
			word_addr_num = 2;
		} else {
			addr.iic_word_current_addr[0] = addr.iic_word_addr & 0xff;
			printf("......%x\n", addr.iic_word_current_addr[0]);
			word_addr_num = 1;
		}

		to = get_ticks();
		if(addr.flag) {
			addr.flag = 0;
			if(iic_init(IIC_CHANNAL,IIC_MODE,addr.iic_addr,0,1,0)== FALSE)
				return 0;
		}
		while(iicreg_write(IIC_CHANNAL,addr.iic_word_current_addr,word_addr_num,buf,addr.iic_current_len, 1) == 0)
		{
		        tn = get_ticks();
		        if(tn > to + 500000) {
						printf("error\n");
				i += addr.iic_current_len;
		                break;
		        }
		}
		buf += addr.iic_current_len;
	}

	return len - i;
}

int e2p_vs_align(void)
{
	return 16;
}

int e2p_vs_reset(void)
{
	return 0;
}

