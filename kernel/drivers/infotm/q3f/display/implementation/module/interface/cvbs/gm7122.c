/***************************************************************************** 
** GM7122 - Guoteng Electronic Technology Co., Ltd. CVBS video encoder driver
**
** Copyright (c) 2012~2112 ShangHai Infotm Ltd all rights reserved. 
** 
** Use of Infotm's code is governed by terms and conditions 
** stated in the accompanying licensing statement. 
** 
** Description: Implementation file of cvbs driver, linux environment 
**
*****************************************************************************/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/err.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <mach/gpio.h>

#include <mach/pad.h>
#include <mach/items.h>
#include <dss_common.h>

#include "gm7122.h"
#include "cvbs.h"

#define DSS_LAYER   "[dss]"
#define DSS_SUB1    "[interface]"
#define DSS_SUB2    "[gm7122]"

static char *f_string[] = {
	"CVBS_PAL",
	"CVBS_NTSC",
	"YPbPr",
	"Invalid"
};
static int gm7122_registers_dump(struct gm7122 *gm7122_data);       
static int gm7122_init_registers(struct gm7122 *gm7122_data);       
static int gm7122_codec_mute(struct gm7122 * gm7122_data, int mute);

struct gm7122 *m_gm7122;

static int gm7122_reset_pin;
static int gm7122_pa_en;

extern struct cvbs_ops *m_cvbs;

static unsigned char valid_regs[19] = {0x28, 0x29, 0x2F, 0x3A, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x61, 
								0x62, 0x63, 0x64, 0x65, 0x66, 0x6B, 0x6C, 0x6D, 0x75};

static unsigned char gm7122_regs_common1[26][2] = {
	{0x28,0x21},
	{0x29,0x1D},
	{0x2A,0x00},
	{0x2B,0x00},
	{0x2C,0x00},
	{0x2D,0x00},
	{0x2E,0x00},
	{0x2F,0x00},
	{0x30,0x00},
	{0x31,0x00},
	{0x32,0x00},
	{0x33,0x00},
	{0x34,0x00},
	{0x35,0x00},
	{0x36,0x00},
	{0x37,0x00},
	{0x38,0x00},
	{0x39,0x00},

	//{0x3A,0x93},	//color  strape 
	//{0x3A,0x03},		//data	\u4e3b\u6a21\u5f0f		Origin is 0x13
	
	{0x3A,0x13},		//data	master mode  by kevin 2013/06/07 
	
	//{0x3A,0x03},	//sync from rcv1 and rcv2   \u4ece\u6a21\u5f0f

	{0x5A,0x00},
	{0x5B,0x6D},
	{0x5C,0x9F},
	{0x5D,0x0e},
	{0x5E,0x1C},
	{0x5F,0x35},
	{0x60,0x00},
};

static unsigned char gm7122_regs_common2[5][2] = {
	{0x62,0x3B},		//RTCI Enable	
	{0x6B,0x00},		// \u4ece\u6a21\u5f0f 7121C 20110105
//	{0x6C,0x01},		//\u4e3b\u6a21\u5f0f
//	{0x6D,0x20},		//\u4e3b\u6a21\u5f0f
	{0x6C,0xff},		//\u4e3b\u6a21\u5f0f
	{0x6D,0xff},		//\u4e3b\u6a21\u5f0f
	{0x75,0xff},
};

static unsigned char gm7122_regs_pal[5][2] = {
	{0x61,0x06},		//PAL
	{0x63,0xCB},		//PAL
	{0x64,0x8A},		//PAL
	{0x65,0x09},		//PAL
	{0x66,0x2A},		//PAL
};

static unsigned char gm7122_regs_ntsc[5][2] = {
	{0x61,0x01},		//NTSC
	{0x63,0x1F},		//NTSC
	{0x64,0x7C},		//NTSC
	{0x65,0xF0},		//NTSC
	{0x66,0x21},		//NTSC
};

static int i2cRead(struct i2c_client *client, unsigned char addr, unsigned char *data)
{
	int i, ret;
	unsigned char buf[2] = { addr, 0xFF };

	struct i2c_msg msgs[] = { 
		{
			.addr	= client->addr,
			.flags	= 0,
			.len		= 1,
			.buf		= &buf[0],
		},{
			.addr	= client->addr,
			.flags	= I2C_M_RD,
			.len		= 1,
			.buf		= &buf[1],
		}
	};

	for (i = 0; i < 5; i++) {
		ret = i2c_transfer(client->adapter, msgs, 2);
		if (ret == 2)
			break;
		else {
			dss_err("i2c_transfer return %d. Retry %d time.\n", ret, i+1);
			continue;
		}
	}
	if (i >= 5) {
		dss_err("Tryed to read 5 times, still fail, give up.\n");
		return ret;
	}

	*data = buf[1];
	
	return 0;
}

static int i2cWrite(struct i2c_client *client, unsigned char addr, unsigned char data)
{
	int i, ret;
	unsigned char buf[2] = { addr, data };

	struct i2c_msg msgs[] = {
		{
			.addr	= client->addr,
			.flags	= 0,
			.len		= 2,
			.buf		= buf,
		}
	};

	//gm7122_dbg("Params: addr=0x%02X data=0x%02X\n", addr, data);

	for (i = 0; i < 5; i++) {
		ret = i2c_transfer(client->adapter, msgs, 1);
		if (ret == 1)
			break;
		else {
			dss_err("i2c_transfer return %d. Retry %d time.\n", ret, i+1);
			continue;
		}
	}
	if (i >= 5) {
		dss_err("Have tried to write 5 times, still fail, give up.\n");
		return -1;
	}
	
	return 0;
}

static int gm7122_i2cRead(struct i2c_client *client, unsigned char data[ ][2], unsigned int length)
{
	int i, ret;

	//gm7122_dbg("Params: SlaveAddr = 0x%02X\n", client->addr);

	for (i = 0; i < length; i++ ) {
		ret = i2cRead(client, data[i][0], &data[i][1]);
		if (ret)
			return ret;
	}

	return 0;
}

static int gm7122_i2cWrite(struct i2c_client *client, unsigned char data[ ][2], unsigned int length)
{
	int i, ret;

	//gm7122_dbg("Params: SlaveAddr = 0x%02X\n", client->addr);

	for (i = 0; i < length; i++ ) {
		ret = i2cWrite(client, data[i][0], data[i][1]);
		if (ret)
			return ret;
	}

	return 0;
}

static int cvbs_config(int VIC)
{
	struct gm7122 *gm7122_data = m_gm7122;
	
	dss_dbg("VIC = %d\n", VIC);
	
	if (VIC == 3)
		gm7122_data->format = f_NTSC;
	else if (VIC == 18)
		gm7122_data->format = f_PAL;
	
	if(item_exist(P_CVBS_RESET)) {
		gpio_set_value(gm7122_reset_pin, 1);
		msleep(250);
		gpio_set_value(gm7122_reset_pin, 0);
		msleep(250);
		gpio_set_value(gm7122_reset_pin, 1);
	}
	
	if(item_exist(P_CVBS_PA_EN)) {
		gpio_set_value(gm7122_pa_en, 1);
	}

	gm7122_init_registers(gm7122_data);
	gm7122_codec_mute(gm7122_data, 0);
	gm7122_data->gm7122_on = 1;

	return 0;
}

static int cvbs_disable(void)
{
	struct gm7122 *gm7122_data = m_gm7122;
	
	dss_dbg("Running...\n");

	gm7122_data->gm7122_on = 0;
	gm7122_codec_mute(gm7122_data, 1);

	if(item_exist(P_CVBS_PA_EN))
	{
		gpio_set_value(gm7122_pa_en, 0);
	}
	
	if(item_exist(P_CVBS_RESET))
	{
		gpio_set_value(gm7122_reset_pin, 0);
		msleep(500);
	}
	
	return 0;
}

static int gm7122_registers_dump(struct gm7122 *gm7122_data)
{
	struct i2c_client *client = gm7122_data->client;
	int i,j;
	unsigned char reg;

	printk("Reg  Hex     D7  D6  D5  D4  D3  D2  D1  D0\n");
	printk("     -----------------------------------------------\n");
	for (i = 0; i < sizeof(valid_regs); i++) {
		printk("%02XH| ", valid_regs[i]);
		i2cRead(client, valid_regs[i], &reg);
		printk(" %02X  - ", reg);
		for (j = 0; j < 8; j++)
			printk(" %d  ", (reg >> (7-j)) & 0x01);
		printk("\n");
	}
	printk("     -----------------------------------------------\n");
	
	return 0;
}

static int gm7122_init_registers(struct gm7122 *gm7122_data)
{
	int i, ret;
	unsigned char buf[5][2] = {{0}};
	int format = gm7122_data->format;

	dss_dbg("Initializing gm7122 registers...\n");
	dss_info("Output format set to %s\n", f_string[format]);
	if (format >= f_END)
		return -EINVAL;

	if (gm7122_data->i2c_forbidden) {
		dss_err("i2c forbidden due to i2c error during initialzing time.\n");
		return -ENXIO;
	}

	/* GM7122 digital core work depend on 27MHz clock through LLC pin. */
	/* So, TVIF CLK output must be avaliable before I2C communication. */
	msleep(50);

	/* Config */
	gm7122_i2cWrite(gm7122_data->client, gm7122_regs_common1, sizeof(gm7122_regs_common1)/2);
	if (format == f_PAL)
		gm7122_i2cWrite(gm7122_data->client, gm7122_regs_pal, sizeof(gm7122_regs_pal)/2);
	else if (format == f_NTSC)
		gm7122_i2cWrite(gm7122_data->client, gm7122_regs_ntsc, sizeof(gm7122_regs_ntsc)/2);
	else {
		dss_err("Invalid format %d\n", format);
		return -1;
	}
	gm7122_i2cWrite(gm7122_data->client, gm7122_regs_common2, sizeof(gm7122_regs_common2)/2);
	msleep(10);

	/* Check */
	for (i = 0; i < sizeof(buf)/2; i++)
		buf[i][0] = gm7122_regs_pal[i][0];		// Copy Address
	gm7122_i2cRead(gm7122_data->client, buf, sizeof(buf)/2);
	if (format == f_PAL)
		ret = strncmp((char *)buf, (char *)gm7122_regs_pal, 5*2);
	else
		ret = strncmp((char *)buf, (char *)gm7122_regs_ntsc, 5*2);
	if (!ret) {
		dss_info("PAL/NTSC registers compare complete. All same.\n");
	//	gm7122_registers_dump(gm7122_data);
		return 0;
	}
	else {
		dss_err("PAL/NTSC registers compare complete. Not equal. Read values:\n");
		gm7122_registers_dump(gm7122_data);
		return -EIO;
	}
}

static int gm7122_codec_mute(struct gm7122 * gm7122_data, int mute)
{
	dss_info("Mute %s\n", mute?"ON":"OFF");
	return 0;
}

static int gm7122_initialize(struct gm7122 *gm7122_data)
{
	dss_info("Initializing gm7122...\n");

	/* Init ids_module */
	
	m_cvbs->config = cvbs_config;
	m_cvbs->disable = cvbs_disable;

	return 0;
}

static int gm7122_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int ret = 0;
	struct gm7122 *gm7122_data;

	if (unlikely(strcmp(id->name, GM7122_NAME) != 0)) {
		dss_err("i2c_device_id %s not match, when calling %s().\n"
				"Maybe something wrong.\n", __func__, id->name);
		ret = -ENODEV;
		goto out;
	}
	dss_info("GM7122 Probe running...\n");
	dss_dbg("DEBUG ON...\n");

	/* Allocate struct gm7122 */
	dss_dbg("Allocating struct gm7122...\n");
	gm7122_data = kzalloc(sizeof(struct gm7122), GFP_KERNEL);
	if (!gm7122_data) {
		dss_err("Failed to allocate memory\n");
		ret = -ENOMEM;
		goto out_data;
	}

	/* Set private data */
	gm7122_data->client = client;
	i2c_set_clientdata(client, gm7122_data);

	/* Initialize codec mute gpio */
	if(item_exist(P_CODEC_MUTE))
	{
		int index;
		index = item_integer(P_CODEC_MUTE, 1);
	//	pad_set(index, HIGH);
	}

	/* Initialize gm7122 reset gpio */
	if(item_exist(P_CVBS_RESET))
	{
		gm7122_reset_pin = item_integer(P_CVBS_RESET, 1);
		if (gpio_is_valid(gm7122_reset_pin)) {
			ret = gpio_request(gm7122_reset_pin, "cvbs_reset");
			if (ret) {
				dss_err("failed request gpio for cvbs_reset\n");
				goto err_request_cvbs_reset;
			}
			gpio_direction_output(gm7122_reset_pin, 0);
		}
	}

	if(item_exist(P_CVBS_PA_EN))
	{
		gm7122_pa_en = item_integer(P_CVBS_PA_EN, 1);
		if (gpio_is_valid(gm7122_pa_en)) {
			ret = gpio_request(gm7122_pa_en, "cvbs_pa_en");
			if (ret) {
				dss_err("failed request gpio for cvbs pa en\n");
				goto err_request_cvbs_pa_en;
			}
			gpio_direction_output(gm7122_pa_en, 0);
		}
	}

	m_gm7122 = gm7122_data;

	/* Initialze gm7122 register */
	ret = gm7122_initialize(gm7122_data);
	if (ret) {
		dss_err("Failed to initialize gm7122 chip...\n");
		goto out;
	}

	dss_dbg("gm7122 probe End Successfully.\n");
	
	return 0;

err_request_cvbs_reset:
	if (gpio_is_valid(gm7122_reset_pin))
		gpio_free(gm7122_reset_pin);
err_request_cvbs_pa_en:
	if (gpio_is_valid(gm7122_pa_en))
		gpio_free(gm7122_pa_en);
out_data:
	kfree(gm7122_data);
out:
	return ret;
	
}

static int gm7122_remove(struct i2c_client *client)
{
	struct gm7122 *gm7122_data;
	gm7122_data = i2c_get_clientdata(client);
	
	i2c_set_clientdata(client, NULL);
	kfree(gm7122_data);
	
	return 0;
}

/* The devices this driver support */
static const struct i2c_device_id gm7122_i2c_id[] = {
	{ GM7122_NAME, 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, gm7122_i2c_id);

static struct i2c_driver gm7122_driver = {
	.driver	= {
			.name = GM7122_NAME,
			.owner	= THIS_MODULE,
	},
	.probe	= gm7122_probe,
	.remove	= gm7122_remove,
	.id_table	= gm7122_i2c_id,
};

static int init = 0;
int gm7122_init(void)
{
	struct i2c_board_info info;
	int nr;
	struct i2c_adapter *adapter;
	struct i2c_client *client;
	int ret = 0;

	dss_dbg("gm7122_init()\n");

	if (init == 0) {
		memset(&info, 0, sizeof(struct i2c_board_info));
		info.addr = GM7122_I2C_ADDR;
		strlcpy(info.type, GM7122_I2C_NAME, I2C_NAME_SIZE);
		nr = GM7122_NR;
		if (item_exist(P_CVBS_I2C_CTL)) {
			nr = item_integer(P_CVBS_I2C_CTL, 1);
			dss_err("nr %d\n", nr);
		}

		adapter = i2c_get_adapter(item_integer(P_CVBS_I2C_CTL, 1));
		if (!adapter) {
			dss_err("Failed to get adapter %d\n", nr);
			ret = -ENODEV;
			goto out;
		}

		client = i2c_new_device(adapter, &info);
		if (!client) {
			dss_err("Failed to create i2c new device.\n");
			ret = -EBUSY;
			goto out_get;
		}

		ret = i2c_add_driver(&gm7122_driver);
		if (ret) {
			dss_err("Failed to add i2c driver.\n");
			goto out_get;
		}
		init = 1;
	}

	return 0;

out_get:
    i2c_put_adapter(adapter);
out:
    return ret;
}

