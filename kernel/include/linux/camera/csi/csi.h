/* Copyright (c) 2014~2112 ShangHai Infotm Ltd all rights reserved. 
** 
** Use of Infotm's code is governed by terms and conditions 
** stated in the accompanying licensing statement. 
** 
** FPGA soc verify mipi csi2 defination header file     
**
** Revision History: 
** ----------------- 
** v0.0.1	sam.zhou@2014/06/23: first version.
**
*****************************************************************************/
#ifndef  __CSI_H_
#define  __CSI_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <linux/types.h>
#include <linux/i2c.h>
struct csi2_host {
	void __iomem *base;
	void *dma_buf_saddr;
	uint8_t log_level;
	uint8_t lanes;	 
	uint32_t freq;
};

struct sys_manager {
	void __iomem *base;
};

void csi2_dump_regs(void);
void calibration(void);
uint8_t csi2_phy_testcode(uint16_t code, uint8_t data);
uint32_t csi2_get_reg(uint32_t addr);
int csi2_set_reg(uint32_t addr, uint32_t value);
int csi2_probe(void);
//int csi_module_sys_reset(struct sys_manager *psys);
//int csi_module_power_on(struct sys_manager *psys);
//int csi_module_power_off(struct sys_manager *psys);
uint32_t csi2_get_version(void);
int csi2_reset_ctrl(void);
int csi2_phy_set_lanes(uint8_t lanes);
uint8_t csi2_phy_get_lanes(void);
// 1: open phy 0: shutdown
int csi2_phy_poweron(uint8_t flag);
int csi2_phy_reset(void);
uint32_t csi2_phy_get_state(void);
int csi2_set_bypass_decc(uint8_t flag);
int csi2_set_data_ids(uint8_t id, uint8_t dt, uint8_t vc);
int csi2_mask_err1(uint32_t mask);
int csi2_umask_err1(uint32_t umask);
int csi2_mask_err2(uint32_t mask);
int csi2_umask_err2(uint32_t umask);
int csi2_phy_test_clear(void);
int csi2_set_iditoipi(int width, int height, int type);
extern struct csi2_host g_host;
extern struct sys_manager g_mipi_sys;
#ifdef __cplusplus
}
#endif

#endif



