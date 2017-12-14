/* **************************************************************************** 
 * ** 
 * ** Copyright (c) 2014~2112 ShangHai Infotm Ltd all rights reserved. 
 * ** 
 * ** Use of Infotm's code is governed by terms and conditions 
 * ** stated in the accompanying licensing statement. 
 * ** 
 * ** Soc FPGA verify: csi_core.c :offer core csi operation
 * **
 * ** Revision History: 
 * ** ----------------- 
 * ** v0.0.1	sam.zhou@2014/06/23: first version.
 * **
 * *****************************************************************************/

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kthread.h>
//#include <linux/lowlevel_api.h>
#include <linux/moduleparam.h>
#include <linux/ctype.h>
#include <linux/crypto.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/dma-mapping.h>
#include <linux/cdev.h>
#include <linux/errno.h>
#include <linux/interrupt.h>
#include <linux/jiffies.h>
#include <linux/list.h>
#include <linux/pci.h>
#include <linux/random.h>
#include <linux/scatterlist.h>
#include <linux/slab.h>
#include <linux/stat.h>
#include <linux/string.h>
#include <linux/timer.h>
#include <linux/file.h>
//#include <linux/fcntl.h>
#include <linux/camera/csi/csi_core.h>
#include <linux/camera/csi/csi.h>
#include <linux/camera/csi/csi_reg.h>
#include <linux/camera/csi/csi_dma.h>
//#include <irq.h>
//#include <malloc.h>

#include <mach/imap-iomap.h>
#include <mach/power-gate.h>
//#include <linux/clk.h>
#include <linux/clk-private.h>

//#define CSI_FREQ	 (200)
//#define CSI_FREQ	 (410)

#define CSI_TST_CODE (0x44)
//#define dump_dat ("csi/csi_dump_data.dat")
//
//volatile int ret_flag;
////static int csi_fd = 0;
extern csi_dma_config_t gdcfg;

uint32_t csi_core_get_version(void)
{
	return csi2_get_version();	
}

uint32_t csi_core_get_state(void)
{
	return csi2_phy_get_state();
}

error_t csi_core_set_reg(uint32_t addr, uint32_t value)
{
	return csi2_set_reg(addr, value);
}

uint32_t csi_core_read_reg(uint32_t addr)
{
	return csi2_get_reg(addr);
}

error_t csi_core_set_freq(uint32_t freq)
{
	printk("--->csi_core_set_freq %d\n", freq);
	if((freq >= 80) && (freq <= 90)) {
        csi2_phy_testcode(CSI_TST_CODE, 0x00);   //Range: 80-90MHz, default bit rate(90Mbps)
	}
	else if((freq > 90) && (freq <= 100)) {
		csi2_phy_testcode(CSI_TST_CODE, 0x20);   //Range: 90-100MHz
	}
	else if((freq > 100) && (freq <= 110)) {
		csi2_phy_testcode(CSI_TST_CODE, 0x40);   //Range: 100-110MHz
	}
    else if((freq > 110) && (freq <= 125)) {
        csi2_phy_testcode(CSI_TST_CODE, 0x02);   //Range: 110-125MHz
    }
    else if((freq > 125) && (freq <= 140)) {
		csi2_phy_testcode(CSI_TST_CODE, 0x22);   //Range: 125-140MHz
	}
	else if((freq > 140) && (freq <= 150)) {
		csi2_phy_testcode(CSI_TST_CODE, 0x42);   //Range: 140-150MHz
	}
	else if((freq > 150) && (freq <= 160)) {
		csi2_phy_testcode(CSI_TST_CODE, 0x04);   //Range: 150-160MHz
	}
	else if((freq > 160) && (freq <= 180)) {
		csi2_phy_testcode(CSI_TST_CODE, 0x24);   //Range: 160-180MHz
	}
	else if((freq > 180) && (freq <= 200)) {
		csi2_phy_testcode(CSI_TST_CODE, 0x44);   //Range: 180-200MHz
	}
	else if((freq > 200) && (freq <= 210)) {
		csi2_phy_testcode(CSI_TST_CODE, 0x06);   //Range: 200-210MHz
	}
    else if((freq > 210) && (freq <= 240)) {
		csi2_phy_testcode(CSI_TST_CODE, 0x26);   //Range: 210-240MHz
	}
	else if((freq > 240) && (freq <= 250)) {
		csi2_phy_testcode(CSI_TST_CODE, 0x46);   //Range: 240-250MHz
	}
	else if((freq > 250) && (freq <= 270)) {
		csi2_phy_testcode(CSI_TST_CODE, 0x08/* 0x48*/);   //Range: 250-270MHz
	}
	else if((freq > 270) && (freq <= 300)) {
		csi2_phy_testcode(CSI_TST_CODE, 0x28);   //Range: 270-300MHz
	}
	else if((freq > 300) && (freq <= 330)) {
		csi2_phy_testcode(CSI_TST_CODE, 0x08);   //Range: 300-330MHz
	}
	else if((freq > 330) && (freq <= 360)) {
		csi2_phy_testcode(CSI_TST_CODE, 0x2a);   //Range: 330-360MHz
	}
	else if((freq > 360) && (freq <= 400)) {
		csi2_phy_testcode(CSI_TST_CODE, 0x4a);   //Range: 360-400MHz
	}
	else if((freq > 400) && (freq <= 450)) {
		csi2_phy_testcode(CSI_TST_CODE, 0x0c);   //Range: 400-450MHz
	}
	else if((freq > 450) && (freq <= 500)) {
		csi2_phy_testcode(CSI_TST_CODE, 0x2c);   //Range: 450-500MHz
	}
	else if((freq > 500) && (freq <= 550)) {
		csi2_phy_testcode(CSI_TST_CODE, 0x0e);   //Range: 500-550MHz
	}
	else if((freq > 550) && (freq <= 600)) {
		csi2_phy_testcode(CSI_TST_CODE, 0x2e);   //Range: 550-600MHz
	}
	else if((freq > 600) && (freq <= 650)) {
		csi2_phy_testcode(CSI_TST_CODE, 0x10);   //Range: 600-650MHz
	}
	else if((freq > 650) && (freq <= 700)) {
		csi2_phy_testcode(CSI_TST_CODE, 0x30);   //Range: 650-700MHz
	}
	else if((freq > 700) && (freq <= 750))
	{
		csi2_phy_testcode(CSI_TST_CODE, 0x12);   //Range: 700-750MHz
	}
	else if((freq > 750) && (freq <= 800)) {
		csi2_phy_testcode(CSI_TST_CODE, 0x32);   //Range: 750-800MHz
	}
	else if((freq > 800) && (freq <= 850))
	{
		csi2_phy_testcode(CSI_TST_CODE, 0x14);   //Range: 800-850MHz
	}
	else if((freq > 850) && (freq <= 900)) {
		csi2_phy_testcode(CSI_TST_CODE, 0x34);   //Range: 850-900MHz
	}
	else if((freq > 900) && (freq <= 950)) {
		csi2_phy_testcode(CSI_TST_CODE, 0x54);   //Range: 900-950MHz
	}
	else if((freq > 950) && (freq <= 1000)) {
		csi2_phy_testcode(CSI_TST_CODE, 0x74);   //Range: 950-1000MHz
	}
	else {
		CSI_ERR("this frequency not support!\n");
		return EVALUE;
	}	
	return SUCCESS;
}
//initialize csi 
error_t csi_core_init(uint8_t lanes, uint32_t csi_freq)
{
	//printk("--->csi_core_init\n");
	//prob device
    char clk_name[4][20] = {"imap-mipi-csi","imap-mipi-dphy-con",
                   "imap-mipi-dphy-ref","imap-mipi-dphy-pix"};
    char clk_con_id[4][20] = {"imap-mipi-csi","imap-mipi-dsi","imap-mipi-dsi",
               "imap-mipi-dsi"};
    struct clk * clk[4];
    int err, i, j;
	csi2_probe();
	g_host.log_level = 1;
	g_host.lanes = lanes;
	g_host.freq  = csi_freq;
	//initialize log level
	//set_log_verbase(g_host.log_level);
	//no need to do this in kernel
	for(i = 0; i < 4; i++){
		clk[i] = clk_get_sys(clk_name[i], clk_con_id[i]);
		if (IS_ERR(clk[i])) {
			pr_err(" %s clock not found! \n",clk_name[i]);
			continue;
		}
		if(!clk[i]->enable_count){
			err = clk_prepare_enable(clk[i]);
			if (err) {
				pr_err("%s clock prepare enable fail!\n",clk_name[i]); 
				goto unprepare_clk;
			}
		}
	}

#if 0
	//register intrrupte handle
	request_irq(CSI_IRQ1, csi_irq1_handler, 0, "csi_irq1", 0/*dev->id*/);
	request_irq(CSI_IRQ2, csi_irq2_handler, 0, "csi_irq2", 0/*dev->id*/);
	request_irq(CSI_IRQ0, csi_irq0_handler, 0, "csi_irq0", 0/*dev->id*/);
#endif
	//mipi module power on
	//csi_module_enable(dev, mem_res);
	module_power_on(SYSMGR_MIPI_BASE);
	//mipi module reset
	//csi_module_reset(dev, mem_res);
	module_reset(SYSMGR_MIPI_BASE, 1);
	//printk("--->csi_core_init done\n");
	//disable all csi irq
	disable_irq(CSI_IRQ1);
	disable_irq(CSI_IRQ2);
	disable_irq(CSI_IRQ0);

	return SUCCESS;
unprepare_clk:
	for(j = 0; j < i; j ++){
		clk_disable_unprepare(clk[j]);
		clk_put(clk[j]);
	}
	return -1;
}
EXPORT_SYMBOL(csi_core_init);

error_t csi_core_open(void)
{
	error_t ret = SUCCESS;

	//test clear
	csi2_phy_test_clear();
	//set phy lane number
	printk("csi phy set lane : %d\n", g_host.lanes);
	ret = csi2_phy_set_lanes(g_host.lanes);
	//printk("-->csi2_phy_set_lanes done\n");
	if(SUCCESS != ret) {
	
		CSI_ERR("csi set phy lanes failed\n");
		goto lane_err;
	}

	csi2_phy_poweron(CSI_ENABLE);
	//reset phy
	ret = csi2_phy_reset();
	if(SUCCESS != ret) {
		CSI_ERR("csi reset phy failed\n");
		goto reset_err;
	}

	//reset controller
	ret = csi2_reset_ctrl();
	if(SUCCESS != ret) {
		CSI_ERR("csi reset controller failed\n");
		goto reset_err;
	}
	
	//set freq
	ret = csi_core_set_freq(g_host.freq);
	if(SUCCESS != ret) {
		CSI_ERR("csi set phy freq failed\n");
		goto lane_err;

	}
	//calibration
	calibration();

	//later should mask error if ok
	//csi2_mask_err1(0x0);
	//csi2_mask_err2(0x0);
	csi2_mask_err1(0xffffffff);
	csi2_mask_err2(0xffffffff);
	pr_err("--->csi open success!\n");
	
	return SUCCESS;
reset_err:
	csi2_phy_poweron(CSI_DISABLE);
lane_err:
	module_power_down(SYSMGR_MIPI_BASE);
	return ret;
}

error_t csi_iditoipi_open(int width, int height, int type )
{
    error_t ret = SUCCESS;

    //test clear
    csi2_phy_test_clear();
    //set phy lane number
    printk("csi phy set lane : %d\n", g_host.lanes);
    ret = csi2_phy_set_lanes(g_host.lanes);
    //printk("-->csi2_phy_set_lanes done\n");
    if(SUCCESS != ret) {

        CSI_ERR("csi set phy lanes failed\n");
        goto lane_err;
    }

    //set iditoipi path configure
    csi2_set_iditoipi(width, height, type);
    csi2_phy_poweron(CSI_ENABLE);
    //reset phy
    ret = csi2_phy_reset();
    if(SUCCESS != ret) {
        CSI_ERR("csi reset phy failed\n");
        goto reset_err;
    }

    //reset controller
    ret = csi2_reset_ctrl();
    if(SUCCESS != ret) {
        CSI_ERR("csi reset controller failed\n");
        goto reset_err;
    }

    //set freq
    ret = csi_core_set_freq(g_host.freq);
    if(SUCCESS != ret) {
        CSI_ERR("csi set phy freq failed\n");
        goto lane_err;

    }
    //calibration
    calibration();

    //later should mask error if ok
    //csi2_mask_err1(0x0);
    //csi2_mask_err2(0x0);
    csi_write(&g_host, CSI2_DATA_IDS_1_REG, 0x2b2b2b2b);
    csi_write(&g_host, CSI2_DATA_IDS_2_REG, 0x2b2b2b2b);
    csi2_mask_err1(0xffffffff);
    csi2_mask_err2(0xffffffff);
    pr_err("--->csi open success!\n");

    return SUCCESS;
reset_err:
    csi2_phy_poweron(CSI_DISABLE);
lane_err:
    module_power_down(SYSMGR_MIPI_BASE);
    return ret;
}

EXPORT_SYMBOL(csi_core_open);
//1:on 0:off
error_t csi_core_poweron(uint8_t flag)
{
	csi2_phy_poweron(flag);
	return SUCCESS;
}

error_t csi_core_close(void)
{
	csi2_phy_poweron(CSI_DISABLE);
	//csi_module_power_off(&g_mipi_sys);
	//csi_module_disable();
	module_power_down(SYSMGR_MIPI_BASE);
	disable_irq(CSI_IRQ1);
	disable_irq(CSI_IRQ2);
	disable_irq(CSI_IRQ0);
	return SUCCESS;
}
EXPORT_SYMBOL(csi_core_close);
error_t csi_core_set_bypass_decc(uint8_t flag)
{
	return csi2_set_bypass_decc(flag);
}

error_t csi_core_mask_err1(uint32_t mask)
{
	return csi2_mask_err1(mask);
}

error_t csi_core_umask_err1(uint32_t umask)
{
	return csi2_umask_err1(umask);
}

error_t csi_core_mask_err2(uint32_t mask)
{
	return csi2_mask_err2(mask);
}

error_t csi_core_umask_err2(uint32_t umask)
{
	return csi2_umask_err2(umask);
}

//id 0~7
error_t csi_core_set_data_ids(uint8_t id, uint8_t dt, uint8_t vc)
{
	return csi2_set_data_ids(id, dt, vc);
}

