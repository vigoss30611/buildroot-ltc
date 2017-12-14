/*
 * power_up.c
 *
 *  Created on: Apr 5, 2017
 *      Author: zhouc
 */
#include "power_up.h"
#include <mach/items.h>
#include <linux/gpio.h>
#include <linux/regulator/consumer.h>
#include <linux/regulator/of_regulator.h>
#include <mach/pad.h>
#include <mach/power-gate.h>
#include <mach/imap-iomap.h>
#include <asm/io.h>
#include <linux/delay.h>

uint en_dbg = 0;
EXPORT_SYMBOL(en_dbg);

int get_sensor_board_info(struct sensor_info *pinfo, int i)
{

    char tmp[64];
    //initailize info 0
    memset((void *)pinfo, 0, sizeof(struct sensor_info));
    memset(tmp, 0, sizeof(tmp));
    sprintf(tmp, "%s%d.%s", "sensor", i, "interface");
    pr_db("tmp = %s \n", tmp);
    if(item_exist(tmp)) {
        pinfo->exist = 1;
    }
    else
    {
        pinfo->exist = 0;
        return -2;
    }
    pr_db("pinfo->exist = %d \n", pinfo->exist);
    if(pinfo->exist) {
        item_string(pinfo->interface, tmp, 0);
        pr_db("interface ;%s\n", pinfo->interface);
        memset(tmp, 0, sizeof(tmp));
        sprintf(tmp, "%s%d.%s", "sensor", i, "name");
        if(item_exist(tmp)) {
            item_string(pinfo->name, tmp, 0);
        }

        memset(tmp, 0, sizeof(tmp));
        sprintf(tmp, "%s%d.%s", "sensor", i, "mclk");
        if(item_exist(tmp)) {
            pinfo->mclk = item_integer(tmp, 0);
        }
//      memset(tmp, 0, sizeof(tmp));
//      sprintf(tmp, "%s%d.%s", "sensor", i, "reset");
//      if(item_exist(tmp)) {
//            if(item_equal(tmp, "pads", 0)) {
//                pinfo->reset_pin = item_integer(tmp, 1);
//            }
//        }
//
//      memset(tmp, 0, sizeof(tmp));
//      sprintf(tmp, "%s%d.%s", "sensor", i, "pwd");
//      if(item_exist(tmp)) {
//            if(item_equal(tmp, "pads", 0)) {
//                pinfo->pwd_pin = item_integer(tmp, 1);
//            }
//        }
//      pr_db("reset pin=%d, pwd pin=%d\n",  pinfo->reset_pin, pinfo->pwd_pin);
        memset(tmp, 0, sizeof(tmp));
        sprintf(tmp, "%s%d.%s", "sensor", i, "mipi_pixclk");
        if(item_exist(tmp)) {
            pinfo->mipi_pixclk = item_integer(tmp, 0);
        }

        memset(tmp, 0, sizeof(tmp));
        sprintf(tmp, "%s%d.%s", "sensor", i, "imager");
        if(item_exist(tmp)) {
            pinfo->imager = item_integer(tmp, 0);
        }

        memset(tmp, 0, sizeof(tmp));
        sprintf(tmp, "%s%d.%s", "sensor", i, "mode");
        if(item_exist(tmp)) {
            pinfo->mode = item_integer(tmp, 0);
        }

        memset(tmp, 0, sizeof(tmp));
        sprintf(tmp, "%s%d.%s", "sensor", i, "i2c");
        if(item_exist(tmp)) {
            pr_db("is here-->%s\n", tmp);
           if(item_equal(tmp, "chn", 0)) {
               pinfo->chn = item_integer(tmp, 1);
           }
           if(item_equal(tmp, "addr", 2)) {
               pinfo->i2c_addr = item_integer(tmp, 3);
           }
        } else {
            /* case: the i2c setting is null */
            pinfo->exist = 0;
            return -2;
        }

        pr_db("chn = %d, i2c_addr = %d\n", pinfo->chn, pinfo->i2c_addr);
        memset(tmp, 0, sizeof(tmp));
        sprintf(tmp, "%s%d.%s", "sensor", i, "csi");
        if(item_exist(tmp)) {
           if(item_equal(tmp, "lanes", 0)) {
               pinfo->lanes = item_integer(tmp, 1);
           }
           if(item_equal(tmp, "freq", 2)) {
               pinfo->csi_freq = item_integer(tmp, 3);
           }
        }

        memset(tmp, 0, sizeof(tmp));
        sprintf(tmp, "%s%d.%s", "sensor", i, "mclk.src");
        if(item_exist(tmp)) {
            item_string(pinfo->mclk_src, tmp, 0);
        }

        memset(tmp, 0, sizeof(tmp));
        sprintf(tmp, "%s%d.%s", "sensor", i, "wraper.use");
        if(item_exist(tmp)) {
            pinfo->wraper_use = item_integer(tmp, 0);
            if(pinfo->wraper_use && (!strcmp(pinfo->interface, "mipi"))) {
                pr_err("the mipi interface have no dvp wraper, please don't config wraper for mipi interface, exit!\n");
                return -1;
            }

            memset(tmp, 0, sizeof(tmp));
            sprintf(tmp, "%s%d.%s", "sensor", i, "wraper.width");
            if(item_exist(tmp)) {
                pinfo->wwidth = item_integer(tmp, 0);
            }

            memset(tmp, 0, sizeof(tmp));
            sprintf(tmp, "%s%d.%s", "sensor", i, "wraper.height");
            if(item_exist(tmp)) {
                pinfo->wheight = item_integer(tmp, 0);
            }

            memset(tmp, 0, sizeof(tmp));
            sprintf(tmp, "%s%d.%s", "sensor", i, "wraper.hdelay");
            if(item_exist(tmp)) {
                pinfo->whdly = item_integer(tmp, 0);
            }

            memset(tmp, 0, sizeof(tmp));
            sprintf(tmp, "%s%d.%s", "sensor", i, "wraper.vdelay");
            if(item_exist(tmp)) {
                pinfo->wvdly = item_integer(tmp, 0);
            }

            memset(tmp, 0, sizeof(tmp));
            sprintf(tmp, "%s%d.%s", "sensor", i, "mux");
            if(item_exist(tmp)) {
                item_string(pinfo->mux, tmp, 0);
            }
        }
    }

    return 0;
}
EXPORT_SYMBOL(get_sensor_board_info);

int get_bridge_sensor_board_info(struct sensor_info *pinfo, int r, int i)
{
	char tmp[64] = {0};

	memset((void *)pinfo, 0, sizeof(struct sensor_info));
	memset(tmp, 0, sizeof(tmp));
	sprintf(tmp, "%s%d.%s.%s%d.%s", "sensor", r, "bridge","sensor", i, "interface");

	pr_db("tmp = %s \n", tmp);
	if(item_exist(tmp)) {
		pinfo->exist = 1;
	} else {
		pinfo->exist = 0;
		return -2;
	}

	pr_db("pinfo->exist = %d \n", pinfo->exist);

	item_string(pinfo->interface, tmp, 0);
	pr_db("interface ;%s\n", pinfo->interface);

	memset(tmp, 0, sizeof(tmp));
	sprintf(tmp, "%s%d.%s.%s%d.%s", "sensor", r, "bridge","sensor", i, "name");
	if(item_exist(tmp)) {
		item_string(pinfo->name, tmp, 0);
	}

	memset(tmp, 0, sizeof(tmp));
	sprintf(tmp, "%s%d.%s.%s%d.%s", "sensor", r, "bridge","sensor", i, "mclk");
	if(item_exist(tmp)) {
		pinfo->mclk = item_integer(tmp, 0);
	}

	memset(tmp, 0, sizeof(tmp));
	sprintf(tmp, "%s%d.%s.%s%d.%s", "sensor", r, "bridge","sensor", i, "mipi_pixclk");
	if(item_exist(tmp)) {
		pinfo->mipi_pixclk = item_integer(tmp, 0);
	}

	memset(tmp, 0, sizeof(tmp));
	sprintf(tmp, "%s%d.%s.%s%d.%s", "sensor", r, "bridge","sensor", i, "imager");
	if(item_exist(tmp)) {
		pinfo->imager = item_integer(tmp, 0);
	}

	memset(tmp, 0, sizeof(tmp));
	sprintf(tmp, "%s%d.%s.%s%d.%s", "sensor", r, "bridge","sensor", i, "mode");
	if(item_exist(tmp)) {
		pinfo->mode = item_integer(tmp, 0);
	}

	memset(tmp, 0, sizeof(tmp));
	sprintf(tmp, "%s%d.%s.%s%d.%s", "sensor", r, "bridge","sensor", i, "i2c");
	if(item_exist(tmp)) {
		pr_db("is here-->%s\n", tmp);
		if(item_equal(tmp, "chn", 0)) {
			pinfo->chn = item_integer(tmp, 1);
		}

		if(item_equal(tmp, "addr", 2)) {
			pinfo->i2c_addr = item_integer(tmp, 3);
		}
	} else {
		/* case: the i2c setting is null */
		pinfo->exist = 0;
		return -2;
	}

	pr_db("chn = %d, i2c_addr = %d\n", pinfo->chn, pinfo->i2c_addr);
	memset(tmp, 0, sizeof(tmp));
	sprintf(tmp, "%s%d.%s.%s%d.%s", "sensor", r, "bridge","sensor", i, "csi");
	if(item_exist(tmp)) {
		if(item_equal(tmp, "lanes", 0)) {
			pinfo->lanes = item_integer(tmp, 1);
		}

		if(item_equal(tmp, "freq", 2)) {
			pinfo->csi_freq = item_integer(tmp, 3);
		}
	}

	memset(tmp, 0, sizeof(tmp));
	sprintf(tmp, "%s%d.%s.%s%d.%s", "sensor", r, "bridge","sensor", i, "mclk.src");
	if(item_exist(tmp)) {
		item_string(pinfo->mclk_src, tmp, 0);
	}

	memset(tmp, 0, sizeof(tmp));
	sprintf(tmp, "%s%d.%s.%s%d.%s", "sensor", r, "bridge","sensor", i, "wraper.use");
	if(item_exist(tmp)) {
		pinfo->wraper_use = item_integer(tmp, 0);
		if(pinfo->wraper_use && (!strcmp(pinfo->interface, "mipi"))) {
			pr_err("the mipi interface have no dvp wraper, please don't config wraper for mipi interface, exit!\n");
			return -1;
		}

		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "%s%d.%s.%s%d.%s", "sensor", r, "bridge","sensor", i, "wraper.width");
		if(item_exist(tmp)) {
			pinfo->wwidth = item_integer(tmp, 0);
		}

		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "%s%d.%s.%s%d.%s", "sensor", r, "bridge","sensor", i, "wraper.height");
		if(item_exist(tmp)) {
			pinfo->wheight = item_integer(tmp, 0);
		}

		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "%s%d.%s.%s%d.%s", "sensor", r, "bridge","sensor", i, "wraper.hdelay");
		if(item_exist(tmp)) {
			pinfo->whdly = item_integer(tmp, 0);
		}

		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "%s%d.%s.%s%d.%s", "sensor", r, "bridge","sensor", i, "wraper.vdelay");
		if(item_exist(tmp)) {
			pinfo->wvdly = item_integer(tmp, 0);
		}
	}

	memset(tmp, 0, sizeof(tmp));
	sprintf(tmp, "%s%d.%s.%s%d.%s", "sensor", r, "bridge","sensor", i, "mux");
	if(item_exist(tmp)) {
		item_string(pinfo->mux, tmp, 0);
	}

	return 0;
}
EXPORT_SYMBOL(get_bridge_sensor_board_info);

static int ddk_sensor_set_voltage(void * psensor_info, char * itm, int val, int index, int i)
{
    int tmp, ret = 0;
    struct sensor_info *ddk_sensor_info;
    ddk_sensor_info = (struct sensor_info *)psensor_info;
    if (!strcmp(itm, "gpio")) {
        tmp = (val > 0) ? val : -(val);
        ret = gpio_request(tmp, "pwd_gpio");
        if(ret != 0 ) {
            //pr_err("power gpio_request %x error!\n", tmp);
            return ret;
        }
        if(val > 0) {
            gpio_direction_output(tmp, 1);
        }
        else {
            gpio_direction_output(tmp, 0);
        }

    }
    else {
        pr_db("itm = %s, val = %d\n", itm, val);
        ddk_sensor_info[index].pwd[i] = regulator_get(NULL, itm);
        if (IS_ERR(ddk_sensor_info[index].pwd[i])) {
            pr_err("regulator get %s failed\n", itm);
            return PTR_ERR(ddk_sensor_info[index].pwd[i]);
        }
        ret = regulator_set_voltage(ddk_sensor_info[index].pwd[i], val, val);
        if(ret) {
            pr_err("regulator_set_voltage failed\n");
            return ret;
        }
        ret = regulator_enable(ddk_sensor_info[index].pwd[i]);
        if(ret) {
            pr_err("regulator enable failed\n");
            return ret;
        }
    }
    return ret;
}

static int auto_switch_pll(struct clk *des_clk, unsigned int freq)
{
    int ret = 0;
    struct clk *parent;
    unsigned long parent_clk_rate, clk_mod;
    char parent_str[MAX_STR_LEN] = {0};

    parent = clk_get_parent(des_clk);
    if (IS_ERR(parent)) {
        pr_err("Failed to get mclk parent, ret=%ld\n", PTR_ERR(parent));
        return PTR_ERR(parent);
    }
    parent_clk_rate = clk_get_rate(parent);
    clk_mod = do_div(parent_clk_rate, freq);

    if (clk_mod && !strcmp(__clk_get_name(parent), "vpll")) {
        pr_db(">>> Parent %s can NOT satisfy destination clock %d\n", __clk_get_name(parent), freq);
        pr_db(">>> Try %s\n", "dpll");
        parent = clk_get_sys(NULL, "dpll");
        if (IS_ERR(parent)) {
            pr_err("Failed to get clk %s, ret=%ld\n", parent_str, PTR_ERR(parent));
            return PTR_ERR(parent);
        }
        parent_clk_rate = clk_get_rate(parent);
        clk_mod = do_div(parent_clk_rate, freq);
    }

    if (clk_mod) {
        pr_db(">>> Parent %s can NOT satisfy destination clock %d\n", __clk_get_name(parent), freq);
        if (!strcmp(__clk_get_name(parent) , "dpll")) {
            strcpy(parent_str, "epll");
        }
        else if (!strcmp(__clk_get_name(parent), "epll")) {
            strcpy(parent_str, "dpll");
        }
        else {
            pr_err("Failed to recognize parent %s\n", (char *)__clk_get_name(parent));
            return -EINVAL;
        }
        pr_err(">>> Try %s\n", parent_str);
        parent = clk_get_sys(NULL, parent_str);
        if (IS_ERR(parent)) {
            pr_err("Failed to get clk %s, ret=%ld\n", parent_str, PTR_ERR(parent));
            return PTR_ERR(parent);
        }
        parent_clk_rate = clk_get_rate(parent);
        clk_mod = do_div(parent_clk_rate, freq);
        if (!clk_mod) {
            pr_db(">>> %s can satisfy destination clock %d, now switch parent to %s\n", parent_str, freq, (char *)__clk_get_name(parent));
            clk_set_parent(des_clk, parent);
        }
        else {
            pr_db(">>> %s also can NOT satisfy destination clock %d, check sensor configuration!!!\n", parent_str, freq);
        }
    }
    else
        pr_db(">>> %s can satisfy destination clock %d\n", (char *)__clk_get_name(parent), freq);

    ret = clk_set_rate(des_clk, freq);
    if (0 > ret){
        pr_err("Fail to set mclk output %d clock\n", freq);
        return ret;
    }

    return 0;
}

static int ddk_sensor_set_clock(void * psensor_info, int val, int index)
{
    int ret = 0;
    struct resource *ioarea = NULL;
    void __iomem *pcm_reg = NULL;
    struct sensor_info *ddk_sensor_info;
    ddk_sensor_info = (struct sensor_info *)psensor_info;
    gpio_request(val, "mclk_pin");
    imapx_pad_set_mode(val, "function");
    if (!strcmp(ddk_sensor_info[index].mclk_src, CLK_SRC_PCM)) {
        module_power_on(SYSMGR_PCM_BASE);
        imapx_pad_init("pcm0");
        ioarea = request_mem_region(IMAP_PCM0_BASE, IMAP_PCM0_SIZE, CLK_SRC_PCM);
        if(IS_ERR(ioarea)) {
            pr_err("failed to request pcm0 register area in ddk sensor driver\n");
            return -EBUSY;
        }
        pcm_reg = ioremap(IMAP_PCM0_BASE, IMAP_PCM0_SIZE);
        if(IS_ERR(pcm_reg)) {
            pr_err("failed to ioremap pcm0 register in  in ddk sensor driver\n");
            return PTR_ERR(pcm_reg);
        }
        //use what frequence you want to do
        writel(0x13, pcm_reg + IMAPX_PCM_CLKCTL);
        writel(4, pcm_reg + IMAPX_PCM_MCLK_CFG);
    }
    else if (!strcmp(ddk_sensor_info[index].mclk_src,  CLK_SRC_SD)) {
        ddk_sensor_info[index].oclk = clk_get_sys("imap-mmc.0", CLK_SRC_SD);
        if (IS_ERR(ddk_sensor_info[index].oclk)) {
            pr_err("fail to get clk 'imap-mmc.0'\n");
            return PTR_ERR(ddk_sensor_info[index].oclk);
        }
        // MMC internal divide by 2
        ret = auto_switch_pll(ddk_sensor_info[index].oclk, ddk_sensor_info[index].mclk*2);
        if (ret)
            return ret;
        clk_prepare_enable(ddk_sensor_info[index].oclk);
#ifdef CONFIG_INFT_MMC0_CLK_CAMERA
        freq_sd_write(0, ddk_sensor_info[index].mclk);
#endif
    }
    else { //Q3, Q3S, Q3I:imap-clk.0 //Q3F: imap-camo, imap-clk.1,
        ddk_sensor_info[index].oclk = clk_get_sys(ddk_sensor_info[index].mclk_src, ddk_sensor_info[index].mclk_src);
        if (IS_ERR( ddk_sensor_info[index].oclk)) {
            pr_err("fail to get camera clock %s\n", ddk_sensor_info[index].mclk_src);
            return PTR_ERR( ddk_sensor_info[index].oclk);
        }
        ret = auto_switch_pll(ddk_sensor_info[index].oclk, ddk_sensor_info[index].mclk);
        if (ret)
            return ret;
        clk_prepare_enable(ddk_sensor_info[index].oclk);
    }

    return 0;
}

static int ddk_sensor_remove_clock(void * psensor_info, struct i2c_client *client)
{
    int index = 0;
    struct sensor_info *ddk_sensor_info;
    ddk_sensor_info = (struct sensor_info *)psensor_info;
    index =*(int *)(client->dev.platform_data);
    pr_db("index = %d \n", index);
    if(ddk_sensor_info[index].oclk){
        clk_disable_unprepare(ddk_sensor_info[index].oclk);
    }

    if(!strcmp(ddk_sensor_info[index].mclk_src, CLK_SRC_SD))
        module_power_down(SYSMGR_PCM_BASE);

    return 0;
}

int ddk_sensor_power_up(void * psensor_info, int index, int up)
{
    int  i = 0, ret = 0;
    char tmp[MAX_STR_LEN] = {0};
    char tag[MAX_STR_LEN] = {0};
    char itm[MAX_STR_LEN] = {0};
    int dly, val;
    int temp;
    struct sensor_info *ddk_sensor_info;
    ddk_sensor_info = (struct sensor_info *)psensor_info;
    if (up) {

        for(i = 0; i < MAX_SEQ_NUM; i++) {
            sprintf(tmp, "%s%d.%s.%d", "sensor", index, "bootup.seq", i);
            pr_db("tmp = %s\n", tmp);
            if(item_exist(tmp)) {
                item_string(tag, tmp, 0);
                item_string(itm, tmp, 1);
                val = item_integer(tmp, 2);
                dly = item_integer(tmp, 3);
                if(!strcmp(tag, "pmu")) {
                    //set vdd iovdd etc voltage
                    ddk_sensor_set_voltage((void * )ddk_sensor_info, itm,  val, index,  i);
                    if (!strcmp(itm, "gpio")) {
                        ddk_sensor_info[index].gpio[i] = val;
                    }
                }
                else if(!strcmp(tag, "pwd")) {
                    //set power down
                    temp = (val > 0) ? val : -(val);
                    ret = gpio_request(temp, "power_down");
                    if(ret != 0 ) {
                        pr_err("power down request %d error!\n", temp);
                        //return ret;
                    }

                    if(val > 0) {
                        gpio_direction_output(temp, 1);
                    }
                    else {
                        gpio_direction_output(temp, 0);
                    }

                    ddk_sensor_info[index].gpio[i] = val;
                }
                else if(!strcmp(tag, "reset")) {
                    //reset
                    temp = (val > 0) ? val : -(val);
                    ret = gpio_request(temp, "rst_pin");
                    if(ret != 0 )
                    {
                        printk("gpio_request reset pin :%d error!\n", temp);
                        //return ret;
                    }
                    if(val > 0) {
                        gpio_direction_output(temp, 1);
                        mdelay(dly);
                        gpio_set_value(temp, 0);
                        mdelay(dly);
                        gpio_set_value(temp, 1);
                    }
                    else
                    {
                        gpio_direction_output(temp, 0);
                        mdelay(dly);
                        gpio_set_value(temp, 1);
                        mdelay(dly);
                        gpio_set_value(temp, 0);
                    }

                    ddk_sensor_info[index].gpio[i] = val;
                    ddk_sensor_info[index].reset_pin = val;
                }
                else if(!strcmp(tag, "extra")) {
                    //extra gpio
                    temp = (val > 0) ? val : -(val);
                    ret = gpio_request(temp, "extra_gpio");
                    if(ret != 0 ) {
                       pr_err("extral gpio request %d error!\n", temp);
                       //return ret;
                    }

                    if(val > 0) {
                       gpio_direction_output(temp, 1);
                    }
                    else {
                       gpio_direction_output(temp, 0);
                    }

                    ddk_sensor_info[index].gpio[i] = val;
                }
                else if(!strcmp(tag, "mclk")) {
                    //set clock
                    ddk_sensor_set_clock((void *)ddk_sensor_info, val, index);

                }
                barrier();
                mdelay(dly);
            }
        }

    }
    else
    {
        int val = 0;
        for(i = 0; i < MAX_SEQ_NUM; i++)
        {
            if(ddk_sensor_info[index].gpio[i] != 0) {
                val = ddk_sensor_info[index].gpio[i];
                if(ddk_sensor_info[index].gpio[i] > 0) {
                    gpio_set_value(val, 0);
                }
                else {
                    gpio_set_value(val, 1);
                }
                mdelay(1);
                gpio_free(ddk_sensor_info[index].gpio[i]);
            }

            if(ddk_sensor_info[index].pwd[i]) {
                regulator_disable(ddk_sensor_info[index].pwd[i]);
                regulator_put(ddk_sensor_info[index].pwd[i]);
                ddk_sensor_info[index].pwd[i] = NULL;
                mdelay(1);
            }
        }
        if (ddk_sensor_info[index].client)
            ddk_sensor_remove_clock((void *)ddk_sensor_info, ddk_sensor_info[index].client);
    }

    return 0;
}
EXPORT_SYMBOL(ddk_sensor_power_up);

int bridge_sensor_power_up(void * psensor_info, int r_index,int index, int up)
{
	int  i = 0, ret = 0;
	char tmp[MAX_STR_LEN] = {0};
	char tag[MAX_STR_LEN] = {0};
	char itm[MAX_STR_LEN] = {0};
	int dly, val;
	int temp;
	struct sensor_info *ddk_sensor_info;
	ddk_sensor_info = (struct sensor_info *)psensor_info;

	if (up) {
		for(i = 0; i < MAX_SEQ_NUM; i++) {
			sprintf(tmp, "%s%d.%s.%s%d.%s.%d",
				"sensor", r_index, "bridge",
				"sensor", index,"bootup.seq", i);

			pr_db("tmp = %s\n", tmp);
			if(item_exist(tmp)) {
				item_string(tag, tmp, 0);
				item_string(itm, tmp, 1);
				val = item_integer(tmp, 2);
				dly = item_integer(tmp, 3);

				if(!strcmp(tag, "pmu")) {
					//set vdd iovdd etc voltage
					ddk_sensor_set_voltage((void * )ddk_sensor_info, itm,  val, index,  i);
					if (!strcmp(itm, "gpio")) {
						ddk_sensor_info[index].gpio[i] = val;
					}
				} else if(!strcmp(tag, "pwd")) {
					//set power down
					temp = (val > 0) ? val : -(val);
					ret = gpio_request(temp, "power_down");
					if(ret != 0 ) {
						pr_err("power down request %d error!\n", temp);
						//return ret;
					}

					if(val > 0) {
						gpio_direction_output(temp, 1);
					} else {
						gpio_direction_output(temp, 0);
					}

					ddk_sensor_info[index].gpio[i] = val;
				} else if(!strcmp(tag, "reset")) {
					//reset
					temp = (val > 0) ? val : -(val);
					ret = gpio_request(temp, "rst_pin");
					if(ret != 0 ) {
						printk("gpio_request reset pin :%d error!\n", temp);
						//return ret;
					}

					if(val > 0) {
						gpio_direction_output(temp, 1);
						mdelay(dly);
						gpio_set_value(temp, 0);
						mdelay(dly);
						gpio_set_value(temp, 1);
					} else {
						gpio_direction_output(temp, 0);
						mdelay(dly);
						gpio_set_value(temp, 1);
						mdelay(dly);
						gpio_set_value(temp, 0);
					}

					ddk_sensor_info[index].gpio[i] = val;
				} else if(!strcmp(tag, "extra")) {
					//extra gpio
					temp = (val > 0) ? val : -(val);
					ret = gpio_request(temp, "extra_gpio");
					if(ret != 0 ) {
						pr_err("extral gpio request %d error!\n", temp);
						//return ret;
					}

					if(val > 0) {
						gpio_direction_output(temp, 1);
					} else {
						gpio_direction_output(temp, 0);
					}

					ddk_sensor_info[index].gpio[i] = val;
				} else if(!strcmp(tag, "mclk")) {
					//set clock
					ddk_sensor_set_clock((void *)ddk_sensor_info, val, index);
				}
				barrier();
				mdelay(dly);
			}
		}

	}else {
		int val = 0;
		for(i = 0; i < MAX_SEQ_NUM; i++) {
			if(ddk_sensor_info[index].gpio[i] != 0) {
				val = ddk_sensor_info[index].gpio[i];
				if(ddk_sensor_info[index].gpio[i] > 0) {
					gpio_set_value(val, 0);
				} else {
					gpio_set_value(val, 1);
				}
				mdelay(1);
				gpio_free(ddk_sensor_info[index].gpio[i]);
			}

			if(ddk_sensor_info[index].pwd[i]) {
				regulator_disable(ddk_sensor_info[index].pwd[i]);
				regulator_put(ddk_sensor_info[index].pwd[i]);
				ddk_sensor_info[index].pwd[i] = NULL;
				mdelay(1);
			}
		}

		if (ddk_sensor_info[index].client)
			ddk_sensor_remove_clock((void *)ddk_sensor_info,
									ddk_sensor_info[index].client);
	}

	return 0;
}
EXPORT_SYMBOL(bridge_sensor_power_up);

