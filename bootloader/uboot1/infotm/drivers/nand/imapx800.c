
#include <common.h>
#include <nand.h>
#include <asm/errno.h>
#include <asm/io.h>
#include <lowlevel_api.h>
#include <efuse.h>
/* TODO */


/* 0: (0,  2]
 * 1: (2,  4]
 * 2: (4,  8]
 * 3: (8,  16]
 * 4: (16, 20]
 * 5: (20, 24]
 * 6: (24, 32]
 * 7: (32, 40]
 * 8: (40, 127]
 * 9: (unfixed)
 * 10:(normal)
 */
static int mecc_stat[12];

/* 0: (0, 4]
 * 1: (4, 8]
 * 2: (unfixed)
 * 3: (normal)
 */
static int secc_stat[4];

static void ecc_add_stat(int type, int bits)
{
	int mecc_seg[9] = {2, 4, 8, 16, 20, 24, 32, 40, 127};
	int secc_seg[2] = {4, 8}, i;

	if(type) {
		/* secc */
		if(bits < 0) secc_stat[2]++;
		else if(bits == 0) secc_stat[3]++;
		else if(bits > 8)
		  printf("x800 SHOULD NOT fix so much secc bits: %d\n", bits);
		else {
			for (i = 0; i < 2; i++)
			  if(bits <= secc_seg[i]) break;
			secc_stat[i] ++;
		}
	} else {
		/* mecc */
		if(bits < 0) mecc_stat[9]++;
		else if(bits == 0) mecc_stat[10]++;
		else if(bits > 127)
		  printf("x800 SHOULD NOT fix so much mecc bits: %d\n", bits);
		else {
			for (i = 0; i < 9; i++)
			  if(bits <= mecc_seg[i]) break;
			mecc_stat[i] ++;
		}
	}
}

void  ecc_print_stat(void)
{
	int a = 0, i;

	for(i = 0; i < 11; i++) a += mecc_stat[i];

#if 0
	if(a < 100) {
		printf("no statistics yet.\n");
		return ;
	}
#endif

	printf("\nglobal MECC statistics\n");
	printf("----------------------------\n");
	printf("[ normal]: %d\n", mecc_stat[10]);
	printf("(0,    2]: %d\n", mecc_stat[0]);
	printf("(2,    4]: %d\n", mecc_stat[1]);
	printf("(4,    8]: %d\n", mecc_stat[2]);
	printf("(8,   16]: %d\n", mecc_stat[3]);
	printf("(16,  20]: %d\n", mecc_stat[4]);
	printf("(20,  24]: %d\n", mecc_stat[5]);
	printf("(24,  32]: %d\n", mecc_stat[6]);
	printf("(32,  40]: %d\n", mecc_stat[7]);
	printf("(40, 127]: %d\n", mecc_stat[8]);
	printf("[unfixed]: %d\n", mecc_stat[9]);
	printf("----------------------------\n");
	printf("total: %d cases, %d MB data, %d%% bit flip percent\n",
				a, a >> 10, (a - mecc_stat[10]) / (a / 100));


	a = 0;
	for(i = 0; i < 4; i++) a += secc_stat[i];
	if(a < 100) return;

	printf("\n\nglobal SECC statistics\n");
	printf("----------------------------\n");
	printf("[ normal]: %d\n", secc_stat[3]);
	printf("(0,    4]: %d\n", secc_stat[0]);
	printf("(4,    8]: %d\n", secc_stat[1]);
	printf("[unfixed]: %d\n", secc_stat[2]);
	printf("----------------------------\n");
	printf("total: %d cases, %d%% bit flip percent\n",
				a, (a - secc_stat[3]) / (a / 100));
}

#if 0
int  nf2_active_async (int asyn_timemode) {

    	int i;
    	int time_tick_start = 0;
    	int time_tick_cur = 0;
    	int tmp = 0;
    	int ret = 0;

    	writel(0xf, NF2CSR);	// cs0 invalid
    	for (i=0; i<0x100; i++) ;
    	writel(0x10, NF2PCLKS);	// enable clock stop at standby
    	for (i=0; i<0x100; i++) ;

    	writel(0x0, NF2PSYNC);    // set host to async mode

    	nf2_asyn_reset(0);   // async reset

    	//step : program trx-afifo
    	nf2_soft_reset(1);
    	writel(0x1, NF2FFCE);     // allow write trx-afifo

    	trx_afifo_ncmd(0x0, 0x0, 0xef);    // set features CMD
    	trx_afifo_ncmd(0x0, 0x1, 0x01);    // set features ADDR 01H
    	trx_afifo_ncmd(0x0, 0x2, asyn_timemode);    // set features P1
    	trx_afifo_ncmd(0x0, 0x2, 0x00);    // set features P2
    	trx_afifo_ncmd(0x0, 0x2, 0x00);    // set features P3
    	trx_afifo_ncmd(0x4, 0x2, 0x00);    // set features P4, check rnb status

    	writel(0x0, NF2FFCE);     // disable write trx-afifo

    	writel(0xe, NF2CSR);  // cs0 valid
    	writel(0x1, NF2STRR); // start trx-afifo

    	time_tick_start = get_ticks();
    	while(1)
    	{
    		//TODO add time out check	    
    	    	time_tick_cur = get_ticks();
    	    	if(time_tick_cur - time_tick_start > 500000)
    	    	{
    	    		printf("nf2_active_async timeout\n");
    	    		ret = -1;
    	    		break;
    	    	}	
    		tmp = readl(NF2INTR) & 0x1;
    		if(tmp == 1)break;
    	}

    	writel(0xf, NF2CSR);  // cs0 invalid
    	writel(0xffffff, NF2INTR);    // clear status

	return ret;
}
#endif

int imap_get_eslc_param_26(unsigned int *buf){
	
	int time_tick_start = 0;
	int time_tick_cur = 0;
	int tmp = 0;
	int ret = 0;

    	nf2_soft_reset(1);

    	writel(0x1, NF2FFCE);     // allow write trx-afifo
	
	trx_afifo_ncmd(0x0, 0x0, 0x37);    // set param CMD0
	trx_afifo_ncmd(0x0, 0x1, 0xb0);    // ADDRS
	trx_afifo_ncmd(0x0, 0x3, 0x00);    // data
	trx_afifo_ncmd(0x0, 0x1, 0xb1);    // ADDRS
	trx_afifo_ncmd(0x0, 0x3, 0x00);    // data
	trx_afifo_ncmd(0x0, 0x1, 0xa0);    // ADDRS
	trx_afifo_ncmd(0x0, 0x3, 0x00);    // data
	trx_afifo_ncmd(0x0, 0x1, 0xa1);    // ADDRS
	trx_afifo_ncmd(0x0, 0x3, 0x00);    // data
    	
	writel(0x0, NF2FFCE);     // disable write trx-afifo

    	writel(0xe, NF2CSR);	// cs0 valid
    	writel(0x1, NF2STRR);	// start trx-afifo
    	
	time_tick_start = get_ticks();
    	while(1)
    	{
    		//TODO add time out check	    
    		time_tick_cur = get_ticks();
    	    if(time_tick_cur - time_tick_start > 500000)
    	    {
    	    	printf("nf2 set param timeout\n");
    	    	ret = -1;
    	    	break;
    	    }	
    		tmp = readl(NF2INTR) & 0x1;
    		if(tmp == 1)break;
    	}

	writel(0xf, NF2CSR);    // cs0 invalid
    	writel(0xffffff, NF2INTR);	// clear status

	*buf = trx_afifo_read_state(0);

	return ret;

}

int imap_set_eslc_param_20(unsigned int *buf){
	
	int time_tick_start = 0;
	int time_tick_cur = 0;
	int tmp = 0;
	int ret = 0;
	unsigned char param0, param1, param2, param3;

	param0 = ((*buf)>>24) & 0xff;
	param1 = ((*buf)>>16) & 0xff;
	param2 = ((*buf)>>8) & 0xff;
	param3 = ((*buf)>>0) & 0xff;

    	nf2_soft_reset(1);

    	writel(0x1, NF2FFCE);     // allow write trx-afifo
	
	trx_afifo_ncmd(0x0, 0x0, 0x36);    // set param CMD0
	trx_afifo_ncmd(0x0, 0x1, 0xb0);    // ADDRS
	trx_afifo_ncmd(0x0, 0x2, param0);    // data
	trx_afifo_ncmd(0x0, 0x1, 0xb1);    // ADDRS
	trx_afifo_ncmd(0x0, 0x2, param1);    // data
	trx_afifo_ncmd(0x0, 0x1, 0xa0);    // ADDRS
	trx_afifo_ncmd(0x0, 0x2, param2);    // data
	trx_afifo_ncmd(0x0, 0x1, 0xa1);    // ADDRS
	trx_afifo_ncmd(0x0, 0x2, param3);    // data
	trx_afifo_ncmd(0x0, 0x0, 0x16);    // set param CMD1
    	
	writel(0x0, NF2FFCE);     // disable write trx-afifo

    	writel(0xe, NF2CSR);	// cs0 valid
    	writel(0x1, NF2STRR);	// start trx-afifo
    	
	time_tick_start = get_ticks();
    	while(1)
    	{
    		//TODO add time out check	    
    		time_tick_cur = get_ticks();
    	    if(time_tick_cur - time_tick_start > 500000)
    	    {
    	    	printf("nf2 set param timeout\n");
    	    	ret = -1;
    	    	break;
    	    }	
    		tmp = readl(NF2INTR) & 0x1;
    		if(tmp == 1)break;
    	}

	writel(0xf, NF2CSR);    // cs0 invalid
    	writel(0xffffff, NF2INTR);	// clear status

	return ret;

}

int imap_eslc_end(void){
	
	int time_tick_start = 0;
	int time_tick_cur = 0;
	int tmp = 0;
	int ret = 0;
    	
	nf2_soft_reset(1);

    	writel(0x1, NF2FFCE);     // allow write trx-afifo

	trx_afifo_ncmd(0x0, 0x0, 0x00);    // set param CMD0
	trx_afifo_ncmd(0x0, 0x1, 0x00);    // ADDRS
	trx_afifo_ncmd(0x0, 0x0, 0x30);    // set param CMD0
	
	writel(0x0, NF2FFCE);     // disable write trx-afifo

    	writel(0xe, NF2CSR);	// cs0 valid
    	writel(0x1, NF2STRR);	// start trx-afifo
    	
	time_tick_start = get_ticks();
    	while(1)
    	{
    		//TODO add time out check	    
    		time_tick_cur = get_ticks();
    	    if(time_tick_cur - time_tick_start > 500000)
    	    {
    	    	printf("nf2 set param timeout\n");
    	    	ret = -1;
    	    	break;
    	    }	
    		tmp = readl(NF2INTR) & 0x1;
    		if(tmp == 1)break;
    	}

	writel(0xf, NF2CSR);    // cs0 invalid
    	writel(0xffffff, NF2INTR);	// clear status

	return ret;

}

int imapx_read_spare(int page, int pagesize){

	int val = 0;
	int ret = 0;
	int tmp = 0;
	int cycle = 5;
	uint8_t buf[20];
	
	nf2_soft_reset(1);

	val  = (1<<11) | (3<<8);
    	writel(val, NF2ECCC);
    
	val  = (20); 
	writel(val, NF2PGEC);
	
	writel((int)buf, NF2SADR0);
	writel(20, NF2SBLKS);
   
	val = (1024<<16) | 1;
       	writel(val, NF2SBLKN);	
    
	val = 1;
	writel(val, NF2DMAC);
	
	writel(page, NF2RADR0);

	val = pagesize; //ecc offset
	writel(val, NF2CADR);

	writel(0x1, NF2FFCE); // allow write trx-afifo
	trx_afifo_ncmd(0x0, 0x0, 0x00);    // page read CMD0
	trx_afifo_ncmd((0x8 | (cycle -1)), 0x1, 0x0);    // 5 cycle addr of row_addr_0 & col_addr
	trx_afifo_ncmd(0x1, 0x0, 0x30);    // page read CMD1, check rnb and read whole page
	writel(0x0, NF2FFCE); // disable write trx-afifo
	writel(0xe, NF2CSR); // cs0 valid

    	writel(0x5, NF2STRR); // start trx-afifo,  and dma 

    	while(1)
    	{
    		//TODO add time out check	    
    		tmp = readl(NF2INTR) & (0x1<<5);
    		if(tmp == (0x1<<5))break;
    	}

	writel(0xf, NF2CSR);    // cs0 invalid
    	writel(0xffffff, NF2INTR);	// clear status

	//spl_printf("0x%x, 0x%x, 0x%x, 0x%x\n", buf[0],buf[1],buf[2],buf[3]);	
	//buf[1024] = (readl(NF2STSR0) & 0xff00) >> 8;
	//buf[1025] = (readl(NF2STSR0) & 0xff);
	if(buf[0] != 0xff){
		ret = 1;
	}else{
		ret = 0;
	}
	return ret;
}

int isbad(loff_t start){

	struct nand_config *nc = nand_get_config(NAND_CFG_NORMAL);
	int ret = 0;
	int page = 0;

	page = (start >> 13) + nc->badpagemajor;
	//spl_printf("check bad 0x%x\n", page);
	ret = imapx_read_spare(page, nc->pagesize);
	if(ret != 0){
		return ret;
	}
	
	page = (start >> 13) + nc->badpageminor;
	//spl_printf("check bad 0x%x\n", page);
	ret = imapx_read_spare(page, nc->pagesize);
		
	return ret;
}

int imap_get_retry_param_20(unsigned char *buf, int len, int param){

	int time_tick_start = 0;
	int time_tick_cur = 0;
	int tmp = 0;
	int ret = 0;
	int ecc_enable = 0;
	int page_mode = 0;
	int ecc_type = 0;
	int eram_mode = 0;
	int tot_ecc_num = 0;
	int tot_page_num = 0;
	int trans_dir = 0;
	int half_page_en = 0;
	int secc_used = 0;
	int secc_type = 0;
	int rsvd_ecc_en = 0;
	unsigned int ch0_adr = 0; 
	int ch0_len = 0;
	unsigned int ch1_adr = 0; 
	int ch1_len = 0;	
	int sdma_2ch_mode = 0;
	int row_addr = 0;
	int col_addr = 0;
	int cur_ecc_offset = 0;
	int busw = 8;

	ch0_adr = (unsigned int)buf;
	ch0_len = len;
	tot_page_num = ch0_len;

	printf("pagemode = 0\n");

	nf2_soft_reset(1);

	
	nf2_ecc_cfg(ecc_enable, page_mode, ecc_type, eram_mode, 
		 	tot_ecc_num, tot_page_num, trans_dir, half_page_en);

	nf2_secc_cfg(secc_used, secc_type, rsvd_ecc_en);

   	nf2_sdma_cfg(ch0_adr, ch0_len, ch1_adr, ch1_len, sdma_2ch_mode);

	nf2_addr_cfg(row_addr, col_addr, cur_ecc_offset, busw);

    	writel(0x1, NF2FFCE);     // allow write trx-afifo
	
	trx_afifo_ncmd(0x4, 0x0, 0xff);    // set CMD ff, check rnb
	trx_afifo_ncmd(0x0, 0x0, 0x36);    // set CMD 36
	if(param == 0x7){
		trx_afifo_ncmd(0x0, 0x1, 0xff);    // set ADDR ff
		trx_afifo_ncmd(0x0, 0x2, 0x40);    // write data 40
		trx_afifo_ncmd(0x0, 0x1, 0xcc);    // set ADDR cc
	}
	if(param == 0xf){
		trx_afifo_ncmd(0x0, 0x1, 0xae);    // set ADDR ae
		trx_afifo_ncmd(0x0, 0x2, 0x00);    // write data 00
		trx_afifo_ncmd(0x0, 0x1, 0xb0);    // set ADDR b0
	}
	trx_afifo_ncmd(0x0, 0x2, 0x4d);    // write data 4d
	trx_afifo_ncmd(0x0, 0x0, 0x16);    // set CMD 16
	trx_afifo_ncmd(0x0, 0x0, 0x17);    // set CMD 17
	trx_afifo_ncmd(0x0, 0x0, 0x04);    // set CMD 04
	trx_afifo_ncmd(0x0, 0x0, 0x19);    // set CMD 19
	trx_afifo_ncmd(0x0, 0x0, 0x00);    // set CMD 00
	trx_afifo_ncmd(0x0, 0x1, 0x00);    // set ADDR 00
	trx_afifo_ncmd(0x0, 0x1, 0x00);    // set ADDR 00
	trx_afifo_ncmd(0x0, 0x1, 0x00);    // set ADDR 00
	trx_afifo_ncmd(0x0, 0x1, 0x02);    // set ADDR 02
	trx_afifo_ncmd(0x0, 0x1, 0x00);    // set ADDR 00
	trx_afifo_ncmd(0x1, 0x0, 0x30);    // set CMD 30, check rnb and read page data
	trx_afifo_ncmd(0x4, 0x0, 0xff);    // set CMD ff, check rnb
	trx_afifo_ncmd(0x4, 0x0, 0x38);    // set CMD 38, check rnb

	writel(0x0, NF2FFCE);     // disable write trx-afifo

    	writel(0xe, NF2CSR);	// cs0 valid
    	//writel(0x1, NF2STRR);	// start trx-afifo
    	writel(0x5, NF2STRR); // start trx-afifo,  and dma 
    	
	time_tick_start = get_ticks();
    	while(1)
    	{
    		//TODO add time out check	    
    		time_tick_cur = get_ticks();
    	    if(time_tick_cur - time_tick_start > 500000)
    	    {
    	    	printf("nf2 get OTP param timeout\n");
    	    	ret = -1;
    	    	break;
    	    }	
    		tmp = readl(NF2INTR) & (0x1<<5);
    		if(tmp == (0x1<<5))break;
    	}

	writel(0xf, NF2CSR);    // cs0 invalid
    	writel(0xffffff, NF2INTR);	// clear status

	return ret;
}

int imap_set_retry_param_micro_20(int retrylevel){

	int time_tick_start = 0;
	int time_tick_cur = 0;
	int tmp = 0;
	int ret = 0;

	nf2_soft_reset(1);

	writel(0x1, NF2FFCE);     // allow write trx-afifo
	trx_afifo_ncmd(0x0, 0x0, 0xef);    // set EFH CMD0
	trx_afifo_ncmd(0x0, 0x1, 0x89);    // ADDRS
	trx_afifo_nop(0x0, 0x100);// for tadl delay
	trx_afifo_ncmd(0x0, 0x2, retrylevel);    // P1
	trx_afifo_ncmd(0x0, 0x2, 0x0);    // P2
	trx_afifo_ncmd(0x0, 0x2, 0x0);    // P3
	trx_afifo_ncmd(0x0, 0x2, 0x0);    // P4
	trx_afifo_nop(0x7, 100);// for twb delay
	trx_afifo_nop(0x0, 0x300);// 2.4us delay
	trx_afifo_nop(0x0, 0x300);// 2.4us delay
	trx_afifo_nop(0x0, 0x300);// 2.4us delay
	
	writel(0x0, NF2FFCE); // disable write trx-afifo

    	writel(0xe, NF2CSR);	// cs0 valid
    	writel(0x1, NF2STRR);	// start trx-afifo

	time_tick_start = get_ticks();
    	while(1)
    	{
    		//TODO add time out check	    
    		time_tick_cur = get_ticks();
    	    if(time_tick_cur - time_tick_start > 500000)
    	    {
    	    	printf("imap_set_retry_param_micro_20 timeout\n");
    	    	ret = -1;
    	    	break;
    	    }	
    		tmp = readl(NF2INTR) & 0x1;
    		if(tmp == 1)break;
    	}
	
	
	writel(0xf, NF2CSR);    // cs0 invalid
    	writel(0xffffff, NF2INTR);	// clear status
	return 0;
}


int imap_get_retry_param_20_from_page(int page, unsigned char *buf, int len){

	int time_tick_start = 0;
	int time_tick_cur = 0;
	int val = 0;
	int ret = 0;
	int tmp = 0;
	int ecc_info = 0;
	int cycle = 5;
	int i = 0;

	for(i=0; i<4; i++){
		nf2_soft_reset(1);

		val  = (3<<8) | (15<<4) | 0x1;//127bit
		writel(val, NF2ECCC);

		val  = (224<<16) | (len); //127bit
		writel(val, NF2PGEC);

		writel((int)buf, NF2SADR0);
		writel(len, NF2SBLKS);

		val = (1024<<16) | 1;
		writel(val, NF2SBLKN);	

		val = 1;
		writel(val, NF2DMAC);

		writel(page, NF2RADR0);

		val = 1024<<16; //ecc offset
		writel(val, NF2CADR);

		writel(0x1, NF2FFCE);     // allow write trx-afifo

		writel(0x1, NF2FFCE); // allow write trx-afifo
		trx_afifo_ncmd(0x0, 0x0, 0x00);    // page read CMD0
		trx_afifo_ncmd((0x8 | (cycle -1)), 0x1, 0x0);    // 5 cycle addr of row_addr_0 & col_addr
		trx_afifo_ncmd(0x1, 0x0, 0x30);    // page read CMD1, check rnb and read whole page
		writel(0x0, NF2FFCE); // disable write trx-afifo
		writel(0xe, NF2CSR); // cs0 valid

		writel(0x7, NF2STRR); // start trx-afifo,  and dma 

		time_tick_start = get_ticks();
		while(1)
		{
			//TODO add time out check	    
			time_tick_cur = get_ticks();
			if(time_tick_cur - time_tick_start > 500000)
			{
				printf("uboot1: imap_get_retry_param_20_from_page timeout\n");
				ret = -1;
				break;
			}	
			tmp = readl(NF2INTR) & (0x1<<5);
			if(tmp == (0x1<<5))break;
		}
		ecc_info = readl(NF2ECCINFO8);

		writel(0xf, NF2CSR);    // cs0 invalid
		writel(0xffffff, NF2INTR);	// clear status

		page += 1;
		if(ecc_info & 0x1){
			printf("uboot1: read otp table from page ecc failed, try next page 0x%x\n", page);
		}else{
			break;
		}
	}
	return ret;
}


int imap_set_retry_param_20(int retrylevel, unsigned char * otp_table, unsigned char * reg_buf){

	int time_tick_start = 0;
	int time_tick_cur = 0;
	int tmp = 0;
	int ret = 0;

	nf2_soft_reset(1);

    	writel(0x1, NF2FFCE);     // allow write trx-afifo

	trx_afifo_ncmd(0x0, 0x0, 0x36);    // set param CMD0
	trx_afifo_ncmd(0x0, 0x1, reg_buf[0]);    // ADDRS
	trx_afifo_nop(0x0, 70);
	trx_afifo_ncmd(0x0, 0x2, otp_table[0 + retrylevel * 8]);    // data
	trx_afifo_nop(0x0, 10);
	trx_afifo_ncmd(0x0, 0x1, reg_buf[1]);    // ADDRS
	trx_afifo_nop(0x0, 70);
	trx_afifo_ncmd(0x0, 0x2, otp_table[1 + retrylevel * 8]);    // data
	trx_afifo_nop(0x0, 10);
	trx_afifo_ncmd(0x0, 0x1, reg_buf[2]);    // ADDRS
	trx_afifo_nop(0x0, 70);
	trx_afifo_ncmd(0x0, 0x2, otp_table[2 + retrylevel * 8]);    // data
	trx_afifo_nop(0x0, 10);
	trx_afifo_ncmd(0x0, 0x1, reg_buf[3]);    // ADDRS
	trx_afifo_nop(0x0, 70);
	trx_afifo_ncmd(0x0, 0x2, otp_table[3 + retrylevel * 8]);    // data
	trx_afifo_nop(0x0, 10);
	trx_afifo_ncmd(0x0, 0x1, reg_buf[4]);    // ADDRS
	trx_afifo_nop(0x0, 70);
	trx_afifo_ncmd(0x0, 0x2, otp_table[4 + retrylevel * 8]);    // data
	trx_afifo_nop(0x0, 10);
	trx_afifo_ncmd(0x0, 0x1, reg_buf[5]);    // ADDRS
	trx_afifo_nop(0x0, 70);
	trx_afifo_ncmd(0x0, 0x2, otp_table[5 + retrylevel * 8]);    // data
	trx_afifo_nop(0x0, 10);
	trx_afifo_ncmd(0x0, 0x1, reg_buf[6]);    // ADDRS
	trx_afifo_nop(0x0, 70);
	trx_afifo_ncmd(0x0, 0x2, otp_table[6 + retrylevel * 8]);    // data
	trx_afifo_nop(0x0, 10);
	trx_afifo_ncmd(0x0, 0x1, reg_buf[7]);    // ADDRS
	trx_afifo_nop(0x0, 70);
	trx_afifo_ncmd(0x0, 0x2, otp_table[7 + retrylevel * 8]);    // data
	trx_afifo_nop(0x0, 10);
	trx_afifo_ncmd(0x0, 0x0, 0x16);    // set param CMD1
    	
	writel(0x0, NF2FFCE);     // disable write trx-afifo

    	writel(0xe, NF2CSR);	// cs0 valid
    	writel(0x1, NF2STRR);	// start trx-afifo
    	
	time_tick_start = get_ticks();
    	while(1)
    	{
    		//TODO add time out check	    
    		time_tick_cur = get_ticks();
    	    if(time_tick_cur - time_tick_start > 500000)
    	    {
    	    	printf("nf2 set param timeout\n");
    	    	ret = -1;
    	    	break;
    	    }	
    		tmp = readl(NF2INTR) & 0x1;
    		if(tmp == 1)break;
    	}
	
	writel(0xf, NF2CSR);    // cs0 invalid
    	writel(0xffffff, NF2INTR);	// clear status

	return ret;	

}

int imap_get_retry_param_26(unsigned char *buf){

	int time_tick_start = 0;
	int time_tick_cur = 0;
	int tmp = 0;
	int ret = 0;
	int retry_param = 0;

    	nf2_soft_reset(1);

    	writel(0x1, NF2FFCE);     // allow write trx-afifo
	
	trx_afifo_ncmd(0x0, 0x0, 0x37);    // set param CMD0
	trx_afifo_ncmd(0x0, 0x1, 0xa7);    // ADDRS
	trx_afifo_ncmd(0x0, 0x3, 0x00);    // data
	trx_afifo_ncmd(0x0, 0x1, 0xad);    // ADDRS
	trx_afifo_ncmd(0x0, 0x3, 0x00);    // data
	trx_afifo_ncmd(0x0, 0x1, 0xae);    // ADDRS
	trx_afifo_ncmd(0x0, 0x3, 0x00);    // data
	trx_afifo_ncmd(0x0, 0x1, 0xaf);    // ADDRS
	trx_afifo_ncmd(0x0, 0x3, 0x00);    // data
    	
	writel(0x0, NF2FFCE);     // disable write trx-afifo

    	writel(0xe, NF2CSR);	// cs0 valid
    	writel(0x1, NF2STRR);	// start trx-afifo
    	
	time_tick_start = get_ticks();
    	while(1)
    	{
    		//TODO add time out check	    
    		time_tick_cur = get_ticks();
    	    if(time_tick_cur - time_tick_start > 500000)
    	    {
    	    	printf("nf2 set param timeout\n");
    	    	ret = -1;
    	    	break;
    	    }	
    		tmp = readl(NF2INTR) & 0x1;
    		if(tmp == 1)break;
    	}

	writel(0xf, NF2CSR);    // cs0 invalid
    	writel(0xffffff, NF2INTR);	// clear status

	retry_param = trx_afifo_read_state(0);
	buf[0] = (retry_param >> 24) & 0xff;
	buf[1] = (retry_param >> 16) & 0xff;
	buf[2] = (retry_param >> 8) & 0xff;
	buf[3] = (retry_param >> 0) & 0xff;
	printf("uboot: 0xa7 = 0x%x, 0xad = 0x%x, 0xae = 0x%x, 0xaf = 0x%x\n", buf[0], buf[1], buf[2], buf[3]);
	return 0;

}

int imap_set_retry_param_26(int retrylevel, unsigned char * buf){

	int time_tick_start = 0;
	int time_tick_cur = 0;
	int tmp = 0;
	int ret = 0;

	nf2_soft_reset(1);

    	writel(0x1, NF2FFCE);     // allow write trx-afifo


	switch(retrylevel){
		case 0:
			trx_afifo_ncmd(0x0, 0x0, 0x36);    // set param CMD0
			trx_afifo_ncmd(0x0, 0x1, 0xa7);    // ADDRS
			trx_afifo_ncmd(0x0, 0x2, buf[0]);    // data
			trx_afifo_ncmd(0x0, 0x1, 0xad);    // ADDRS
			trx_afifo_ncmd(0x0, 0x2, buf[1]);    // data
			trx_afifo_ncmd(0x0, 0x1, 0xae);    // ADDRS
			trx_afifo_ncmd(0x0, 0x2, buf[2]);    // data
			trx_afifo_ncmd(0x0, 0x1, 0xaf);    // ADDRS
			trx_afifo_ncmd(0x0, 0x2, buf[3]);    // data
			trx_afifo_ncmd(0x0, 0x0, 0x16);    // set param CMD1
			break;
		case 1:
			trx_afifo_ncmd(0x0, 0x0, 0x36);    // set param CMD0
			trx_afifo_ncmd(0x0, 0x1, 0xa7);    // ADDRS
			trx_afifo_ncmd(0x0, 0x2, buf[0]);    // data
			trx_afifo_ncmd(0x0, 0x1, 0xad);    // ADDRS
			trx_afifo_ncmd(0x0, 0x2, buf[1] + 0x06);    // data
			trx_afifo_ncmd(0x0, 0x1, 0xae);    // ADDRS
			trx_afifo_ncmd(0x0, 0x2, buf[2] + 0x0a);    // data
			trx_afifo_ncmd(0x0, 0x1, 0xaf);    // ADDRS
			trx_afifo_ncmd(0x0, 0x2, buf[3] + 0x06);    // data
			trx_afifo_ncmd(0x0, 0x0, 0x16);    // set param CMD1
			break;
		case 2:
			trx_afifo_ncmd(0x0, 0x0, 0x36);    // set param CMD0
			trx_afifo_ncmd(0x0, 0x1, 0xa7);    // ADDRS
			trx_afifo_ncmd(0x0, 0x2, 0x00);    // data
			trx_afifo_ncmd(0x0, 0x1, 0xad);    // ADDRS
			trx_afifo_ncmd(0x0, 0x2, buf[1] - 0x3);    // data
			trx_afifo_ncmd(0x0, 0x1, 0xae);    // ADDRS
			trx_afifo_ncmd(0x0, 0x2, buf[2] - 0x7);    // data
			trx_afifo_ncmd(0x0, 0x1, 0xaf);    // ADDRS
			trx_afifo_ncmd(0x0, 0x2, buf[3] - 0x8);    // data
			trx_afifo_ncmd(0x0, 0x0, 0x16);    // set param CMD1
			break;
		case 3:
			trx_afifo_ncmd(0x0, 0x0, 0x36);    // set param CMD0
			trx_afifo_ncmd(0x0, 0x1, 0xa7);    // ADDRS
			trx_afifo_ncmd(0x0, 0x2, 0x00);    // data
			trx_afifo_ncmd(0x0, 0x1, 0xad);    // ADDRS
			trx_afifo_ncmd(0x0, 0x2, buf[1] - 0x6);    // data
			trx_afifo_ncmd(0x0, 0x1, 0xae);    // ADDRS
			trx_afifo_ncmd(0x0, 0x2, buf[2] - 0xd);    // data
			trx_afifo_ncmd(0x0, 0x1, 0xaf);    // ADDRS
			trx_afifo_ncmd(0x0, 0x2, buf[3] - 0xf);    // data
			trx_afifo_ncmd(0x0, 0x0, 0x16);    // set param CMD1
			break;
		case 4:
			trx_afifo_ncmd(0x0, 0x0, 0x36);    // set param CMD0
			trx_afifo_ncmd(0x0, 0x1, 0xa7);    // ADDRS
			trx_afifo_ncmd(0x0, 0x2, 0x00);    // data
			trx_afifo_ncmd(0x0, 0x1, 0xad);    // ADDRS
			trx_afifo_ncmd(0x0, 0x2, buf[1] - 0x9);    // data
			trx_afifo_ncmd(0x0, 0x1, 0xae);    // ADDRS
			trx_afifo_ncmd(0x0, 0x2, buf[2] - 0x14);    // data
			trx_afifo_ncmd(0x0, 0x1, 0xaf);    // ADDRS
			trx_afifo_ncmd(0x0, 0x2, buf[3] - 0x17);    // data
			trx_afifo_ncmd(0x0, 0x0, 0x16);    // set param CMD1
			break;
		case 5:
			trx_afifo_ncmd(0x0, 0x0, 0x36);    // set param CMD0
			trx_afifo_ncmd(0x0, 0x1, 0xa7);    // ADDRS
			trx_afifo_ncmd(0x0, 0x2, 0x00);    // data
			trx_afifo_ncmd(0x0, 0x1, 0xad);    // ADDRS
			trx_afifo_ncmd(0x0, 0x2, 0x00);    // data
			trx_afifo_ncmd(0x0, 0x1, 0xae);    // ADDRS
			trx_afifo_ncmd(0x0, 0x2, buf[2] - 0x1a);    // data
			trx_afifo_ncmd(0x0, 0x1, 0xaf);    // ADDRS
			trx_afifo_ncmd(0x0, 0x2, buf[3] - 0x1e);    // data
			trx_afifo_ncmd(0x0, 0x0, 0x16);    // set param CMD1
			break;
		case 6:
			trx_afifo_ncmd(0x0, 0x0, 0x36);    // set param CMD0
			trx_afifo_ncmd(0x0, 0x1, 0xa7);    // ADDRS
			trx_afifo_ncmd(0x0, 0x2, 0x00);    // data
			trx_afifo_ncmd(0x0, 0x1, 0xad);    // ADDRS
			trx_afifo_ncmd(0x0, 0x2, 0x00);    // data
			trx_afifo_ncmd(0x0, 0x1, 0xae);    // ADDRS
			trx_afifo_ncmd(0x0, 0x2, buf[2] - 0x20);    // data
			trx_afifo_ncmd(0x0, 0x1, 0xaf);    // ADDRS
			trx_afifo_ncmd(0x0, 0x2, buf[3] - 0x25);    // data
			trx_afifo_ncmd(0x0, 0x0, 0x16);    // set param CMD1
			break;
		default:
			printf("read retry level is not allow\n");
			break;	
	}

    	writel(0x0, NF2FFCE);     // disable write trx-afifo

    	writel(0xe, NF2CSR);	// cs0 valid
    	writel(0x1, NF2STRR);	// start trx-afifo
    	
	time_tick_start = get_ticks();
    	while(1)
    	{
    		//TODO add time out check	    
    		time_tick_cur = get_ticks();
    	    if(time_tick_cur - time_tick_start > 500000)
    	    {
    	    	printf("nf2 set param timeout\n");
    	    	ret = -1;
    	    	break;
    	    }	
    		tmp = readl(NF2INTR) & 0x1;
    		if(tmp == 1)break;
    	}
	
	writel(0xf, NF2CSR);    // cs0 invalid
    	writel(0xffffff, NF2INTR);	// clear status

	return ret;

}

int imap_get_retry_param_21(unsigned char * buf)
{	
	/*set retry param by willie*/  
	buf[0] = 0xa1;	//CMD: set register data 
	buf[1] = 0x00; 	
	buf[2] = 0xa7;	//read level for P0	reg or data?
	buf[3] = 0xa4;	//read level for P1
	buf[4] = 0xa5;	//read level for P2
	buf[5] = 0xa6;	//read level for P3
	/*retry table*/
	buf[6] = 0x00;	//default 0, retry end need do default;
	buf[7] = 0x00;
	buf[8] = 0x00;
	buf[9] = 0x00;

	buf[10] = 0x05;	//retry table 1, all 14 times;
	buf[11] = 0x0a;
	buf[12] = 0x00;
	buf[13] = 0x00;
	
	buf[14] = 0x28;	//retry table 2;
	buf[15] = 0x00;
	buf[16] = 0xec;
	buf[17] = 0xd8;	

	buf[18] = 0xed;	//retry table 3;
	buf[19] = 0xf5;
	buf[20] = 0xed;
	buf[21] = 0xe6;	

	buf[22] = 0x0a;	//retry table 4;
	buf[23] = 0x0f;
	buf[24] = 0x05;
	buf[25] = 0x00;	

	buf[26] = 0x0f;	//retry table 5;
	buf[27] = 0x0a;
	buf[28] = 0xfb;
	buf[29] = 0xec;	

	buf[30] = 0xe8;	//retry table 6;
	buf[31] = 0xef;
	buf[32] = 0xe8;
	buf[33] = 0xdc;	

	buf[34] = 0xf1;	//retry table 7;
	buf[35] = 0xfb;
	buf[36] = 0xfe;
	buf[37] = 0xf0;	

	buf[38] = 0x0a;	//retry table 8;
	buf[39] = 0x00;
	buf[40] = 0xfb;
	buf[41] = 0xec;	

	buf[42] = 0xd0;	//retry table 9;
	buf[43] = 0xe2;
	buf[44] = 0xd0;
	buf[45] = 0xc2;	

	buf[46] = 0x14;	//retry table 10;
	buf[47] = 0x0f;
	buf[48] = 0xfb;
	buf[49] = 0xec;	

	buf[50] = 0xe8;	//retry table 11;
	buf[51] = 0xfb;
	buf[52] = 0xe8;
	buf[53] = 0xdc;	

	buf[54] = 0x1e;	//retry table 12;
	buf[55] = 0x14;
	buf[56] = 0xfb;
	buf[57] = 0xec;	

	buf[58] = 0xfb;	//retry table 13;
	buf[59] = 0xff;
	buf[60] = 0xfb;
	buf[61] = 0xf8;	

	buf[62] = 0x07;	//retry table 14;
	buf[63] = 0x0c;
	buf[64] = 0x02;
	buf[65] = 0x00;	

	return 0;
}

int imap_set_retry_param_21(int retrylevel, unsigned char * buf){

	int time_tick_start = 0;
	int time_tick_cur = 0;
	int tmp = 0;
	int ret = 0;
	int i = 0;

	nf2_soft_reset(1);

    	writel(0x1, NF2FFCE);     // allow write trx-afifo
#if 0
	printf("retrylevel %d \n", retrylevel);
	printf("retry tab 0x%x, 0x%x 0x%x, 0x%x\n", 
			buf[(retrylevel + 1)*4 + 2], buf[(retrylevel + 1)*4 + 3], buf[(retrylevel + 1)*4 + 4], buf[(retrylevel + 1)*4 + 5]);
#endif

	for(i=0; i<4; i++)
	{
		trx_afifo_nop(0x0, 0x64);       //3ns * 0x64 = 300ns
		trx_afifo_ncmd(0x0, 0x0, buf[0]);
		trx_afifo_ncmd(0x0, 0x2, buf[1]);
		trx_afifo_ncmd(0x0, 0x2, buf[i + 2]);
		trx_afifo_ncmd(0x0, 0x2, buf[(retrylevel + 1)*4 + i + 2]);
	}
    	
	writel(0x0, NF2FFCE);     // disable write trx-afifo

    	writel(0xe, NF2CSR);	// cs0 valid
    	writel(0x1, NF2STRR);	// start trx-afifo
    	
	time_tick_start = get_ticks();
    	while(1)
    	{
    		//TODO add time out check	    
    		time_tick_cur = get_ticks();
    	    if(time_tick_cur - time_tick_start > 500000)
    	    {
    	    	printf("nf2 set param timeout\n");
    	    	ret = -1;
    	    	break;
    	    }	
    		tmp = readl(NF2INTR) & 0x1;
    		if(tmp == 1)break;
    	}
	
	writel(0xf, NF2CSR);    // cs0 invalid
    	writel(0xffffff, NF2INTR);	// clear status

	return ret;

}

unsigned char buf_crd_param19 [63] = {
	0x00, 0x00, 0x00,
	0xf0, 0xf0, 0xf0,
	0xef, 0xe0, 0xe0,
	0xdf, 0xd0, 0xd0,
	0x1e, 0xe0, 0x10,
	0x2e, 0xd0, 0x20,
	0x3d, 0xf0, 0x30,
	0xcd, 0xe0, 0xd0,
	0x0d, 0xd0, 0x10,
	0x01, 0x10, 0x20,
	0x12, 0x20, 0x20,
	0xb2, 0x10, 0xd0,
	0xa3, 0x20, 0xd0,
	0x9f, 0x00, 0xd0,
	0xbe, 0xf0, 0xc0,
	0xad, 0xc0, 0xc0,
	0x9f, 0xf0, 0xc0,
	0x01, 0x00, 0x00,
	0x02, 0x00, 0x00,
	0x0d, 0xb0, 0x00,
	0x0c, 0xa0, 0x00,
};

int init_retry_param_sandisk19(void ){

	int i = 0 , tmp = 0;

	nf2_soft_reset(1);
	writel(0x1, NF2FFCE);     // allow write trx-afifo
	
	trx_afifo_ncmd(0x0, 0x0, 0xB6);           
	trx_afifo_ncmd(0x0, 0x0, 0x3b);
	trx_afifo_ncmd(0x0, 0x0, 0xb9);

	for(i=4; i<13; i++){
		trx_afifo_ncmd(0x0, 0x0, 0x53);
		trx_afifo_ncmd(0x0, 0x1, i);    // set ADDR
		trx_afifo_nop(0x0, 0x64);
		trx_afifo_ncmd(0x0, 0x2, 0x0);
		trx_afifo_nop(0x0, 0x64);
	}

	writel(0x0, NF2FFCE);

	writel(0xe, NF2CSR);    // cs0 valid
	writel(0x1, NF2STRR);   // start trx-afifo

	while(1)
	{
		//TODO add time out check           
		tmp = readl(NF2INTR) & 0x1;
		if(tmp == 1)break;
	}

	writel(0xf, NF2CSR);    // cs0 invalid
	writel(0xffffff, NF2INTR);      // clear status

	return 0;
}

int imap_set_retry_param_sandisk19(int retrylevel){
	int i = 0, tmp = 0;
	int ret = 0;

	/*sandisk special need: enable*/                  
	if(retrylevel == 1)        
            init_retry_param_sandisk19();

	nf2_soft_reset(1);

	writel(0x1, NF2FFCE);     // allow write trx-afifo
	
	trx_afifo_ncmd(0x0, 0x0, 0x3b);
	trx_afifo_ncmd(0x0, 0x0, 0xb9);
	for(i=0; i<3; i++){
		trx_afifo_ncmd(0x0, 0x0, 0x53);
		if(i == 2)
			trx_afifo_ncmd(0x0, 0x1, 0x04 + i + 1);    // set ADDR ff
		else
		trx_afifo_ncmd(0x0, 0x1, 0x04 + i);    // set ADDR ff
		trx_afifo_nop(0x0, 0x64);
		trx_afifo_ncmd(0x0, 0x2, buf_crd_param19[retrylevel * 3 + i]);    // set data
		trx_afifo_nop(0x0, 0x64);
	}
	/*sandisk special need: disable*/                  
	if(retrylevel == 0)                                
		trx_afifo_ncmd(0x0, 0x0, 0xD6);            

	writel(0x0, NF2FFCE);     // disable write trx-afifo

    	writel(0xe, NF2CSR);	// cs0 valid
    	writel(0x1, NF2STRR);	// start trx-afifo
    	
    	while(1)
    	{
    		//TODO add time out check	    
    		tmp = readl(NF2INTR) & 0x1;
    		if(tmp == 1)break;
    	}
	
	writel(0xf, NF2CSR);    // cs0 invalid
    	writel(0xffffff, NF2INTR);	// clear status


	return ret;
}

int nf2_set_busw(int busw){

	if(busw == 16){
		writel(0x1, NF2BSWMODE);
	}else{
		writel(0x0, NF2BSWMODE);
	}	

	return 0;
}

int nf2_active_sync (int syn_timemode) {

	int time_tick_start = 0;
	int time_tick_cur = 0;
	int tmp = 0;
	int ret = 0;
    	nf2_soft_reset(1);

    	writel(0x0, NF2PSYNC);    // set host to async mode
    	//step : program trx-afifo
    	writel(0x1, NF2FFCE);     // allow write trx-afifo

    	trx_afifo_ncmd(0x0, 0x0, 0xef);    // set features CMD
    	trx_afifo_ncmd(0x0, 0x1, 0x01);    // set features ADDR 01H
    	trx_afifo_ncmd(0x0, 0x2, syn_timemode);    // set features P1
    	trx_afifo_ncmd(0x0, 0x2, 0x00);    // set features P2
    	trx_afifo_ncmd(0x0, 0x2, 0x00);    // set features P3
    	trx_afifo_ncmd(0x4, 0x2, 0x00);    // set features P4, check rnb status
    	trx_afifo_reg(0x2, 0xf, 0x0);      // cs invalid
    	trx_afifo_nop(0x0, 0x100);         // nop
    	trx_afifo_reg(0x3, 0x1, 0x0);      // set sync mode

    	writel(0x0, NF2FFCE);     // disable write trx-afifo

    	writel(0xe, NF2CSR);	// cs0 valid
    	writel(0x1, NF2STRR);	// start trx-afifo

    	time_tick_start = get_ticks();
    	while(1)
    	{
    		//TODO add time out check	    
    		time_tick_cur = get_ticks();
    	    if(time_tick_cur - time_tick_start > 500000)
    	    {
    	    	printf("nf2_active_sync timeout\n");
    	    	ret = -1;
    	    	break;
    	    }	
    		tmp = readl(NF2INTR) & 0x1;
    		if(tmp == 1)break;
    	}

    	writel(0xffffff, NF2INTR);	// clear status

	return ret;
}

int nf2_asyn_reset(int cs)
{
	int time_tick_start = 0;
	int time_tick_cur = 0;
	int tmp = 0;
	int ret = 0;

     //step 1 : Reset internal logic
     nf2_soft_reset(1);

     //step : program trx-afifo
     writel(0x1, NF2FFCE);     // allow write trx-afifo
     trx_afifo_ncmd(0x4, 0x0, 0xff);    // reset CMD, wait for rnb ready
     writel(0x0, NF2FFCE);     // disable write trx-afifo

     tmp = readl(NF2CSR);
     tmp  = ~(1<<cs);  // cs0 valid
     writel(tmp, NF2CSR);
     
     writel(0x1, NF2STRR); // start trx-afifo

    time_tick_start = get_ticks();
    while(1)
    {
    	//TODO add time out check	    
    	time_tick_cur = get_ticks();
	if(time_tick_cur - time_tick_start > 500000)
	{
		printf("nf2_asyn_reset timeout\n");
		ret = -1;
		break;
	}	
    	tmp = readl(NF2INTR) & 0x1;
    	if(tmp == 1)break;
    }

     writel(0xffffff, NF2INTR);  // clear status
     writel(0xf, NF2CSR);  // cs0 invalid
 
     return ret;
 
}

unsigned int nf2_randc_seed_hw (unsigned int raw_seed, unsigned int cycle, int polynomial)
{
	unsigned int result;
    int time_tick_start = 0;
    int time_tick_cur = 0;

	writel(cycle << 16 | raw_seed, NF2RAND0);
	writel(0x1, NF2RAND1);
	writel(0x0, NF2RAND1);
    	
	time_tick_start = get_ticks();
    while(1)
    {
    	//TODO add time out check	    
    	time_tick_cur = get_ticks();
        if(time_tick_cur - time_tick_start > 500000)
        {
        	printf("nf2_randc_seed_hw timeout\n");
        	result = -1;
        	return result;
        }	
		result = readl(NF2RAND2);
		if((result & 0x10000) == 0)
		  break;
    }
	
	result = readl(NF2RAND2) & 0xffff;
	
	return result;

}

#if 0
unsigned int nf2_randc_seed (unsigned int raw_seed, unsigned int cycle, int polynomial)
{
    unsigned int randc_reg0  ;
    unsigned int randc_reg1  ;
    unsigned int randc_reg2  ;
    unsigned int randc_reg3  ;
    unsigned int randc_reg4  ;
    unsigned int randc_reg5  ;
    unsigned int randc_reg6  ;
    unsigned int randc_reg7  ;
    unsigned int randc_reg8  ;
    unsigned int randc_reg9  ;
    unsigned int randc_reg10 ;
    unsigned int randc_reg11 ;
    unsigned int randc_reg12 ;
    unsigned int randc_reg13 ;
    unsigned int randc_reg ;

    int i;

    randc_reg = raw_seed;

    //randc_reg
    for (i = 0; i<=cycle; i++){
    	if(polynomial & 0x1){
    		randc_reg0  = ((randc_reg >> 1 ) ^ (randc_reg)) & 0x1;
    	}
    	else{
    		randc_reg0  =  (randc_reg >> 1 ) & 0x1;
    	}
    	if((polynomial & 0x2)>>1){
    		randc_reg1  = ((randc_reg >> 2 ) ^ (randc_reg)) & 0x1;
    	}
    	else{
    		randc_reg1  =  (randc_reg >> 2 ) & 0x1;
    	}
    	if((polynomial & 0x4)>>2){
    		randc_reg2  = ((randc_reg >> 3 ) ^ (randc_reg)) & 0x1;
    	}
    	else{
    		randc_reg2  =  (randc_reg >> 3 ) & 0x1;
    	}	
    	if((polynomial & 0x8)>>3){
    		randc_reg3  = ((randc_reg >> 4 ) ^ (randc_reg)) & 0x1;
    	}
    	else{
    		randc_reg3  =  (randc_reg >> 4 ) & 0x1;
    	}	
    	if((polynomial & 0x10)>>4){
    		randc_reg4  = ((randc_reg >> 5 ) ^ (randc_reg)) & 0x1;
    	}
    	else{
    		randc_reg4  =  (randc_reg >> 5 ) & 0x1;
    	}
    	if((polynomial & 0x20)>>5){
    		randc_reg5  = ((randc_reg >> 6 ) ^ (randc_reg)) & 0x1;
    	}
    	else{
    		randc_reg5  =  (randc_reg >> 6 ) & 0x1;
    	}
    	if((polynomial & 0x40)>>6){
    		randc_reg6  = ((randc_reg >> 7 ) ^ (randc_reg)) & 0x1;
    	}
    	else{
    		randc_reg6  =  (randc_reg >> 7 ) & 0x1;
    	}
    	if((polynomial & 0x80)>>7){
    		randc_reg7  = ((randc_reg >> 8 ) ^ (randc_reg)) & 0x1;
    	}
    	else{
    		randc_reg7  =  (randc_reg >> 8 ) & 0x1;
    	}
    	if((polynomial & 0x100)>>8){
    		randc_reg8  = ((randc_reg >> 9 ) ^ (randc_reg)) & 0x1;
    	}
    	else{
    		randc_reg8  =  (randc_reg >> 9 ) & 0x1;
    	}
    	if((polynomial & 0x200)>>9){
    		randc_reg9  = ((randc_reg >> 10 ) ^ (randc_reg)) & 0x1;
    	}
    	else{
    		randc_reg9  =  (randc_reg >> 10) & 0x1;
    	}
    	if((polynomial & 0x400)>>10){
    		randc_reg10  = ((randc_reg >> 11 ) ^ (randc_reg)) & 0x1;
    	}
    	else{
    		randc_reg10 =  (randc_reg >> 11) & 0x1;
    	}
    	if((polynomial & 0x800)>>11){
    		randc_reg11 = ((randc_reg >> 12) ^ (randc_reg)) & 0x1;
    	}
    	else{
    		randc_reg11 =  (randc_reg >> 12) & 0x1;
    	}
    	if((polynomial & 0x1000)>>12){
    		randc_reg12 = ((randc_reg >> 13) ^ (randc_reg)) & 0x1;
    	}
    	else{
    		randc_reg12 =  (randc_reg >> 13) & 0x1;
    	}
    	if((polynomial & 0x2000)>>13){
    		randc_reg13 = ((randc_reg >> 0) ^ (randc_reg)) & 0x1;
    	}
    	else{
    		randc_reg13 =  (randc_reg >> 0 ) & 0x1;
    	}
        randc_reg   = (randc_reg13<<13) | (randc_reg12<<12) | (randc_reg11<<11) | (randc_reg10<<10) | (randc_reg9<<9) | (randc_reg8<<8) |
                       (randc_reg7<<7) | (randc_reg6<<6) | (randc_reg5<<5) | (randc_reg4<<4) | (randc_reg3<<3) | (randc_reg2<<2) | (randc_reg1<<1) | randc_reg0;
    }

    return randc_reg;
}
#endif

#if 0
int nf2_timing_async(int timing, int rnbtimeout)
{
	/* init nf pads */
	pads_chmod(PADSRANGE(0, 15), PADS_MODE_CTRL, 0);

	writel(0x0, NF2PSYNC);
	writel((timing & 0x1fffff), NF2AFTM);
	writel(0xe001, NF2STSC);	
	writel(rnbtimeout, NF2TOUT);

	return 1;
}	
#endif

uint32_t x800_gtiming = 0;
void nf2_set_gtiming(uint32_t a)
{
	x800_gtiming = a;
}


int nf2_timing_init(int interface, int timing, int rnbtimeout, int phyread, int phydelay, int busw)
{
	unsigned int pclk_cfg = 0;
	/* init nf pads */ 
	module_enable(NF2_SYSM_ADDR);
	pads_chmod(PADSRANGE(0, 15), PADS_MODE_CTRL, 0);
	if(ecfg_check_flag(ECFG_ENABLE_PULL_NAND)) {
		pads_pull(PADSRANGE(0, 15), 1, 1);
		pads_pull(PADSRANGE(10, 11), 1, 0);		/* cle, ale */
		pads_pull(15, 1, 0);					/* dqs */
	}
	if(busw == 16){
		pads_chmod(PADSRANGE(18, 19), PADS_MODE_CTRL, 0);
		pads_chmod(PADSRANGE(25, 30), PADS_MODE_CTRL, 0);
		if(ecfg_check_flag(ECFG_ENABLE_PULL_NAND)){
	  		pads_pull(PADSRANGE(18, 19), 1, 1);
	  		pads_pull(PADSRANGE(25, 30), 1, 1);
		}
	}

	nf2_set_busw(busw);

	pclk_cfg = readl(NF2PCLKM);
	pclk_cfg &= ~(0x3<<4);
	pclk_cfg |= (0x3<<4); //ecc_clk/8
	writel(pclk_cfg, NF2PCLKM);	

	if(interface == 0)//async
	{
		writel(0x0, NF2PSYNC);
		pclk_cfg = readl(NF2PCLKM);
		pclk_cfg |= (0x1); //sync clock gate
		writel(pclk_cfg, NF2PCLKM);	
	}
	else if(interface == 1)// onfi sync
	{
		writel(0x1, NF2PSYNC);
		pclk_cfg = readl(NF2PCLKM);
		pclk_cfg &= ~(0x1); //sync clock pass
		writel(pclk_cfg, NF2PCLKM);	
	}
	else	//toggle
	{
		writel(0x2, NF2PSYNC);
		pclk_cfg = readl(NF2PCLKM);
		pclk_cfg &= ~(0x1); //sync clock pass
		writel(pclk_cfg, NF2PCLKM);	
	}	
	
	if(x800_gtiming)
	  writel(x800_gtiming, NF2AFTM);
	else
	  writel((timing & 0x1fffff), NF2AFTM);

	writel(((timing & 0xff000000) >> 24), NF2SFTM);
	writel(0xe001, NF2STSC);	
	//writel(0x4023, NF2PRDC);	
	writel(phyread, NF2PRDC);	
	//writel(0xFFFA00, NF2TOUT);
	writel(rnbtimeout, NF2TOUT);
	//writel(0x3818, NF2PDLY);
	writel(phydelay, NF2PDLY);


	return 1;	
}

int nf2_hardware_reset(struct nand_config *c, int active_sync) {
	int ret;

	nf2_timing_init(((c->interface == 2)?2:0), c->timing, c->rnbtimeout, c->phyread, c->phydelay, c->busw);
	ret = nf2_asyn_reset(0);
	if(ret)
		return ret;

	if(active_sync && c->interface == 1) {
		nf2_timing_init(c->interface, c->timing, c->rnbtimeout, c->phyread, c->phydelay, c->busw);
		return nf2_active_sync(0x10);
	}

	return 0;
}

int nf2_check_irq(void)
{
	unsigned int cInt = 0;
	unsigned int val0 = 0, val1 = 0;

	val0 = readl(NF2INTR);	
	val1 = readl(NF2INTE);
    	cInt = val0 & val1 ;    // valid flag

    if (cInt & (1<<18)){    // cpu free int
        val0 = readl(NF2INTE);
	val0 &= ~(0x1<<18); // disable free int enable
	writel(val0, NF2INTE);
	val0  = (1<<18);
        writel(val0, NF2INTR);
        cInt &= ~(1<<18);
    }
    else if (cInt & (1<<17)){    // cpu word int
        val0 = readl(NF2INTE);
	val0 &= ~(1<<17);    // disable word int enable
	writel(val0, NF2INTE);
	val0 = (1<<17);
        writel(val0, NF2INTR);
        cInt &= ~(1<<17);
    }
    else if ((cInt & (1<<10)) && (cInt & (1<<19))) {    // cpu data int and last data int
        val0 = readl(NF2INTE);
	val0 &= ~((1<<19) | (1<<10));    // disable data int and last data int enable
	writel(val0, NF2INTE);
        val0 = (1<<19) | (1<<10);
	writel(val0, NF2INTR);
        cInt &= ~(1<<19 | 1<<10);
    }
    else if (cInt & (1<<10)){    // cpu data int
        val0 = readl(NF2INTE);
	val0 &= ~(1<<10);    // disable data int enable
	writel(val0, NF2INTE);
        val0 = (1<<10);
	writel(val0, NF2INTR);
        cInt &= ~(1<<10);
    }
    else if(cInt & (1<<12)){
   	printf("write error state\n");
        writel(0x1000, NF2INTE);	
    }	    	    
    else if (cInt & (1<<0)) {   // trxf-empty int
	writel(cInt, NF2INTR);
        val0 = 0xf;  // cs0 invalid
	writel(val0, NF2CSR);
    }
    else if (cInt & (1<<1)) {   // trxf-Line int
        val0 = (1<<1);
	writel(val0, NF2INTR);
        val0  = 1<<0;     // trxf-line int ack.
	writel(val0, NF2INTA);
    }
    else if (cInt & (1<<15)) {   // trxf-repeat nested error int
        val0 = (1<<15);
        writel(val0, NF2INTR);
        cInt &= ~(1<<15);
    }
    else if (cInt & (1<<8)) {   // dec error number threshold
        val0 = (1<<8);
        writel(val0, NF2INTR);
        cInt        &= ~(1<<8);
    }
    else if (cInt & (1<<9)) {   // dec uncorrectable
        val0 = (1<<9);
        writel(val0, NF2INTR);
	(*(volatile int *)(0x43ffF088)) = 0x39;	//step
        cInt        &= ~(1<<9);
    }
    else if (cInt & (1<<4)) {   // ADMA Line int
        val0 = (1<<4);
        writel(val0, NF2INTR);
	// set new adma address
        val0 = readl(NF2ACADR);
	writel(val0, NF2AADR);
	val0 = 1<<1;     // adma int ack.
        writel(val0, NF2INTA);
        cInt &= ~(1<<4);
    }
    else if (cInt & (1<<7)) {   // ADMA valid error int
        val0 = (1<<7);
        writel(val0, NF2INTR);
	// set new adma address
        val0 = readl(NF2ACADR) + 8;
	writel(val0, NF2AADR);	
	nf2_soft_reset(8);  // reset dma
        val0 = 1<<1;     // adma int ack.
        writel(val0, NF2INTA);
	cInt        &= ~(1<<7);
    }
    else if (cInt & (1<<6)) {   // SDMA page boundary  int
        val0 = (1<<6);
        writel(val0, NF2INTR);
	// set new sdma address
        val0 = 1<<2;     // sdma page boundary int ack.
        writel(val0, NF2INTA);
	(*(volatile int *)(0x43ffF088)) = 0x38;	//step
        cInt        &= ~(1<<6);
    }
    else if (cInt & (1<<16)) {   // SDMA block int
        val0 = (1<<16);
        writel(val0, NF2INTR);
	val0 = 1<<3;     // sdma block int ack.
        writel(val0, NF2INTA);
	cInt        &= ~(1<<16);
    }
    
    return cInt;
}

unsigned int nf2_ecc_cap (unsigned int ecc_type)
{
	unsigned int ecc_cap = 0;
	
    switch (ecc_type) {
        case 0 : {  ecc_cap = 2;} break;		//2bit
        case 1 : {  ecc_cap = 4;} break;		//4bit
        case 2 : {  ecc_cap = 8;} break;		//8bit
        case 3 : {  ecc_cap = 16;} break;		//16bit
        case 4 : {  ecc_cap = 24;} break;		//24bit
        case 5 : {  ecc_cap = 32;} break;		//32bit
        case 6 : {  ecc_cap = 40;} break;		//40bit
        case 7 : {  ecc_cap = 48;} break;		//48bit
        case 8 : {  ecc_cap = 56;} break;		//56bit
        case 9 : {  ecc_cap = 60;} break;		//60bit
        case 10 : {  ecc_cap = 64;} break;		//64bit
        case 11 : {  ecc_cap = 72;} break;		//72bit
        case 12 : {  ecc_cap = 80;} break;		//80bit
        case 13 : {  ecc_cap = 96;} break;		//96bit
        case 14 : {  ecc_cap = 112;} break;		//112bit
        case 15 : {  ecc_cap = 127;} break;		//127bit
        default : break;
    }	
    
    return ecc_cap;
}

int nf2_soft_reset(int num)
{
    volatile unsigned int tmp = 0;
    int time_tick_start = 0;
    int time_tick_cur = 0;
    int ret = 0;

    writel(num, NF2SFTR);
    time_tick_start = get_ticks();
    while(1)
    {
    	//TODO add time out check	    
    	time_tick_cur = get_ticks();
	if(time_tick_cur - time_tick_start > 500000)
	{
		ret = -1;
		break;
	}	
    	tmp = readl(NF2SFTR) & 0x1f;
    	if(tmp == 0)break;
    }

    return ret;
    
}


void trx_afifo_reg (unsigned int flag, unsigned int addr, unsigned int data)
{
	unsigned int value = 0;

	if (flag==0){   // 32-bit register write
		value = (0xf<<12) | (flag<<8) | addr;
		writel(value, NF2TFPT);
	    	value = data & 0xffff;
	    	writel(value, NF2TFPT);
	    	value = (data>>16) & 0xffff;
	    	writel(value, NF2TFPT);
	    }
	else if (flag==1){   // 16-bit register write
	    	value = (0xf<<12) | (flag<<8) | addr;
	    	writel(value, NF2TFPT);
	    	value = data & 0xffff;
	    	writel(value, NF2TFPT);
	    }

    	else {
        	value = (0xf<<12) | (flag<<8) | addr;
		writel(value, NF2TFPT);
	    }

}

/*
    @func   void | trx_afifo_enable | 
    @param	NULL
    @comm	allow write trx-afifo
    @xref
    @return NULL
*/
void trx_afifo_enable(void)
{
	volatile unsigned int tmp = 0;
	tmp = readl(NF2FFCE);
	tmp |= 0x1;     // allow write trx-afifo
	writel(tmp, NF2FFCE);
}

/*
    @func   void | trx_afifo_disable | 
    @param	NULL
    @comm	disable write trx-afifo
    @xref
    @return NULL
*/
void trx_afifo_disable(void)
{
	writel(0x0, NF2FFCE); // disable write trx-afifo
}

/*
    @func   void | trx_nand_cs | 
    @param	NULL
    @comm	nand cs select
    @xref
    @return 1: finish, 0: not finish
*/
void trx_nand_cs(int valid)
{
	unsigned int val = 0;

	if(valid == 1)
	{
		val = ~(1<<0);  // cs0 valid
		writel(val, NF2CSR);
	}
	else
	{
		val = 0xf;
		writel(val, NF2CSR);
	}	
}

/*
    @func   void | trx_afifo_start | 
    @param	NULL
    @comm	start trx-afifo
    @xref
    @return NULL
*/
void trx_afifo_start(void)
{
	writel(0x1, NF2STRR); // start trx-afifo
}

/*
    @func   void | trx_afifo_is_finish | 
    @param	which your major data
    @comm	start trx-afifo
    @xref
    @return 1: finish, 0: not finish
*/
int trx_afifo_is_finish(int major)
{
	unsigned int val = 0;

	val = readl(NF2INTR) & major;
	return val;    
}

/*
    @func   void | trx_afifo_intr_clear | 
    @param	NULL
    @comm	clear nand all interrupt
    @xref
    @return 1: finish, 0: not finish
*/
void trx_afifo_intr_clear(void)
{
	writel(0xffffff, NF2INTR); // clear status
}

/*
    @func   void | trx_afifo_read_state | 
    @param	index
    @comm	return NF2STAR0 or NF2STAR1
    @xref
    @return 1: finish, 0: not finish
*/
int trx_afifo_read_state(int index)
{
	switch(index)
	{	
	case 0:
		return readl(NF2STSR0);
	case 1:
		return readl(NF2STSR1);
	case 2:
		return readl(NF2STSR2);
	case 3:
		return readl(NF2STSR3);
	}
	return -1;	
}

int nf2_afifo_reset(void)
{
	volatile unsigned int tmp = 0;
	int time_tick_start = 0;
	int time_tick_cur = 0;
	int ret = 0;

	writel(0x10, NF2SFTR);

	time_tick_start = get_ticks();
    	while(1)
    	{	
    	    	time_tick_cur = get_ticks();

    	    	if(time_tick_cur - time_tick_start > 500000)
    	    	{
    	    		ret = -1;
    	    		break;
    	    	}	
    		tmp = readl(NF2SFTR) & 0x1f;
    		if(tmp == 0)break;
    	}

    	return ret;
}

/*
    @func   void | trx_afifo_etd | 
    @param	NULL
    @comm	nand Extend-Data op
    @xref
    @return NULL
*/
void trx_afifo_etd (unsigned int type, unsigned int dest, unsigned int len)
{
	unsigned int value = 0;
	value = (0x2<<14) | (len<<2) | (type<<1) | dest;
	writel(value, NF2TFPT);
	return;
}

void cpu_afifo_disable(void)
{
	volatile unsigned int tmp = 0;
	tmp = readl(NF2FFCE);
	tmp &= ~0x2;     // not allow cpu access afifo
	writel(tmp, NF2FFCE);
	return;	
}

void cpu_afifo_enable(void)
{
	volatile unsigned int tmp = 0;
	tmp = readl(NF2FFCE);
	tmp |= 0x2;     // allow cpu access afifo
	writel(tmp, NF2FFCE);	
	return;
}

/*
    @func   void | trx_afifo_nop | 
    @param	NULL
    @comm	nand nop cmd
    @xref
    @return NULL
*/
void trx_afifo_nop (unsigned int flag, unsigned int nop_cnt)
{
	unsigned int value = 0;

	value = ((flag & 0xf)<<10) | (nop_cnt & 0x3ff);
	writel(value, NF2TFPT);
}

/*
    @func   void | trx_afifo_ncmd | 
    @param	NULL
    @comm	nand Normal CMD/ADR/DAT op
    @xref
    @return NULL
*/
void trx_afifo_ncmd (unsigned int flag, unsigned int type, unsigned int wdat)
{
	unsigned int value = 0;
	
	value = (0x1<<14) | ((flag & 0xf) <<10) | (type<<8) | wdat;
	writel(value, NF2TFPT);
}

unsigned int nf2_secc_cap (unsigned secc_type)
{
	unsigned int secc_cap = 0;
	
	switch(secc_type)
	{
	case 0: { secc_cap = 4;} break;
	case 1: { secc_cap = 8;} break;
	default: break;
	}
	
	return secc_cap;
}

void nf2_secc_cfg(int secc_used, int secc_type, int rsvd_ecc_en)
{
	volatile unsigned int tmp;
	unsigned int secc_cap;
	
	tmp = readl(NF2ECCC);
	tmp |= (secc_type <<17) | (secc_used <<16) | (rsvd_ecc_en<<3);
	writel(tmp, NF2ECCC);
	
	secc_cap = nf2_secc_cap (secc_type);
	tmp = readl(NF2ECCLEVEL);
	tmp &= ~(0xf<<16);
	tmp |= (secc_cap << 16);
	writel(tmp, NF2ECCLEVEL);
}

void nf2_ecc_cfg(int ecc_enable, int page_mode, int ecc_type, int eram_mode, 
		 int tot_ecc_num, int tot_page_num,  int trans_dir, int half_page_en)
{
	unsigned int tmp = 0;
	unsigned int ecc_cap = 0;	
	unsigned int val = 0;
	
	val  = (half_page_en<<18) | (eram_mode <<12) | (page_mode<<11) | (3<<8) | (ecc_type<<4) | (trans_dir<<1) | ecc_enable;
    	writel(val, NF2ECCC);
    
	val  = ((tot_ecc_num)<<16) | (tot_page_num);
	writel(val, NF2PGEC);
	
	ecc_cap = nf2_ecc_cap(ecc_type);
	tmp = readl(NF2ECCLEVEL);
	tmp &= ~(0x7f<<8);
	tmp |= (ecc_cap << 8);
	writel(tmp, NF2ECCLEVEL);
	nf_dbg("NF2ECCLEVEL = 0x%x\n", tmp);
}

int nf2_ecc_num (int ecc_type, int page_num, int half_page_en)
{
    int kb_cnt = 0;
    int ecc_num = 0;

    kb_cnt = (page_num >> 10);

    switch (ecc_type) {
        case 0 : {  ecc_num = 4*kb_cnt;} break;			//2bit
        case 1 : {  ecc_num = 8*kb_cnt;} break;			//4bit
        case 2 : {  ecc_num = 16*kb_cnt;} break;		//8bit
        case 3 : {  ecc_num = 28*kb_cnt;} break;		//16bit
        case 4 : {  ecc_num = 44*kb_cnt;} break;		//24bit
        case 5 : {  ecc_num = 56*kb_cnt;} break;		//32bit
        case 6 : {  ecc_num = 72*kb_cnt;} break;		//40bit
        case 7 : {  ecc_num = 84*kb_cnt;} break;		//48bit
        case 8 : {  ecc_num = 100*kb_cnt;} break;		//56bit
        case 9 : {  ecc_num = 108*kb_cnt;} break;		//60bit
        case 10 : {  ecc_num = 112*kb_cnt;} break;		//64bit
        case 11 : {  ecc_num = 128*kb_cnt;} break;		//72bit
        case 12 : {  ecc_num = 140*kb_cnt;} break;		//80bit
        case 13 : {  ecc_num = 168*kb_cnt;} break;		//96bit
        case 14 : {  ecc_num = 196*kb_cnt;} break;		//112bit
        case 15 : {  ecc_num = 224*kb_cnt;} break;		//127bit
        default : break;
    }
    
    if(half_page_en == 1)
    {
    	ecc_num = ecc_num << 1;
    }


    return ecc_num;
}


void nf2_addr_cfg(int row_addr0, int col_addr, int ecc_offset, int busw)
{
	unsigned int tmp;
       	
	if(busw == 16){
		col_addr = col_addr >> 1;
		ecc_offset = ecc_offset >> 1;
	}

	tmp = (ecc_offset<<16) | col_addr;

	writel(row_addr0, NF2RADR0);
	writel(0x0, NF2RADR1);
	writel(0x0, NF2RADR2);
	writel(0x0, NF2RADR3);
	writel(tmp, NF2CADR);	
}

void nf2_sdma_cfg(int ch0_adr, int ch0_len, int ch1_adr, int ch1_len, int sdma_2ch_mode)
{
	int dma_bst_len     = 1024;
	int blk_number		= 1;
	int adma_mode		= 0;	//SDMA
    	int bufbound        = 0;
    	int bufbound_chk    = 0;    // not check page address boundary
    	int dma_enable      = 1;
	unsigned int value = 0;

	writel(ch0_adr, NF2SADR0);
	writel(ch1_adr, NF2SADR1);
	writel((ch1_len<<16 | ch0_len), NF2SBLKS);
   
	value = (dma_bst_len<<16) | blk_number;
       	writel(value, NF2SBLKN);	
    
	value = (bufbound<<4) | (bufbound_chk<<3) | (sdma_2ch_mode<<2) | (adma_mode<<1) | dma_enable;
	writel(value, NF2DMAC);
    
   	return;
}

int nf2_check_ecc(int page_num, int *fixed_bits)
{
	int nf2_ecc_info[10];
	int block_cnt = 0;
	int i = 0, j = 0, k = 0;
	int ret = 0;
	int fixed_bit = 0;

	nf2_ecc_info[0] = readl(NF2ECCINFO0);
	nf2_ecc_info[1] = readl(NF2ECCINFO1);
	nf2_ecc_info[2] = readl(NF2ECCINFO2);
	nf2_ecc_info[3] = readl(NF2ECCINFO3);
	nf2_ecc_info[4] = readl(NF2ECCINFO4);
	nf2_ecc_info[5] = readl(NF2ECCINFO5);
	nf2_ecc_info[6] = readl(NF2ECCINFO6);
	nf2_ecc_info[7] = readl(NF2ECCINFO7);
	nf2_ecc_info[8] = readl(NF2ECCINFO8);

	*fixed_bits = 0;	
	block_cnt = page_num>>10;
	for(i=0; i<block_cnt; i++)
	{
		nf_dbg("nf2_ecc_info[8] = 0x%x\n", nf2_ecc_info[8]);
		if(nf2_ecc_info[8] & (1<<i))
		{
			ret = -1;
			ecc_add_stat(0, -1);		// register mecc bits
			break;
		}
		else
		{
			j = i>>2;
			k = i%4;
			switch(k)
			{
			case 0:
				fixed_bit = nf2_ecc_info[j]& 0x7f;
				break;
			case 1:
				fixed_bit = (nf2_ecc_info[j]& 0x7f00) >> 8;
				break;
			case 2:
				fixed_bit = (nf2_ecc_info[j]& 0x7f0000) >> 16;
				break;
			case 3:
				fixed_bit = (nf2_ecc_info[j]& 0x7f000000) >> 24;
				break;	
			}
			ecc_add_stat(0, fixed_bit);		// register mecc bits
		}	
		*fixed_bits += fixed_bit;	
	}
	
	return ret;
}

int nf2_check_secc(int *fixed_bits)
{
	int nf2_secc_info = 0;
	
	nf2_secc_info = readl(NF2ECCINFO9);
	if(nf2_secc_info & 0x40) {
		ecc_add_stat(1, -1);		// register secc bits
		return -1;
	}
	else
	{
		if(nf2_secc_info & 0x80)
		{
			*fixed_bits = nf2_secc_info & 0xf;
		}	
		ecc_add_stat(1, *fixed_bits);		// register secc bits
	}	
	
	return 0;
}
		
int nf2_set_polynomial(int polynomial)
{
	writel(polynomial, NF2RANDMP);	
	return 1;
}

/*
    @func   void | nf2_erase | 
    @param	NULL
    @comm	cs , address
    @xref
    @return 1: finish, 0: not finish
*/
int nf2_erase(int cs, int addr, int cycle)
{
	int time_tick_start = 0;
        int time_tick_cur = 0;
	int ret = 0;
	int i = 0;

	ret = nf2_soft_reset(1);
    	if(ret != 0)
		return -1;

	writel(addr, NF2RADR0); // row addr 0
 
	//step : program trx-afifo
    
	trx_afifo_enable(); 
	trx_afifo_ncmd(0x0, 0x0, 0x60);    // ERASE CMD0
#if 0
	trx_afifo_ncmd((0x8 | (cycle - 3)), 0x1, 0x04);    // 3 cycle addr of row_addr_0
#else
	for(i=0; i<(cycle-2); i++){
		trx_afifo_ncmd(0x0, 0x1, ((addr & (0xff << (i * 8))) >> i * 8));    // write addr
		
	}
#endif	
	trx_afifo_ncmd(0x2, 0x0, 0xD0);    // erase CMD1,wait for rnb ready and check status
	trx_afifo_disable();

    	trx_nand_cs(1);  // cs0 valid
    	trx_afifo_start(); // start trx-afifo

    	// wait for trx-fsm finish and erase successful
	time_tick_start = get_ticks();
	while(1)
    	{    	
		time_tick_cur = get_ticks();
		if(time_tick_cur - time_tick_start > 500000)
		{
			ret = -1;
			break;
		}
    		if(trx_afifo_is_finish(0x801) == 0x801)
    			break;
    	}	
    	
    	trx_afifo_intr_clear();
    	trx_nand_cs(0);

	return ret;
}

/**
 * return 0 successful, return -1 failed
*/
int nf2_page_op(int ecc_enable, int page_mode, int ecc_type, int rsvd_ecc_en,
                    int page_num, int sysinfo_num, int trans_dir, int row_addr,
		    int ch0_adr, int ch0_len, int secc_used, int secc_type,
                    int half_page_en, int main_part, int cycle, int randomizer,
		    uint32_t *data_seed, uint16_t *sysinfo_seed,  uint16_t *ecc_seed, uint16_t *secc_seed,
		    uint16_t *data_last1K_seed, uint16_t *ecc_last1K_seed, int *ecc_unfixed, int *secc_unfixed,
		    int ch1_adr, int ch1_len, int sdma_2ch_mode, int busw)
{ 
	int ecc_num = 0;
	int secc_num = 0;
	int spare_num = 0;
	int rsvd_num = 4;
	int ecc_offset = 0;
	int secc_offset = 0;
	int tot_ecc_num = 0;
	int tot_page_num = 0;
	int cur_ecc_offset = 0;
	int col_addr = 0;
	int eram_mode = 0;
	int time_tick_start = 0;
	int time_tick_cur = 0;
	int fixed_bits = 0;
	int value = 0;
	int ecc_ret = 0;
	int ret = 0;

	*ecc_unfixed = 0x0;
	*secc_unfixed = 0x0;

	ret = nf2_soft_reset(1);
	if(ret != 0)
		return -1;
		
	spare_num = sysinfo_num + rsvd_num;

	if(randomizer == 1)
	{
		nf_dbg("randomizer enable\n");
		writel(0x1, NF2RANDME);	
	}
	else
	{
		nf_dbg("randomizer disable\n");
		writel(0x0, NF2RANDME);	
	}	

	if(page_mode == 0) //whole page op
	{
		col_addr = 0;
    	
		if(ecc_enable == 1)
		{
			if(randomizer == 1)
			{
				writel(data_seed[0] ,NF2DATSEED0);
				writel(data_seed[1] ,NF2DATSEED1);
				writel(data_seed[2] ,NF2DATSEED2);
				writel(data_seed[3] ,NF2DATSEED3);	
				writel((ecc_seed[1]<<16 | ecc_seed[0]) ,NF2ECCSEED0);
				writel((ecc_seed[3]<<16 | ecc_seed[2]) ,NF2ECCSEED1);
				writel((ecc_seed[5]<<16 | ecc_seed[4]) ,NF2ECCSEED2);
				writel((ecc_seed[7]<<16 | ecc_seed[6]) ,NF2ECCSEED3);
			}	
			ecc_num = nf2_ecc_num (ecc_type, page_num, half_page_en);
			ecc_offset = page_num + spare_num;
    			ch0_len = page_num + spare_num;
			tot_page_num = ch0_len;
			cur_ecc_offset = ecc_offset;
			if(secc_used == 1)
			{
				if(secc_type == 0)
				{
					secc_num = 8;
				}
				else
				{
					secc_num = 12;
				}	
				tot_ecc_num = ecc_num + secc_num;
				secc_offset = ecc_offset + ecc_num;

			}
			else
			{
				tot_ecc_num = ecc_num;
				secc_num = 0;
				secc_offset = 0;
			}	
    		}
		else
		{
			if(randomizer == 1)
			{
				writel(data_seed[0] ,NF2DATSEED0);
				writel(data_seed[1] ,NF2DATSEED1);
				writel(data_seed[2] ,NF2DATSEED2);
				writel(data_seed[3] ,NF2DATSEED3);	
			}	
			ecc_num = 0;
			ecc_offset = 0;
			secc_num = 0;
			secc_offset = 0;
			ch0_len = page_num + spare_num;
			tot_page_num = ch0_len;
			tot_ecc_num = 0;
			secc_used = 0;
			cur_ecc_offset = ecc_offset;
		}	
    	}		
	else // main page or spare op
	{
    
		if(main_part == 1)//main range
		{
			col_addr = 0;
			if(ecc_enable == 1)				
			{
				if(randomizer == 1)
				{
					writel(data_seed[0] ,NF2DATSEED0);
					writel(data_seed[1] ,NF2DATSEED1);
					writel(data_seed[2] ,NF2DATSEED2);
					writel(data_seed[3] ,NF2DATSEED3);	
					writel((ecc_seed[1]<<16 | ecc_seed[0]) ,NF2ECCSEED0);
					writel((ecc_seed[3]<<16 | ecc_seed[2]) ,NF2ECCSEED1);
					writel((ecc_seed[5]<<16 | ecc_seed[4]) ,NF2ECCSEED2);
					writel((ecc_seed[7]<<16 | ecc_seed[6]) ,NF2ECCSEED3);
				}	
				ecc_num = nf2_ecc_num (ecc_type, page_num, half_page_en);
				ecc_offset = page_num + spare_num;
				ch0_len  = page_num;
				tot_page_num = ch0_len;
				tot_ecc_num = ecc_num;
				secc_used = 0;
				secc_num = 0;
				secc_offset = 0;
				cur_ecc_offset = ecc_offset;
			}
			else
			{
				if(randomizer == 1)
				{
					writel(data_seed[0] ,NF2DATSEED0);
					writel(data_seed[1] ,NF2DATSEED1);
					writel(data_seed[2] ,NF2DATSEED2);
					writel(data_seed[3] ,NF2DATSEED3);	
				}	
				ecc_num = 0;
				ecc_offset = 0;
				ch0_len  = page_num;
				tot_page_num = ch0_len;
				tot_ecc_num = 0;
				secc_used = 0;
				secc_num = 0;
				secc_offset = 0;
				cur_ecc_offset = ecc_offset;		
			}	
		}
		else //spare range
		{
			if(ecc_enable == 1)
			{
				ecc_num = nf2_ecc_num (ecc_type, page_num, half_page_en);
				ecc_offset = page_num + spare_num;
				if(secc_used == 1)
				{
					if(randomizer == 1)
					{
						writel((sysinfo_seed[1]<<16 | sysinfo_seed[0]) ,NF2DATSEED0);
						writel((sysinfo_seed[3]<<16 | sysinfo_seed[2]) ,NF2DATSEED1);
						writel((sysinfo_seed[5]<<16 | sysinfo_seed[4]) ,NF2DATSEED2);
						writel((sysinfo_seed[7]<<16 | sysinfo_seed[6]) ,NF2DATSEED3);
						writel((secc_seed[1]<<16 | secc_seed[0]) ,NF2ECCSEED0);
						writel((secc_seed[3]<<16 | secc_seed[2]) ,NF2ECCSEED1);
						writel((secc_seed[5]<<16 | secc_seed[4]) ,NF2ECCSEED2);
						writel((secc_seed[7]<<16 | secc_seed[6]) ,NF2ECCSEED3);
					}	

					if(secc_type == 0)
					{
						secc_num = 8;
					}
					else
					{
						secc_num = 12;
					}	
					ch0_len  = spare_num;
					tot_page_num = ch0_len;
					tot_ecc_num = secc_num;
					secc_offset = ecc_offset + ecc_num;
					cur_ecc_offset = secc_offset;
					col_addr = page_num;
				}
				else //use last 1K ecc
				{
					if(randomizer == 1)
					{
						writel((data_last1K_seed[1]<<16 | data_last1K_seed[0]) ,NF2DATSEED0);
						writel((data_last1K_seed[3]<<16 | data_last1K_seed[2]) ,NF2DATSEED1);
						writel((data_last1K_seed[5]<<16 | data_last1K_seed[4]) ,NF2DATSEED2);
						writel((data_last1K_seed[7]<<16 | data_last1K_seed[6]) ,NF2DATSEED3);
						writel((ecc_last1K_seed[1]<<16 | ecc_last1K_seed[0]) ,NF2ECCSEED0);
						writel((ecc_last1K_seed[3]<<16 | ecc_last1K_seed[2]) ,NF2ECCSEED1);
						writel((ecc_last1K_seed[5]<<16 | ecc_last1K_seed[4]) ,NF2ECCSEED2);
						writel((ecc_last1K_seed[7]<<16 | ecc_last1K_seed[6]) ,NF2ECCSEED3);
					}	

					ch0_len = spare_num;
					tot_page_num = 1024 + spare_num;
					tot_ecc_num = ecc_num / (page_num>>10);
					cur_ecc_offset = ecc_offset + tot_ecc_num * ((page_num>>10) - 1);
					col_addr = page_num - 1024;	
				}	
			}
			else
			{
				if(randomizer == 1)
				{
					writel((sysinfo_seed[1]<<16 | sysinfo_seed[0]) ,NF2DATSEED0);
					writel((sysinfo_seed[3]<<16 | sysinfo_seed[2]) ,NF2DATSEED1);
					writel((sysinfo_seed[5]<<16 | sysinfo_seed[4]) ,NF2DATSEED2);
					writel((sysinfo_seed[7]<<16 | sysinfo_seed[6]) ,NF2DATSEED3);
				}	
				ch0_len = spare_num;
				tot_page_num = ch0_len;
				tot_ecc_num = 0;
				secc_used = 0;
				ecc_num = 0;
				ecc_offset = 0;
				secc_num = 0;
				secc_offset = 0;
				cur_ecc_offset = secc_offset;
				col_addr = page_num;
			}	

		}
    	}

	if(sdma_2ch_mode == 1)
	{
		ch0_len -= ch1_len;
	}	

	nf2_ecc_cfg(ecc_enable, page_mode, ecc_type, eram_mode, 
		 	tot_ecc_num, tot_page_num, trans_dir, half_page_en);
	nf_dbg("nf2_ecc_cfg %d, %d, %d, %d, %d, %d, %d, %d\n", ecc_enable, page_mode, ecc_type, eram_mode, tot_ecc_num, tot_page_num, trans_dir, half_page_en);

	nf2_secc_cfg(secc_used, secc_type, rsvd_ecc_en);
	nf_dbg("nf2_secc_cfg %d, %d, %d\n", secc_used, secc_type, rsvd_ecc_en);

   	nf2_sdma_cfg(ch0_adr, ch0_len, ch1_adr, ch1_len, sdma_2ch_mode);
	nf_dbg("nf2_sdma_cfg 0x%x, %d, 0x%x, %d, %d\n", ch0_adr, ch0_len, ch1_adr, ch1_len, sdma_2ch_mode);

	nf2_addr_cfg(row_addr, col_addr, cur_ecc_offset, busw);
	nf_dbg("nf2_addr_cfg 0x%x, 0x%x, %d, %d\n", row_addr, col_addr, cur_ecc_offset, busw);


	if(trans_dir == 0)//read op
	{
		writel(0x20, NF2INTE); // enable sdma finished interrupt
		writel(0x1, NF2FFCE); // allow write trx-afifo
		trx_afifo_ncmd(0x0, 0x0, 0x00);    // page read CMD0
		trx_afifo_ncmd((0x8 | (cycle -1)), 0x1, 0x0);    // 5 cycle addr of row_addr_0 & col_addr
		//trx_afifo_ncmd(0x1, 0x0, 0x30);    // page read CMD1, check rnb and read whole page
		trx_afifo_ncmd(0x4, 0x0, 0x30);    // page read CMD1, check rnb
		trx_afifo_nop(0x5, 0x7);           // nop and then read whole page, 7nop > 20ns for tRR PARAM
		trx_afifo_reg(0x8, 0x1, 0x0);      // row_addr_0 add 1	
		writel(0x0, NF2FFCE); // disable write trx-afifo
		writel(0xe, NF2CSR); // cs0 valid
		udelay(10);

    		if (ecc_enable) {
			writel(0x7, NF2STRR); // start trx-afifo, ecc and dma
    		} else {
    		 	writel(0x5, NF2STRR); // start trx-afifo,  and dma 
    		}

		time_tick_start = get_ticks();
		while(1)
		{
			time_tick_cur = get_ticks();
			if(time_tick_cur - time_tick_start > 500000)
			{
				printf("read timeout\n");
				ret = -1;
				break;
			}
			if(nf2_check_irq() == (0x1<<5))
			{
				ret = 0;
				break;	
			}
		}	
		time_tick_start = time_tick_cur - time_tick_start;
		nf_dbg("nf2_read_op time = %d\n", time_tick_start);

		value = readl(NF2INTR);
		writel(value, NF2INTR);
		writel(0xf, NF2CSR); //cs invalid

		ecc_ret = nf2_check_ecc(page_num, &fixed_bits);
		if(ecc_ret != 0)
		{
			*ecc_unfixed = 0xff;
//			printf("ecc unfixed\n");
		}	
		if(secc_used == 1)
		{
			ecc_ret = nf2_check_secc(&fixed_bits);
			if(ecc_ret != 0)
			{
				*secc_unfixed = 0xff;
//				printf("secc unfixed 0x%x, 0x%x, 0x%x, 0x%x\n", readl(NF2DEBUG8), readl(NF2DEBUG9), readl(NF2DEBUG10), readl(NF2DEBUG11));
			}	
		}	
	}	
	else //write op
	{
		writel(0x1001, NF2INTE); // enable trx-empty interrupt.
		writel(0x1, NF2FFCE); // allow write trx-afifo
		trx_afifo_ncmd(0x0, 0x0, 0x80);    // page program CMD0
		trx_afifo_ncmd((0x8 | (cycle -1)), 0x1, 0x0);    // 5 cycle addr of row_addr_0 & col_addr
		//trx_afifo_nop(0x3, 0x0);           // nop and then write whole page
		trx_afifo_nop(0x3, 0x70);           // nop and then write whole page
		trx_afifo_ncmd(0x2, 0x0, 0x10);    // page program CMD1,wait for rnb ready and check status
		trx_afifo_reg(0x8, 0x1, 0x0);      // row_addr_0 add 1
		writel(0x0, NF2FFCE); //disable write trx-afifo
		writel(0xe, NF2CSR); //cs0 valid
		udelay(10);

    		if (ecc_enable) {
			writel(0x7, NF2STRR);//start trx-afifo, ecc and dma
    		} else {
			writel(0x5, NF2STRR);//start trx-afifo,  and dma
    		}
  		
		time_tick_start = get_ticks();
		while(1)
		{
			time_tick_cur = get_ticks();
			if(time_tick_cur - time_tick_start > 500000)
			{
				printf("write op timeout 0x%x, 0x%x, isfifo 0x%x, osfifo 0x%x, trxfifo 0x%x, afifo 0x%x, curdma 0x%x, ch0adr 0x%x, ch1adr 0x%x, \
						0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n",
					readl(NF2INTR), readl(NF2WSTA), readl(NF2IFST), readl(NF2OFST), readl(NF2TFST2), readl(NF2AFST), readl(NF2SCADR), readl(NF2SADR0), readl(NF2SADR1),
					readl(NF2DEBUG0), readl(NF2DEBUG1), readl(NF2DEBUG2), readl(NF2DEBUG3), readl(NF2DEBUG4), readl(NF2DEBUG5), readl(NF2DEBUG6), readl(NF2DEBUG7),
					readl(NF2DEBUG8), readl(NF2DEBUG9), readl(NF2DEBUG10), readl(NF2DEBUG11));

				ret = -1;
				break;
			}
			if(nf2_check_irq() == 1)
			{
				ret = 0;
				break;	
			}
		}	
		nf2_check_irq();
		time_tick_start = time_tick_cur - time_tick_start;
		nf_dbg("nf2_read_write time = %d\n", time_tick_start);
		value = readl(NF2INTR);
		writel(value, NF2INTR);
		writel(0xf, NF2CSR); //cs invalid
	}	

	return ret;

}

#if 0
int nand_readfeather(uint8_t buf[])
{
	int ret = 0;
	int feather = 0;
	int time_tick_start = 0;
	int time_tick_cur = 0;

	ret = nf2_soft_reset(1);
	if(ret != 0)
		return -1;
	trx_afifo_enable();
    	trx_afifo_ncmd(0x0, 0x0, 0xEE);    // read id CMD
    	trx_afifo_ncmd(0x7, 0x1, 0x00);    // ADDRS
    	trx_afifo_ncmd(0xB, 0x3, 0x00);     // read data    
	trx_afifo_disable();
    		
    	trx_nand_cs(1);
    	trx_afifo_start();
    	
	time_tick_start = get_ticks();	
  	while(1)
    	{
		time_tick_cur = get_ticks();
		if(time_tick_cur - time_tick_start > 500000)
		{
			ret = -1;
			break;
		}
    		if(trx_afifo_is_finish(1) == 1)
    			break;
    	}	
    		
    	trx_afifo_intr_clear();
    	trx_nand_cs(0);
    		
    	feather = trx_afifo_read_state(0);

	printf("feather = 0x%x\n", feather);

	return ret;
	
}
#endif

/* NAND driver specific implementation */
int nand_readid(uint8_t buf[], int interface, int timing, int rnbtimeout, int phyread, int phydelay, int busw)
{
	/* Read CDBG_NAND_ID_COUNT IDs into buffer
	 * If any error happened, return -1;
	 */

	int time_tick_start = 0;
	int time_tick_cur = 0;
	int read_id = 0;
	int ret = 0;

	/* interface is set to 0 when reading ID, warit oct31,2011 */
	interface = 0;

	module_reset(NF2_SYSM_ADDR);
	nf2_timing_init(interface, timing, rnbtimeout, phyread, phydelay, busw);
	ret = nf2_asyn_reset(0);
	if(ret != 0)
		return -1;
	ret = nf2_soft_reset(1);
	if(ret != 0)
		return -1;
	
	trx_afifo_enable();
	trx_afifo_ncmd(0x0, 0x0, 0x90);    // read id CMD
	trx_afifo_ncmd(0x0, 0x1, 0x00);    // ADDRS
	trx_afifo_nop (0, 8);				// NOP
	trx_afifo_ncmd(0xf, 0x3, 0x00);    // 8 read data    
	trx_afifo_disable();

	trx_nand_cs(1);
	trx_afifo_start();

	time_tick_start = get_ticks();	
	while(1)
	{
		time_tick_cur = get_ticks();

		if(time_tick_cur - time_tick_start > 500000)
		{
			ret = -1;
			break;
		}
		if(trx_afifo_is_finish(1) == 1)
			break;
	}	

	trx_afifo_intr_clear();
	trx_nand_cs(0);

	read_id = trx_afifo_read_state(0);

	buf[7] = read_id & 0xff;
	buf[6] = (read_id & 0xff00) >> 8;
	buf[5] = (read_id & 0xff0000) >> 16;
	buf[4] = (read_id & 0xff000000) >> 24;

	read_id = trx_afifo_read_state(1);
	buf[3] = read_id & 0xff;
	buf[2] = (read_id & 0xff00) >> 8;
	buf[1] = (read_id & 0xff0000) >> 16;
	buf[0] = (read_id & 0xff000000) >> 24;

#if 0
	printf("ID: %02x %02x %02x %02x %02x %02x %02x %02x\n",
				buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
#endif
	return ret;
}

