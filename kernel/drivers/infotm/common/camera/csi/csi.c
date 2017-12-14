/* **************************************************************************** 
 * ** 
 * ** Copyright (c) 2014~2112 ShangHai Infotm Ltd all rights reserved. 
 * ** 
 * ** Use of Infotm's code is governed by terms and conditions 
 * ** stated in the accompanying licensing statement. 
 * ** 
 * ** Soc FPGA verify: csi.c 
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

#include <linux/camera/csi/csi_reg.h>
#include <linux/camera/csi/csi.h>
#include <../arch/arm/mach-apollo/include/mach/power-gate.h>

#define POWER_TIME_OUT (0xff)
#define MOUDLE_RESET   (0xffffffff)
#define IDS_IDENTY        (0x04)
#define CSI_DEBUG(fmt, args...)   \
        do{                              \
               printk(fmt, ##args);     \
        }while(0)

struct csi2_host g_host;
struct sys_manager g_mipi_sys;


//should probe the csi2 device
int csi2_probe(void)
{
	g_host.base = ioremap_nocache(CSI_REG_BASE, IMAP_MIPI_CSI_SIZE);/*devm_ioremap_resource(dev, mem_res);*/
	g_mipi_sys.base = ioremap_nocache(SYSMGR_MIPI_BASE, 0xc000 - 0x4800);//*/(void *)IO_ADDRESS(SYSMGR_MIPI_BASE);
	g_host.lanes = 0;
	g_host.log_level = 0;	
	//printk("-->csi2_probe: host_base is %x, mipi base is %x\n", g_host.base, g_mipi_sys.base);
	g_host.dma_buf_saddr = NULL ;
	return 0;

}

void csi2_set_bits(struct csi2_host *host, uint32_t addr,
						int width, int offset, uint32_t value)
{
	uint32_t tmp;
	//printk("--->csi2_set_bits addr is %x\n",  ((unsigned int)((host)->base) + (addr)));
	tmp = csi_read(host, addr);
	tmp = ((value) << (offset)) | (~(((1 << (width))- 1) << (offset))  & tmp);
	//printk("--->csi2_set_bits write %x\n", tmp);
	csi_write(host, addr, tmp);

	tmp = csi_read(host, addr);
	//printk("--->check value is %x\n", tmp);
	
	//printk("--->csi2_set_bits write done\n");
}

uint32_t csi2_get_bits(struct csi2_host *host, uint32_t addr,
				int width, int offset)
{
	uint32_t tmp;
	tmp = csi_read(host, addr);
	tmp = (tmp >> (offset)) & ((1 << (width))- 1) ;
	//printk("--->csi2_get_bits is %x\n", tmp);
	return  tmp;
}

uint32_t csi2_get_version(void)
{
	return (csi_read(&g_host, CSI2_VERSION_REG));	
}

int csi2_reset_ctrl(void)
{
	csi2_set_bits(&g_host, CSI2_RESET_REG, 1, CSI2_RESET_N, CSI_DISABLE);
	udelay(10);
	csi2_set_bits(&g_host, CSI2_RESET_REG, 1, CSI2_RESET_N, CSI_ENABLE);
	return 0;
}

int csi2_set_reg(uint32_t addr, uint32_t value)
{
	csi_write(&g_host, addr, value);
	return 0;
}

uint32_t csi2_get_reg(uint32_t addr)
{
	return (csi_read(&g_host, addr));
}

int csi2_phy_test_clear(void)
{
	csi_write(&g_host, CSI2_DPHY_TST_CRTL0_REG, CSI_ENABLE);
	udelay(5);
	csi_write(&g_host, CSI2_DPHY_TST_CRTL0_REG, CSI_DISABLE);
	return 0;
}

uint8_t csi2_phy_testcode(uint16_t code, uint8_t data)
{
	uint32_t tmp;
	//set test clock high
	csi_write(&g_host, CSI2_DPHY_TST_CRTL0_REG, 0x02);
	//set testcode
	csi_write(&g_host, CSI2_DPHY_TST_CRTL1_REG, code);
	//set testen high
	tmp = csi_read(&g_host, CSI2_DPHY_TST_CRTL1_REG);
	csi_write(&g_host, CSI2_DPHY_TST_CRTL1_REG, tmp | (1 << 16));
	//set test clock low
	csi_write(&g_host, CSI2_DPHY_TST_CRTL0_REG, 0x00);
	//set testen low
	tmp = csi_read(&g_host, CSI2_DPHY_TST_CRTL1_REG);
	csi_write(&g_host, CSI2_DPHY_TST_CRTL1_REG, tmp & (~(1 << 16)));
	//set data
	csi_write(&g_host, CSI2_DPHY_TST_CRTL1_REG, data);
	csi_write(&g_host, CSI2_DPHY_TST_CRTL0_REG, 0x02);
	tmp = csi_read(&g_host, CSI2_DPHY_TST_CRTL1_REG);
	return ((tmp >> 8) & 0xff);

}

void csi2_dump_regs(void)
{
	printk("CSI_REG_OFFSET%x =[0x%x]\n", CSI2_VERSION_REG, csi_read(&g_host, CSI2_VERSION_REG));
	printk("CSI_REG_OFFSET%x =[0x%x]\n", CSI2_N_LANES_REG, csi_read(&g_host, CSI2_N_LANES_REG));
	printk("CSI_REG_OFFSET%x =[0x%x]\n", CSI2_DPHY_SHUTDOWN_REG, csi_read(&g_host, CSI2_DPHY_SHUTDOWN_REG));
	printk("CSI_REG_OFFSET%x =[0x%x]\n", CSI2_DPHYRST_REG, csi_read(&g_host, CSI2_DPHYRST_REG));
	printk("CSI_REG_OFFSET%x =[0x%x]\n", CSI2_RESET_REG, csi_read(&g_host, CSI2_RESET_REG));
	printk("CSI_REG_OFFSET%x =[0x%x]\n", CSI2_DPHY_STATE_REG, csi_read(&g_host, CSI2_DPHY_STATE_REG));
	printk("CSI_REG_OFFSET%x =[0x%x]\n", CSI2_DATA_IDS_1_REG, csi_read(&g_host, CSI2_DATA_IDS_1_REG));
	printk("CSI_REG_OFFSET%x =[0x%x]\n", CSI2_DATA_IDS_2_REG, csi_read(&g_host, CSI2_DATA_IDS_2_REG));
	printk("CSI_REG_OFFSET%x =[0x%x]\n", CSI2_ERR1_REG, csi_read(&g_host, CSI2_ERR1_REG));
	printk("CSI_REG_OFFSET%x =[0x%x]\n", CSI2_ERR2_REG, csi_read(&g_host, CSI2_ERR2_REG));
	printk("CSI_REG_OFFSET%x =[0x%x]\n", CSI2_MASK1_REG, csi_read(&g_host, CSI2_MASK1_REG));
	printk("CSI_REG_OFFSET%x =[0x%x]\n", CSI2_MASK2_REG, csi_read(&g_host, CSI2_MASK2_REG));
	printk("CSI_REG_OFFSET%x =[0x%x]\n", CSI2_DPHY_TST_CRTL0_REG, csi_read(&g_host, CSI2_DPHY_TST_CRTL0_REG));
	printk("CSI_REG_OFFSET%x =[0x%x]\n", CSI2_DPHY_TST_CRTL1_REG, csi_read(&g_host, CSI2_DPHY_TST_CRTL1_REG));
	printk("CSI_REG_OFFSET%x =[0x%x]\n", CSIDMAREG_DATATYPE, csi_read(&g_host, CSIDMAREG_DATATYPE));
	printk("CSI_REG_OFFSET%x =[0x%x]\n", CSIDMAREG_FRAMESIZE, csi_read(&g_host, CSIDMAREG_FRAMESIZE));
	printk("CSI_REG_OFFSET%x =[0x%x]\n", CSIDMAREG_VSYNCCNT, csi_read(&g_host, CSIDMAREG_VSYNCCNT));
	printk("CSI_REG_OFFSET%x =[0x%x]\n", CSIDMAREG_HBLANK, csi_read(&g_host, CSIDMAREG_HBLANK));
}
EXPORT_SYMBOL(csi2_dump_regs);

void calibration(void)
{
	uint32_t tmp;
	uint32_t val, cmpout, aux_tripu, aux_tripd, aux, aux_a, aux_b;
	aux_a = 0;
	aux_b = 0x07;
	//set test clock high
	csi_write(&g_host, CSI2_DPHY_TST_CRTL0_REG, 0x02);
	//set testcode
	csi_write(&g_host, CSI2_DPHY_TST_CRTL1_REG, 0x21);
	//set testen high
	tmp = csi_read(&g_host, CSI2_DPHY_TST_CRTL1_REG);
	csi_write(&g_host, CSI2_DPHY_TST_CRTL1_REG, tmp | (1 << 16));
	//set test clock low
	csi_write(&g_host, CSI2_DPHY_TST_CRTL0_REG, 0x00);
	//set testen low
	tmp = csi_read(&g_host, CSI2_DPHY_TST_CRTL1_REG);
	csi_write(&g_host, CSI2_DPHY_TST_CRTL1_REG, tmp & (~(1 << 16)));

	val = 0x00;
	csi_write(&g_host, CSI2_DPHY_TST_CRTL1_REG, (val << 2) | 0x03);
	csi_write(&g_host, CSI2_DPHY_TST_CRTL0_REG, 0x02);
	cmpout = csi_read(&g_host, CSI2_DPHY_TST_CRTL1_REG);
	//pr_info("testdout=0x%x, val=0x%x, testdin=0x%x\n", cmpout, val, (val << 2) | 0x03);
	cmpout = cmpout >> 8;
	cmpout >>= 7;
	aux_tripu = cmpout;
	//pr_info("cmpout=0x%x\n", cmpout);

	for(val = 0x01; val <= 0x07; val++) {
		udelay(10);
		csi_write(&g_host, CSI2_DPHY_TST_CRTL0_REG, 0x00);
		csi_write(&g_host, CSI2_DPHY_TST_CRTL1_REG, (val << 2) | 0x03);
		csi_write(&g_host, CSI2_DPHY_TST_CRTL0_REG, 0x02);
		cmpout = csi_read(&g_host, CSI2_DPHY_TST_CRTL1_REG);
		//pr_info("testdout1=0x%x, val=0x%x, testdin1=0x%x\n", cmpout, val, (val << 2) | 0x03);
		cmpout = cmpout >> 8;
		cmpout >>= 7;
		//pr_info("cmpout1=0x%x\n", cmpout);
		if(cmpout != aux_tripu){
			aux_a = val;
			aux_tripu = cmpout;
			break;
		}
	}
	
	if(0x00 == aux_a) {
		if(cmpout == 0) {
			aux = 0x00;
		}
		else {
			aux = 0x07;
		}

	}
	else {
		val = 0x07;
		udelay(10);
		//test clear
		csi_write(&g_host, CSI2_DPHY_TST_CRTL0_REG, 0x00);
		csi_write(&g_host, CSI2_DPHY_TST_CRTL1_REG, (val << 2) | 0x03);
		csi_write(&g_host, CSI2_DPHY_TST_CRTL0_REG, 0x02);
		cmpout = csi_read(&g_host, CSI2_DPHY_TST_CRTL1_REG);
		//pr_info("testdout2=0x%x, val=0x%x, testdin2=0x%x\n", cmpout, val, (val << 2) | 0x03);
		cmpout = cmpout >> 8;
		cmpout >>= 0x07;
		aux_tripd = cmpout;
		//pr_info("cmpout2=0x%x\n", cmpout);

		if(aux_tripd != aux_tripu) {
			aux = 0x03;
			pr_err("calibration failed\n");
		}
		else {
			for(val = 0x06; val >= 0x00; val--) {
				udelay(10);
				csi_write(&g_host, CSI2_DPHY_TST_CRTL0_REG, 0x00);
				csi_write(&g_host, CSI2_DPHY_TST_CRTL1_REG, (val << 2) | 0x03);
				csi_write(&g_host, CSI2_DPHY_TST_CRTL0_REG, 0x02);
				cmpout = csi_read(&g_host, CSI2_DPHY_TST_CRTL1_REG);
				//pr_info("testdout3=0x%x, val=0x%x, testdin3=0x%x\n", cmpout, val, (val << 2) | 0x03);
				cmpout = cmpout >> 8;
				cmpout >>= 0x07;
				//pr_info("cmpout3=0x%x\n", cmpout);
				if(cmpout != aux_tripd) {
					aux_b = val;
					aux_tripd = cmpout;
					break;
				}
			}
			aux = (aux_a + aux_b + 1) >> 1; //round_max
		}

	}

	//pr_info("last aux=0x%x\n", aux);
	udelay(10);
	csi_write(&g_host, CSI2_DPHY_TST_CRTL0_REG, 0x00);
	csi_write(&g_host, CSI2_DPHY_TST_CRTL1_REG, (aux << 2) | 0x03);
	csi_write(&g_host, CSI2_DPHY_TST_CRTL0_REG, 0x02);
	cmpout = csi_read(&g_host, CSI2_DPHY_TST_CRTL1_REG);
	//pr_info("last cmpout=0x%x\n", cmpout);
}

int csi2_set_iditoipi(int width, int height, int type)
{
    csi_write(&g_host, CSIDMAREG_DATATYPE, type);
    csi_write(&g_host, CSIDMAREG_FRAMESIZE, (height << 16) | (width & 0xffff));
    //this is for xinwu xc9080 for setting hblank, default value is 0x64(d100)
    csi_write(&g_host, CSIDMAREG_HBLANK, 0x3ff);
    return 0;
}

int csi2_phy_set_lanes(uint8_t lanes)
{
	csi2_set_bits(&g_host, CSI2_N_LANES_REG, 2, CSI2_N_LANES, lanes-1);
	return 0;
}

uint8_t csi2_phy_get_lanes(void)
{
	return (csi2_get_bits(&g_host, CSI2_N_LANES_REG, 2, CSI2_N_LANES) + 1);
}

// 1: open phy 0: shutdown
int csi2_phy_poweron(uint8_t flag)
{
	csi2_set_bits(&g_host, CSI2_DPHY_SHUTDOWN_REG, 1, CSI2_PHY_SHUTDOWN, flag);
	return 0;
}
EXPORT_SYMBOL(csi2_phy_poweron);
//get phy state
uint32_t csi2_phy_get_state(void)
{
	return (csi2_get_bits(&g_host, CSI2_DPHY_STATE_REG, 12, CSI2_PHY_RXULPSESC_0));
}

int csi2_phy_reset(void)
{
	csi2_set_bits(&g_host, CSI2_DPHYRST_REG, 1, CSI2_PHY_RSTZ, CSI_DISABLE);
	udelay(5);
	csi2_set_bits(&g_host, CSI2_DPHYRST_REG, 1, CSI2_PHY_RSTZ, CSI_ENABLE);
	return 0;
}

int csi2_set_bypass_decc(uint8_t flag)
{
	csi2_set_bits(&g_host, CSI2_DPHY_STATE_REG, 1, CSI2_PHY_BYPASS_2ECC_TST, flag);
	return 0;
}

//id 0~7
int csi2_set_data_ids(uint8_t id, uint8_t dt, uint8_t vc)
{

	if(IDS_IDENTY > id ) {
		uint8_t offset = 0;
		offset = id*8;
		csi2_set_bits(&g_host, CSI2_DATA_IDS_1_REG, 6, offset, dt);
		offset += 6;
		csi2_set_bits(&g_host, CSI2_DATA_IDS_1_REG, 2, offset, vc); 
	}
	else {
		uint8_t offset = 0;
		offset = (id-4)*8;
		csi2_set_bits(&g_host, CSI2_DATA_IDS_2_REG, 6, offset, dt);
		offset += 6;
		csi2_set_bits(&g_host, CSI2_DATA_IDS_1_REG, 2, offset, vc); 
	}
	return 0;
}

int csi2_mask_err1(uint32_t mask)
{
	uint32_t tmp = 0;
	tmp = csi_read(&g_host, CSI2_MASK1_REG);
	tmp |= mask;
	csi_write(&g_host, CSI2_MASK1_REG, tmp);

	return 0;
}

int csi2_umask_err1(uint32_t umask)
{
	uint32_t tmp = 0;
	tmp = csi_read(&g_host, CSI2_MASK1_REG);
	tmp &= ~umask;
	csi_write(&g_host, CSI2_MASK1_REG, tmp);
	return 0;
}

int csi2_mask_err2(uint32_t mask)
{
	uint32_t tmp = 0;
	tmp = csi_read(&g_host, CSI2_MASK2_REG);
	tmp |= mask;
	csi_write(&g_host, CSI2_MASK2_REG, tmp);

	return 0;
}

int csi2_umask_err2(uint32_t umask)
{
	uint32_t tmp = 0;
	tmp = csi_read(&g_host, CSI2_MASK2_REG);
	tmp &= ~umask;
	csi_write(&g_host, CSI2_MASK2_REG, tmp);
	return 0;
}

