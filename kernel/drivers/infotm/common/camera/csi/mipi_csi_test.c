/* **************************************************************************** 
 * ** 
 * ** Copyright (c) 2014~2112 ShangHai Infotm Ltd all rights reserved. 
 * ** 
 * ** Use of Infotm's code is governed by terms and conditions 
 * ** stated in the accompanying licensing statement. 
 * ** 
 * ** Soc FPGA verify: csi_main.c :offer  csi module test items operation
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
#include <media/media-device.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/errno.h>
#include <linux/device.h>
#include <linux/i2c.h>
#include <linux/interrupt.h>
#include <mach/pad.h>
#include <mach/power-gate.h>
#include <InfotmMedia.h>
#include <utlz_lib.h>
#include <linux/camera/csi/csi_core.h>
#include <linux/camera/csi/csi_dma.h>
#include <linux/camera/csi/csi_reg.h>
#include "../camif/camif_core.h"
#include <mach/pad.h>
#include <linux/gpio.h>

#ifdef DMA_DUMP_DATA_TEST
extern csi_dma_config_t gdcfg; 
extern volatile int ret_flag;
// capture image from sensor
static int run_csi_rtl_case(struct i2c_client *client, bool is_sccb, uint16_t enable_reg, uint16_t enable_mask)
{
	//initialize csi dma 
	csi_dma_init(&gdcfg);
	// ar0330 enable
#ifdef AUTO_INIT_MIPI_SENSOR
	sensor_enable(client, 1, is_sccb, enable_reg, enable_mask);
#endif
	//start dma to transfer data
	csi_dma_start();
#if 1
	while(1) {
		msleep(10);
		if(1 == ret_flag){
			printk("dump picture data from memory to rtl_data.bin\n");
			return 1;
			
		}	
	}
#endif
	return 0;
}
#endif

int mipi_csi_probe(struct i2c_client *client, bool is_sccb, uint16_t enable_reg, uint16_t enable_mask, int lanes, int csi_freg)
{
      struct clk *pbusclk = NULL;
      struct clk *poclk = NULL;
	  
	int ret = 0;

	printk("***************************\n");

	printk("-->start imapx_mipi_csi_probe\n");
	csi_core_init(NULL, NULL, lanes, csi_freg);

	csi_core_open(NULL, NULL);

#ifdef AUTO_INIT_MIPI_SENSOR
	sensor_mipi_init_func(client, is_sccb);
#endif

#ifdef DMA_DUMP_DATA_TEST
	printk("-->test case :csi raw12.\n");
	csi_dma_config(&gdcfg, (CSI_SENSOR_FRAME_SIZE*5)/4);
	//ar0330_id_check(client);
	run_csi_rtl_case(client, is_sccb, enable_reg, enable_mask); //data type :raw12	
#else
	disable_irq(CSI_IRQ0);
#endif

	printk("-->Get the CSI controller version :0x%x\n", csi_core_get_version());	
	printk("-->Current CSI lanes status :0x%x\n", csi_core_get_state());
	printk("-->imapx_mipi_csi_probe done!\n");
}

EXPORT_SYMBOL(mipi_csi_probe);

#if 0 // not used now
static int imapx_mipi_csi_probe(struct platform_device *pdev)
{
	return 0;
}


static int imapx_mipi_csi_remove(struct platform_device *pdev)
{
	return 0;
	//
}

static int imapx_mipi_csi_plat_suspend(struct device *dev)
{
	//TODO
	return 0;
}

static int imapx_mipi_csi_plat_resume(struct device *dev)
{
	//TODO
	return 0;
}

//define mipi csi power manager operation
SIMPLE_DEV_PM_OPS(imapx_mipi_csi_pm_ops, imapx_mipi_csi_plat_suspend, 
		imapx_mipi_csi_plat_resume);

//define platform driver structure
static struct platform_driver imapx_mipi_csi_driver = {
		.probe	= imapx_mipi_csi_probe,
		.remove = imapx_mipi_csi_remove,
		.driver = {                    //device driver structure
			.name  = "imapx-mipi-csi-ctrl",
			.owner = THIS_MODULE,
			.pm    = &imapx_mipi_csi_pm_ops,
		},
};

static int __init imapx_mipi_csi_init(void)
{
	return 0;//platform_driver_register(&imapx_mipi_csi_driver);
}

static void __exit imapx_mipi_csi_exit(void)
{
	//platform_driver_unregister(&imapx_mipi_csi_driver);
}

module_init(imapx_mipi_csi_init);
module_exit(imapx_mipi_csi_exit);

MODULE_AUTHOR("linyun.xiong");
MODULE_DESCRIPTION("imapx mipi csi driver");
MODULE_LICENSE("GPL");
#endif

