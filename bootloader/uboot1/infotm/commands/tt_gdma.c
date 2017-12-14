#include <common.h>
#include <command.h>
#include <gdma.h>
#include <malloc.h>
#include <tt.h>
#include <div64.h>

#define gdma_debug(x...) printf("gdma: " x)
#define DEF_LEN         0x19000

int error = 0;


int dma_compare_result(uint32_t src_addr, uint32_t dst_addr, uint32_t data_len)
{   
	uint32_t tmp, res=1;
	uint8_t* pSrc; 
	uint8_t* pDst;

	tmp = data_len;
	pSrc = (uint8_t *)src_addr;
	pDst = (uint8_t *)dst_addr;
	while(tmp)
	{   
		if(*pSrc++ != *pDst++) {
			res = 0;
			printf("Leave %d for compare\n", tmp);
			break;
		}
		tmp--;
	}       
	return res;
}           

int test_gdma(uint32_t src_addr, uint32_t dst_addr, uint32_t len, uint32_t align)
{
    int to, vs, vs1, i;
	uint64_t time = 1;

    dma_clear_result((char *)dst_addr, len);
    time = get_ticks();
    gdma_memcpy_align ((uint8_t *)src_addr, (uint8_t *)dst_addr, len, align);
    time = get_ticks() - time;
    if (time) {
		if(lldiv(time, 1000))
		  vs = (len >> 10) * 1000 / (uint32_t)lldiv(time, 1000);
		else
		  vs = (len >> 10) * 1000000 / (uint32_t)time;
	} else
	vs = -1;
    if (dma_compare_result (src_addr, dst_addr, len)) {
	gdma_debug("correct, Vm is %d KB/s\n", vs);
	//printf("align is %d\n", align);
	if (align) {
	    to = get_timer(0);
	    for (i = 0;i < (len / align);i++) {
		memcpy((char *)src_addr, (char *)dst_addr, align);
		src_addr += align;
		dst_addr += align;
	    }
	    to = get_timer(to);
	    vs1 = (len >> 10) * 1000 / to;
	    gdma_debug("memcpy correct, Vm is %d KB/s, align is %d\n", vs1, align);
	    printf("vs is %d, vs1 is %d\n", vs, vs1);
	    if (vs > vs1) {
		printf("vs > vs1\n");
		return -1;
	    }
	}
	return 0;
    } else {
	gdma_debug("memcpy failed, Vm is %d KB/s\n", vs);
	error++;
	return -1;
    }
}

int do_tt_gdma(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
    uint32_t src = 0, dst = 0, len = DEF_LEN, length = 0;
    int to, i, all;
	uint32_t data;

    gdma_debug ("Start to test gdma:\n");
    switch(argc) {
        case 2:
#if 0
			for(i = 1;i < (DEF_LEN + 1);i++) {	
				len = (DEF_LEN / i) * i;
				if (test_gdma(0x40000000, 0x42000000, len, i) < 0) {
				  return -1;
				}
			}
#else
			i = simple_strtoul(argv[1], NULL, 10);
			len = (DEF_LEN / i) * i;
			test_gdma(0x40000000, 0x42000000, len, i);
#endif
			return 0;
        case 1:
            break;
        case 4:
	    printf("hfashfahlf\n");
            src = simple_strtoul(argv[1], NULL, 16);
            dst = simple_strtoul(argv[2], NULL, 16);
            len = simple_strtoul(argv[3], NULL, 16);
            test_gdma(src, dst, len, 0);
            return 0;
        default:
            gdma_debug("Usage:\n%s\n", cmdtp->help);
            break;
    }
    to = get_timer(0);
    for (i = 0, all = 0;;i++) {
		length = tt_rnd_get_int(0, len);
		//data = tt_rnd_get_data(&len1, length, length);
		data = 0x40000000 + tt_rnd_get_int(0, 0x100000) * 8;        
		dst = 0x4e000000 + tt_rnd_get_int(0, 0x100000) * 8;
		printf("source addr is %x, dst addr is %x, len is %x\n", data, dst, length);
        if (!test_gdma(data, dst, length, 0))
            all += length;
        else
            break;
        if(tstc() && (getc() == 0x3)) 
            break;
    }
    to = get_timer(to);
	printf("Total transfer %d, but have %d error\n", i++, error);
    printf("%d MB data tested in %d minutes.\n", (int)(all >> 20), to / 60000);
    return 0;
}

U_BOOT_CMD(
    gdma, 4, 1, do_tt_gdma,
    "memory copy",
    "gdma src_addr dst_addr length\n"
);
