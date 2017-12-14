/***************************************************************************** 
** Copyright (c) 2015~2019 ShangHai Infotm Ltd all rights reserved. 
** 
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** Description: Belt connect driver & OS.
**
** Author:
**     Willie 
**      
** Revision History: 
** ----------------- 
** 0.1  11/25/2015
** 1.0	12/02/2015
** 1.1  01/15/2015    add dram DFI interface
*****************************************************************************/
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/cpufreq.h>
#include <linux/interrupt.h>
#include <linux/dma-mapping.h>
#include <linux/poll.h>
#include <linux/rtc.h>
#include <linux/gpio.h>

#include <asm/io.h>
#include <asm/irq.h>
#include <mach/hardware.h>
#include <mach/dpmu.h>
#include <mach/items.h>
#include <mach/nvedit.h>

#define SAMPLE_PER	100		//unit: ms
#define SAMPLE_NUM	8		//need 4, 8, 16, 32

//#define DPMU_DEBUG 
static int dev_irq_flag = 0;
static int all_irq_flag = 0;

typedef struct dpmu_irq {
	wait_queue_head_t irq_wq;
	int dpmu_condition;
}dpmu_irq_t;
dpmu_irq_t *dpmu_irq_n = NULL;

static long dpmu_dev_ioctl(struct file *file,
			unsigned int cmd, unsigned long arg)                       
{
	return 0;
}

int dpmu_dev_open(struct inode *fi, struct file *fp)
{
	return 0;
}

static int dpmu_dev_read(struct file *filp,
   char __user *buf, size_t length, loff_t *offset)
{
	return 0;
}

static uint32_t c0_wd[10] = {0}, c0_rd[10] = {0}, b1_wd[10] = {0}, b1_rd[10] = {0}, b3_wd[10] = {0}, b3_rd[10], b4_wd[10] = {0}, b4_rd[10] = {0}, b6_wd[10] = {0}, b6_rd[10] = {0}, dfi_wc[10] = {0}, dfi_rc[10] = {0};
static uint32_t c0_wb[10] = {0}, c0_rb[10] = {0}, b1_wb[10] = {0}, b1_rb[10] = {0}, b3_wb[10] = {0}, b3_rb[10], b4_wb[10] = {0}, b4_rb[10] = {0}, b6_wb[10] = {0}, b6_rb[10] = {0};
static int VAL_CNT = 0;
static int VAL_CNT_ALL = 0;
static const char *bus1_dev[BUS1_NUM] = {"ISP", "CAMIF", "MIPICSI", "IDS"};
static const char *bus3_dev[BUS3_NUM] = {"FODET", "ISPOST"};
static const char *bus4_dev[BUS4_NUM] = {"VENC_H1"};

//bus6 sub bus module
static const char *bus6_dev_sub[BUS6_SUB_NUM] = {"USB_Host", "USB_OTG", "SD1", "SD2", "SD0"};

//bus6 main module
static const char *bus6_dev_main[BUS6_MAIN_NUM] = {"SUB_BUS", "GDMA", "Ethernet", "DSP", "CRYPTO"};

static void dpmu_state(void)
{
#if 0
	printk("==================************cpu & bus dpmu user anysis start************===================\n");
	printk("======ordinal: writel burst count, read burst count, writel data count, read data count======\n");
	printk("cpu0: 0x%.8x, 0x%.8x, 0x%.8x, 0x%.8x\n", readl(CPU_WBSTC), readl(CPU_RBSTC), readl(CPU_WDATC), readl(CPU_RDATC));
	printk("bus1: 0x%.8x, 0x%.8x, 0x%.8x, 0x%.8x\n", readl(BUS1_WBSTC), readl(BUS1_RBSTC), readl(BUS1_WDATC), readl(BUS1_RDATC));
	printk("bus3: 0x%.8x, 0x%.8x, 0x%.8x, 0x%.8x\n", readl(BUS3_WBSTC), readl(BUS3_RBSTC), readl(BUS3_WDATC), readl(BUS3_RDATC));
	printk("bus4: 0x%.8x, 0x%.8x, 0x%.8x, 0x%.8x\n", readl(BUS4_WBSTC), readl(BUS4_RBSTC), readl(BUS4_WDATC), readl(BUS4_RDATC));
	printk("bus6: 0x%.8x, 0x%.8x, 0x%.8x, 0x%.8x\n", readl(BUS6_WBSTC), readl(BUS6_RBSTC), readl(BUS6_WDATC), readl(BUS6_RDATC));
	printk("cpu1: 0x%.8x, 0x%.8x, 0x%.8x, 0x%.8x\n", readl(CPU_WBSTC1), readl(CPU_RBSTC1), readl(CPU_WDATC1), readl(CPU_RDATC1));
	printk("DRAM W/R command counter reg: 0x%.8x, 0x%.8x\n", readl(DRAM_WCNT), readl(DRAM_RCNT));
	printk("==================************cpu & bus dpmu user anysis end**************===================\n\n");
#endif
	printk("======1%s[%d], readl MSK 0x%x, CMP 0x%x, VAL_CNT %d\n", __func__, __LINE__, readl(BUS1_IDMSK), readl(BUS1_IDCMP), VAL_CNT);
	printk("======3%s[%d], readl MSK 0x%x, CMP 0x%x\n", __func__, __LINE__, readl(BUS3_IDMSK), readl(BUS3_IDCMP));
	printk("======4%s[%d], readl MSK 0x%x, CMP 0x%x\n", __func__, __LINE__, readl(BUS4_IDMSK), readl(BUS4_IDCMP));
	printk("======6%s[%d], readl MSK 0x%x, CMP 0x%x\n", __func__, __LINE__, readl(BUS6_IDMSK), readl(BUS6_IDCMP));
	printk("====bus1: 0x%.8x, 0x%.8x\n", readl(BUS1_WDATC), readl(BUS1_RDATC));
	printk("====bus3: 0x%.8x, 0x%.8x\n", readl(BUS3_WDATC), readl(BUS3_RDATC));
	printk("====bus4: 0x%.8x, 0x%.8x\n", readl(BUS4_WDATC), readl(BUS4_RDATC));
	printk("====bus6: 0x%.8x, 0x%.8x\n", readl(BUS6_WDATC), readl(BUS6_RDATC));
	printk("====DFI:  0x%.8x, 0x%.8x\n", readl(DRAM_WCNT), readl(DRAM_WCNT));
}

static void reg_update(void)
{
	if(all_irq_flag != 1){
		c0_wd[VAL_CNT] = readl(CPU_WDATC);
		c0_rd[VAL_CNT] = readl(CPU_RDATC);
		b1_wd[VAL_CNT] = readl(BUS1_WDATC);
		b1_rd[VAL_CNT] = readl(BUS1_RDATC);
		b3_wd[VAL_CNT] = readl(BUS3_WDATC);
		b3_rd[VAL_CNT] = readl(BUS3_RDATC);
		b4_wd[VAL_CNT] = readl(BUS4_WDATC);
		b4_rd[VAL_CNT] = readl(BUS4_RDATC);
		b6_wd[VAL_CNT] = readl(BUS6_WDATC);
		b6_rd[VAL_CNT] = readl(BUS6_RDATC);
		c0_wb[VAL_CNT] = readl(CPU_WBSTC);
		c0_rb[VAL_CNT] = readl(CPU_RBSTC);
		b1_wb[VAL_CNT] = readl(BUS1_WBSTC);
		b1_rb[VAL_CNT] = readl(BUS1_RBSTC);
		b3_wb[VAL_CNT] = readl(BUS3_WBSTC);
		b3_rb[VAL_CNT] = readl(BUS3_RBSTC);
		b4_wb[VAL_CNT] = readl(BUS4_WBSTC);
		b4_rb[VAL_CNT] = readl(BUS4_RBSTC);
		b6_wb[VAL_CNT] = readl(BUS6_WBSTC);
		b6_rb[VAL_CNT] = readl(BUS6_RBSTC);
		dfi_wc[VAL_CNT] = readl(DRAM_WCNT);
		dfi_rc[VAL_CNT] = readl(DRAM_RCNT);
	}
	VAL_CNT++;
	VAL_CNT_ALL++;
	if(VAL_CNT == SAMPLE_NUM)
		VAL_CNT = 0;
}

#define iswhite(x) (x == ' ' || x == '\t')
static int get_para(char * buf, const char *desc, int para, int all)
{
    const char *p;
    char *q;
    int i, n = -1;

    for(p = desc, i = 0, q = buf;
            (*p != '\0') && (i < 1024);
            p++, i++) {

        if((p != desc && iswhite(*(p - 1)) && !iswhite(*p))
                || (p == desc && !iswhite(*p)))
            n += (all && (n == para))? 0: 1;

        if(n == para) {
            *q++ = *p;
//            printk(KERN_ERR "n: %d, c=%c\n", n, *p);
            if((!all && iswhite(*(p+1)))
                    || *(p+1) == '\n' || !*(p+1)) {
//                printk(KERN_ERR "%d: %d: %d\n", (!all && iswhite(*(p+1))),
//                        *(p+1) == '\n', !*(p+1));
                *q = 0;
                return 0;
            }
        }
    }

    *buf = 0;
    return -1;
}

static void set_sample(int period)
{
	uint32_t f = 0, time = 0;
	/*set dpmu sample period
	 *dpmu freq 400M, 2.5ns
	 *sample period 16 times of this register define by alin
	 *2.5ns * 16 * 0xee6b280 = 10s
	 *so 10s sample period: writel(0xee6b280, DPMU_SPRD);
	 *use item calcuate: period = time * 16 *(1/freq)
	 *62500 = 1000000/16
	 * */
	//printk("====0DPMU_SPRD 0x%x\n", readl(DPMU_SPRD));
    //controller clock = 1/2 dram clock
	f = item_integer("dram.freq", 0)/2; 
	time = period * 1000 / 16 * f;
	writel(time, DPMU_SPRD);
	//printk("====1DPMU_SPRD 0x%x\n", readl(DPMU_SPRD));

}

static uint64_t ave(uint32_t *addr)
{
	uint32_t i = 0;
	uint64_t sum = 0;
	for(i = 0; i<SAMPLE_NUM; i++){
		//printk("addr%d 0x%x ", i, addr[i]);
		sum	+= addr[i];
	}
	//printk("\n");
	return (sum >> 3);
}

/*calc result: KB/s		8---> data count(data is 64bit ubit), burst count unit,  1024--->Byte to Meg, 1000/SAMPLE_PER result unit*/
//#define calc(x) (x*8/16/1024*1000/SAMPLE_PER)
static uint64_t calc(uint32_t ori)
{
	uint64_t ret = 0;
	ret = ori / 1024 * 1000 / SAMPLE_PER * 8;
	//printk("======calc ori 0x%x, ret %lld\n", ori, ret);
	return ret;
}

static uint64_t calc_B(uint32_t ori)
{
	uint64_t ret = 0;
	ret = ori * 1000 / SAMPLE_PER;
	//printk("======calc ori 0x%x, ret %lld\n", ori, ret);
	return ret;
}

#define CA_M(x)  ((x)/1024)
#define CA_B(x)  ((x) % 1024*100/1024)

static void dpmu_bus(void)
{
	//uint32_t c0 = 0, b1 = 0, b2 = 0, b3 = 0, b4 =0, c1 = 0;
	printk("=============************cpu & bus dpmu user anysis start************==============\n");
	printk(" cpu:  W: %4lld.%.2lldMB/s	R: %4lld.%.2lldMB/s\n", CA_M(calc(ave(c0_wd))), CA_B(calc(ave(c0_wd))), CA_M(calc(ave(c0_rd))), CA_B(calc(ave(c0_rd))));
	printk("bus1:  W: %4lld.%.2lldMB/s	R: %4lld.%.2lldMB/s\n", CA_M(calc(ave(b1_wd))), CA_B(calc(ave(b1_wd))), CA_M(calc(ave(b1_rd))), CA_B(calc(ave(b1_rd))));
	printk("bus3:  W: %4lld.%.2lldMB/s	R: %4lld.%.2lldMB/s\n", CA_M(calc(ave(b3_wd))), CA_B(calc(ave(b3_wd))), CA_M(calc(ave(b3_rd))), CA_B(calc(ave(b3_rd))));
	printk("bus4:  W: %4lld.%.2lldMB/s	R: %4lld.%.2lldMB/s\n", CA_M(calc(ave(b4_wd))), CA_B(calc(ave(b4_wd))), CA_M(calc(ave(b4_rd))), CA_B(calc(ave(b4_rd))));
	printk("bus6:  W: %4lld.%.2lldMB/s	R: %4lld.%.2lldMB/s\n", CA_M(calc(ave(b6_wd))), CA_B(calc(ave(b6_wd))), CA_M(calc(ave(b6_rd))), CA_B(calc(ave(b6_rd))));
	printk("=============************cpu & bus dpmu user anysis end************==============\n");
}

static void dpmu_dfi(void)
{
	//uint32_t c0 = 0, b1 = 0, b2 = 0, b3 = 0, b4 =0, c1 = 0;
	printk("=============************cpu & bus dpmu user anysis start************==============\n");
	printk(" cpu:  W: %4lld.%.2lldMB/s	R: %4lld.%.2lldMB/s\n", CA_M(calc(ave(c0_wd))), CA_B(calc(ave(c0_wd))), CA_M(calc(ave(c0_rd))), CA_B(calc(ave(c0_rd))));
	printk("bus1:  W: %4lld.%.2lldMB/s	R: %4lld.%.2lldMB/s\n", CA_M(calc(ave(b1_wd))), CA_B(calc(ave(b1_wd))), CA_M(calc(ave(b1_rd))), CA_B(calc(ave(b1_rd))));
	printk("bus3:  W: %4lld.%.2lldMB/s	R: %4lld.%.2lldMB/s\n", CA_M(calc(ave(b3_wd))), CA_B(calc(ave(b3_wd))), CA_M(calc(ave(b3_rd))), CA_B(calc(ave(b3_rd))));
	printk("bus4:  W: %4lld.%.2lldMB/s	R: %4lld.%.2lldMB/s\n", CA_M(calc(ave(b4_wd))), CA_B(calc(ave(b4_wd))), CA_M(calc(ave(b4_rd))), CA_B(calc(ave(b4_rd))));
	printk("bus6:  W: %4lld.%.2lldMB/s	R: %4lld.%.2lldMB/s\n", CA_M(calc(ave(b6_wd))), CA_B(calc(ave(b6_wd))), CA_M(calc(ave(b6_rd))), CA_B(calc(ave(b6_rd))));
	printk("c0_B:  W: %lld		R: %lld\n", 		calc_B(ave(c0_wb))*8, calc_B(ave(c0_rb))*8);
	printk("b1_B:  W: %lld		R: %lld\n", 		calc_B(ave(b1_wb))*8, calc_B(ave(b1_rb))*8);
	printk("b3_B:  W: %lld		R: %lld\n", 		calc_B(ave(b3_wb))*8, calc_B(ave(b3_rb))*8);
	printk("b4_B:  W: %lld		R: %lld\n", 		calc_B(ave(b4_wb))*8, calc_B(ave(b4_rb))*8);
	printk("b6_B:  W: %lld		R: %lld\n", 		calc_B(ave(b6_wb))*8, calc_B(ave(b6_rb))*8);
	printk("DFI:   W: %lld		R: %lld\n", 		calc_B(ave(dfi_wc)), calc_B(ave(dfi_rc)));
	printk("=============************cpu & bus dpmu user anysis end************==============\n");
}

static void calc_bus_l(uint32_t mask_id, uint32_t compare_id)
{
	writel(mask_id,		BUS1_IDMSK);
	writel(compare_id,	BUS1_IDCMP);
	writel(mask_id,		BUS3_IDMSK);
	writel(compare_id,	BUS3_IDCMP);
	writel(mask_id,		BUS4_IDMSK);
	writel(compare_id,	BUS4_IDCMP);
}

static void calc_bus6(uint32_t mask_id, uint32_t compare_id)
{
	writel(mask_id,		BUS6_IDMSK);
	writel(compare_id,	BUS6_IDCMP);
}

static void calc_id(uint32_t id)
{
	int ret = 0;
   	uint32_t time_out_cnt = 0;
	
	writel(0, DPMU_INTE);
	writel(0, DPMU_EN);
	//set_sample(SAMPLE_PER);
	if(id == 0){
		calc_bus_l(ID_NMSK, ID_NCMP);
		calc_bus6(ID_NMSK, ID_NCMP);
	} else {
		calc_bus_l(MSK_L, (id-1)<<CMP_L_OFFSET);
		calc_bus6(MSK_BUS6_MAIN, (id-1)<<CMP_MAIN_OFFSET);
	}

	writel(1, DPMU_EN);
	writel(1, DPMU_INTE);
	
	dpmu_irq_n->dpmu_condition = 0;
	VAL_CNT = 0;
	time_out_cnt = wait_event_timeout(dpmu_irq_n->irq_wq, dpmu_irq_n->dpmu_condition,  HZ * SAMPLE_PER / 1000 * (SAMPLE_NUM + 1));
	if (time_out_cnt == 0) {
		printk("dpmu irq %d timeout\n", dpmu_irq_n->dpmu_condition);
		if (dpmu_irq_n->dpmu_condition == 0)
			ret = -EIO;
	}

	if(id == 0){
		c0_wd[0] = readl(CPU_WDATC);
		c0_rd[0] = readl(CPU_RDATC);
	}

	if(id < (BUS1_NUM+1)){
		b1_wd[id] = readl(BUS1_WDATC);
		b1_rd[id] = readl(BUS1_RDATC);
	}
	if(id < (BUS3_NUM+1)){
		b3_wd[id] = readl(BUS3_WDATC);
		b3_rd[id] = readl(BUS3_RDATC);
	}
	if (id < (BUS4_NUM+1)){
		b4_wd[id] = readl(BUS4_WDATC);
		b4_rd[id] = readl(BUS4_RDATC);
	}
	if (id < (BUS6_MAIN_NUM+1)){
		b6_wd[id] = readl(BUS6_WDATC);
		b6_rd[id] = readl(BUS6_RDATC);
	}
}

static int device_cal(int bus_msk, uint32_t mask_id, uint32_t compare_id)
{
	int time_out_cnt = 0, ret = 0;

	writel(0, DPMU_INTE);
	writel(0, DPMU_EN);
	writel(mask_id,		(void *)bus_msk);
	writel(compare_id,	(void *)(bus_msk + 0x4));
	writel(1, DPMU_EN);
	writel(1, DPMU_INTE);
	mdelay(10);
	dpmu_irq_n->dpmu_condition = 0;
	VAL_CNT = 0; 
	
	time_out_cnt = wait_event_timeout(dpmu_irq_n->irq_wq, dpmu_irq_n->dpmu_condition,  HZ * SAMPLE_PER / 1000 * (SAMPLE_NUM + 1));
	if (time_out_cnt == 0) {
		printk("dpmu irq %d timeout\n", dpmu_irq_n->dpmu_condition);
		if (dpmu_irq_n->dpmu_condition == 0)
			ret = -EIO;
	}
	
	return ret;
}

static int cmp_id_l[10] = {0x0, 0x100, 0x200, 0x300};
static int cmp_id_bus6_main[10] = {0x0, 0x1<<6, 0x2<<6, 0x3<<6, 0x4<<6};
static int cmp_id_bus6_sub[10] = {0x0, 0x1<<3, 0x2<<3, 0x3<<3, 0x4<<3};

static int dpmu_device(char *str)
{
	char buf[64];
	uint32_t i = 0;
	int ret = 0;

	get_para(buf, str, 2, 0);
	for(i=0; i<BUS1_NUM; i++){
		if(strncmp(buf, bus1_dev[i], 8) == 0){
			ret = device_cal((int)BUS1_IDMSK, MSK_L, cmp_id_l[i]);
			if (ret != 0)
				goto
				   err_dev;	
			printk("bus1:%d: 	%s:W: %4lld.%.2lldMB/s   R: %4lld.%.2lldMB/s\n", i, bus1_dev[i], CA_M(calc(ave(b1_wd))), CA_B(calc(ave(b1_wd))), CA_M(calc(ave(b1_rd))), CA_B(calc(ave(b1_rd))));
			goto find_dev;
		}
	}
	for(i=0; i<BUS3_NUM; i++){
		if(strncmp(buf, bus3_dev[i], 8) == 0){
			ret = device_cal((int)BUS3_IDMSK, MSK_L, cmp_id_l[i]);
			if (ret != 0)
				goto
				   err_dev;	
			printk("bus3:%d:	%s:W: %4lld.%.2lldMB/s   R: %4lld.%.2lldMB/s\n", i, bus3_dev[i], CA_M(calc(ave(b3_wd))), CA_B(calc(ave(b3_wd))), CA_M(calc(ave(b3_rd))), CA_B(calc(ave(b3_rd))));
			goto find_dev;
		}
	}
	for(i=0; i<BUS4_NUM; i++){
		if(strncmp(buf, bus4_dev[i], 8) == 0){
			ret = device_cal((int)BUS4_IDMSK, MSK_L, cmp_id_l[i]);
			if (ret != 0)
				goto
				   err_dev;	
			printk("bus4:%d: 	%s:W: %4lld.%.2lldMB/s   R: %4lld.%.2lldMB/s\n",  i, bus4_dev[i], CA_M(calc(ave(b4_wd))), CA_B(calc(ave(b4_wd))), CA_M(calc(ave(b4_rd))), CA_B(calc(ave(b4_rd))));
			goto find_dev;
		}
	}
	for(i=0; i<BUS6_MAIN_NUM; i++){
		if(strncmp(buf, bus6_dev_main[i], 8) == 0){
			ret = device_cal((int)BUS6_IDMSK, MSK_BUS6_MAIN, cmp_id_bus6_main[i]);
			if (ret != 0)
				goto
				   err_dev;	
			printk("bus6:%d 	%s:W: %4lld.%.2lldMB/s   R: %4lld.%.2lldMB/s\n", i, bus6_dev_main[i], CA_M(calc(ave(b6_wd))), CA_B(calc(ave(b6_wd))), CA_M(calc(ave(b6_rd))), CA_B(calc(ave(b6_rd))));
			goto find_dev;
		}
	}

	for(i=0; i<BUS6_SUB_NUM; i++){
		if(strncmp(buf, bus6_dev_sub[i], 8) == 0){
			ret = device_cal((int)BUS6_IDMSK, MSK_BUS6_SUB, cmp_id_bus6_sub[i]);
			if (ret != 0)
				goto err_dev;
			printk("bus6:%d 	%s:W: %4lld.%.2lldMB/s   R: %4lld.%.2lldMB/s\n", i, bus6_dev_sub[i], CA_M(calc(ave(b6_wd))), CA_B(calc(ave(b6_wd))), CA_M(calc(ave(b6_rd))), CA_B(calc(ave(b6_rd))));
			goto find_dev;
		}
    }
	
err_dev:
	printk("no device %s found in any bus!\n", buf);
	return -EINVAL;

find_dev:
	return 0;
}

static void print_all(void)
{
	int i;
	
	printk("cpu :  W: %4lld.%.2lldMB/s	R: %4lld.%.2lldMB/s\n",CA_M(calc(c0_wd[0])), CA_B(calc(c0_wd[0])), CA_M(calc(c0_rd[0])), CA_B(calc(c0_rd[0])));
	printk("bus1:  W: %4lld.%.2lldMB/s	R: %4lld.%.2lldMB/s\n",CA_M(calc(b1_wd[0])), CA_B(calc(b1_wd[0])), CA_M(calc(b1_rd[0])), CA_B(calc(b1_rd[0])));
	for(i = 1; i < (BUS1_NUM+1); i++)
		printk("	%-8s:W: %4lld.%.2lldMB/s   R: %4lld.%.2lldMB/s\n", bus1_dev[i-1], CA_M(calc(b1_wd[i])), CA_B(calc(b1_wd[i])), CA_M(calc(b1_rd[i])), CA_B(calc(b1_rd[i])));
	
	printk("bus3:  W: %4lld.%.2lldMB/s	R: %4lld.%.2lldMB/s\n",CA_M(calc(b3_wd[0])), CA_B(calc(b3_wd[0])), CA_M(calc(b3_rd[0])), CA_B(calc(b3_rd[0])));
	for(i = 1; i < (BUS3_NUM+1); i++)
		printk("	%-8s:W: %4lld.%.2lldMB/s   R: %4lld.%.2lldMB/s\n", bus3_dev[i-1], CA_M(calc(b3_wd[i])), CA_B(calc(b3_wd[i])), CA_M(calc(b3_rd[i])), CA_B(calc(b3_rd[i])));
	
	printk("bus4:  W: %4lld.%.2lldMB/s	R: %4lld.%.2lldMB/s\n",CA_M(calc(b4_wd[0])), CA_B(calc(b4_wd[0])), CA_M(calc(b4_rd[0])), CA_B(calc(b4_rd[0])));
	for(i = 1; i < (BUS4_NUM+1); i++)
		printk("	%-8s:W: %4lld.%.2lldMB/s   R: %4lld.%.2lldMB/s\n", bus4_dev[i-1], CA_M(calc(b4_wd[i])), CA_B(calc(b4_wd[i])), CA_M(calc(b4_rd[i])), CA_B(calc(b4_rd[i])));
	
	printk("bus6:  W: %4lld.%.2lldMB/s	R: %4lld.%.2lldMB/s\n",CA_M(calc(b6_wd[0])), CA_B(calc(b6_wd[0])), CA_M(calc(b6_rd[0])), CA_B(calc(b6_rd[0])));
	for(i = 1; i < (BUS6_MAIN_NUM+1); i++)
		printk("	%-8s:W: %4lld.%.2lldMB/s   R: %4lld.%.2lldMB/s\n", bus6_dev_main[i-1], CA_M(calc(b6_wd[i])), CA_B(calc(b6_wd[i])), CA_M(calc(b6_rd[i])), CA_B(calc(b6_rd[i])));
	
	
}

static void dpmu_all(void)
{
	int i = 0;
	for (i=0; i<8; i++)
		calc_id(i);

	writel(0, DPMU_EN);
	print_all();
	writel(1, DPMU_EN);
}

static void clear_id(void)
{
	writel(0,		BUS1_IDMSK);
	writel(0,		BUS1_IDCMP);
	writel(0,		BUS3_IDMSK);
	writel(0,		BUS3_IDCMP);
	writel(0,		BUS4_IDMSK);
	writel(0,		BUS4_IDCMP);
	writel(0,		BUS6_IDMSK);
	writel(0,		BUS6_IDCMP);
}

static ssize_t dpmu_dev_write(struct file *filp,
		const char __user *buf, size_t length,
		loff_t *offset)
{
	char str[256];
	int ret;

	ret = copy_from_user(str, buf, min(256, (int)length));

	if(strncmp(str, "dpmu bus", 8) == 0){
		dpmu_bus();
	}else if(strncmp(str, "dpmu all", 8) == 0){
		all_irq_flag = 1;
		dpmu_all();
		all_irq_flag = 0;
		clear_id();
	}else if(strncmp(str, "dpmu device", 11) == 0){
		dev_irq_flag = 1;
		ret = dpmu_device(str);
		dev_irq_flag = 0;
		clear_id();
		if(ret != 0)
			return -EINVAL;
	}else if(strncmp(str, "dpmu dfi", 8) == 0){
		dpmu_dfi();
	}else
		return -EINVAL;

	return length;
}

static const struct file_operations dpmu_dev_fops = {
	.read  = dpmu_dev_read,
	.write = dpmu_dev_write,
	.open  = dpmu_dev_open,
	.unlocked_ioctl = dpmu_dev_ioctl,
};

static irqreturn_t dpmu_irq_handler(int irq, void *dev)
{
#ifdef DPMU_DEBUG
	dpmu_state();
#endif
	/*clear irq*/
	writel(1, DPMU_INT);
	if ((dev_irq_flag == 1) && ((VAL_CNT + 1) == SAMPLE_NUM)){
		dpmu_irq_n->dpmu_condition = 1;
		wake_up(&dpmu_irq_n->irq_wq);
	}
	if ((all_irq_flag == 1) && (VAL_CNT == 1)){
		dpmu_irq_n->dpmu_condition = 1;
		wake_up(&dpmu_irq_n->irq_wq);
	}
	reg_update();
	return IRQ_HANDLED;
}

static int config_dpmu(void)
{
	int ret = 0;
    uint32_t value = 0;    
	set_sample(SAMPLE_PER);

    //enable dpmu clk
    value = readl(IO_ADDRESS(SYSMGR_EMIF_BASE+0x68));
    writel(value&~2, IO_ADDRESS(SYSMGR_EMIF_BASE+0x68));

	writel(1, DPMU_EN);

	dpmu_irq_n = kzalloc(sizeof(dpmu_irq_n), GFP_KERNEL);
	init_waitqueue_head(&dpmu_irq_n->irq_wq);
	
	/*dpmu intr NO is 248*/
	ret = request_irq(DPMU_IRQ, dpmu_irq_handler, IRQF_DISABLED, "dpmu_irq", &dpmu_irq_handler);
	if(ret){
		printk("NO:%d irq request failed!\n", DPMU_IRQ);
		return -1;
	}

	/*dpmu irq enable*/
	writel(1, DPMU_INTE);

	return 0;
}

static int __init dpmu_dev_init(void)
{
	struct class *cls;
	int ret;

	printk(KERN_ALERT "dpmu driver (c) 2015, 2020 InfoTM\n");

	/* create dpmu dev node */
	ret = register_chrdev(DPMU_MAJOR, DPMU_NAME, &dpmu_dev_fops);
	if(ret < 0){
		printk(KERN_ERR "Register char device for dpmu failed.\n");
		return -EFAULT;
	}

	cls = class_create(THIS_MODULE, DPMU_NAME);
	if(!cls){
		printk(KERN_ERR "Can not register class for dpmu .\n");
		return -EFAULT;
	}

	/* create device node */
	device_create(cls, NULL, MKDEV(DPMU_MAJOR, 0), NULL, DPMU_NAME);

	ret = config_dpmu();
	if(ret < 0){
		printk(KERN_ERR "DPMU registe failed.\n");
		return -EFAULT;
	}
	
#if 0
	if(item_exist("dvfs.enable") && item_equal("dvfs.enable", "1", 0))
	  __dvfs = 1;
#endif

	return 0;
}

static void __exit dpmu_dev_exit(void) {}

module_init(dpmu_dev_init);
module_exit(dpmu_dev_exit);
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("willie.wei");
MODULE_DESCRIPTION("dpmu for ddr");

