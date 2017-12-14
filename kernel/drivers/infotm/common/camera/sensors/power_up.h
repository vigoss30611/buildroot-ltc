/*
 * power_up.h
 *
 *  Created on: Apr 5, 2017
 *      Author: zhouc
 */

#ifndef POWER_UP_H_
#define POWER_UP_H_

#include <linux/i2c.h>
#include <linux/clk.h>
#include <linux/clk-provider.h>

#define MAX_SENSOR_NUM (2)
#define MAX_SEQ_NUM (16)
#define MAX_STR_LEN (64) //must be 64
#define CLK_SRC_PCM ("pcm0-camif")
#define CLK_SRC_SD  ("sd-mmc0")

extern uint en_dbg;

#define pr_db(fmt, arg...) \
	do { \
		if (en_dbg) printk("%s::"fmt, __func__, ##arg); \
	}while(0)

#define IMAPX_PCM_MCLK_CFG       (0x0c)
#define IMAPX_PCM_CLKCTL         (0x08)

struct sensor_info {
	int exist;
	char name[MAX_STR_LEN];
	char interface[MAX_STR_LEN];
	int i2c_addr;
	int chn;
	int lanes;
	int csi_freq;
	int mipi_pixclk;
	char mclk_src[MAX_STR_LEN];
	int reset_pin;
	int pwd_pin;
	int imager;
	int mclk;
	int wraper_use;
	int wwidth;
	int wheight;
	int whdly;
	int wvdly;
	int mode;
	struct regulator *pwd[MAX_SEQ_NUM];
	int gpio[MAX_SEQ_NUM];
	struct clk *oclk;
	struct i2c_client *client;
	struct i2c_adapter *adapter;
	char mux[MAX_STR_LEN];
};

uint32_t freq_sd_write(u32 slot_id , u32 req_freq);
int get_sensor_board_info(struct sensor_info *pinfo, int i);
int get_bridge_sensor_board_info(struct sensor_info *pinfo, int r, int i);
int ddk_sensor_power_up(void * psensor_info, int index, int up);
int bridge_sensor_power_up(void * psensor_info, int r_index,int index, int up);
#endif /* POWER_UP_H_ */
