#include "sensors/bridge/bridge_dev.h"

#define LOG_TAG "BRIDGE_XC9080_MIPI"
#include <felixcommon/userlog.h>

int xc9080_mipi_init(struct _bridge_struct*bridge);
int xc9080_mipi_i2c_switch(struct _bridge_struct*bridge, unsigned int mode);
int xc9080_mipi_set_cfg(struct _bridge_struct*bridge, unsigned int arg);
int xc9080_mipi_get_hw_cfg(struct _bridge_struct*bridge, unsigned int mode);
int xc9080_mipi_set_hw_cfg(struct _bridge_struct*bridge, unsigned int mode);
int xc9080_mipi_exit(struct _bridge_struct*bridge);

BRIDGE_DEV xc9080_mipi = {
    .name = "xc9080",
    .init = xc9080_mipi_init,
    .i2c_switch = xc9080_mipi_i2c_switch,
    .set_cfg = xc9080_mipi_set_cfg,
    .get_hw_cfg = xc9080_mipi_get_hw_cfg,
    .set_hw_cfg = xc9080_mipi_set_hw_cfg,
    .exit = xc9080_mipi_exit,
};

void xc9080_mipi_register(void)
{
    bridge_dev_register(&xc9080_mipi);
}

int xc9080_mipi_init(struct _bridge_struct*bridge)
{
    bridge->bridge_cfg.i2c_addr = bridge->info.i2c_addr;
    bridge->bridge_cfg.imager = 0; /*0: MIPI Gasket 1 : For DVP Gasket*/
    bridge->bridge_cfg.i2c_data_bits = 8;
    bridge->bridge_cfg.i2c_mode = 0;
    bridge->bridge_cfg.hsync = 0;
    bridge->bridge_cfg.vsync = 0;

    strncpy(bridge->bridge_cfg.sensor_name, "xc9080", MAX_STR_LEN);

    bridge->bridge_cfg.parallel = 0;
    bridge->bridge_cfg.mipi_lanes = 4;
    strncpy(bridge->bridge_cfg.mosaic_type, "BGGR", 16);

    /* It will update for bridge_sensors */
    bridge->bridge_cfg.width = 2160; /*1080 *2*/
    bridge->bridge_cfg.height = 1080;

    bridge->bridge_cfg.widthTotal = 2560; /*(0x500)1280 * 2*/
    bridge->bridge_cfg.heightTotal=  1106; /*0x452*/
    bridge->bridge_cfg.fps = 30;
    bridge->bridge_cfg.pclk= 42.5;
    bridge->bridge_cfg.bits = 10;

    return 0;
}

int xc9080_mipi_exit(struct _bridge_struct*bridge)
{
    return 0;
}

int xc9080_mipi_i2c_switch(struct _bridge_struct*bridge, unsigned int mode)
{
    int ret = 0;
    IMG_UINT16 sensor_offline[9] = {
        0xfffd, 0x80,
        0xfffe, 0x50,
        0x004d, 0x00,
    };

    IMG_UINT16 sensor_online[9] = {
        0xfffd, 0x80,
        0xfffe, 0x50,
        0x004d, 0x03,
    };

    LOG_DEBUG("%s mode=%d\n", __func__, mode);

    if (mode == I2C_CHN_SWITCH_TO_SENSOR) //switch to hm2131 sensors i2c channel
    {
        ret = bridge_i2c_write(&bridge->client, sensor_online[0], sensor_online[1]);
        ret = bridge_i2c_write(&bridge->client, sensor_online[2], sensor_online[3]);
        ret = bridge_i2c_write(&bridge->client, sensor_online[4], sensor_online[5]);
    }
    else if (mode == I2C_CHN_SWITCH_TO_BRIDGE) //switch to bridge i2c channel
    {
        ret = bridge_i2c_write(&bridge->client, sensor_offline[0], sensor_offline[1]);
        ret = bridge_i2c_write(&bridge->client, sensor_offline[2], sensor_offline[3]);
        ret = bridge_i2c_write(&bridge->client, sensor_offline[4], sensor_offline[5]);
    }
    else
    {
        return -1;
    }

    return ret;
}

int xc9080_mipi_set_cfg(struct _bridge_struct*bridge, unsigned int arg)
{
    bridge->bridge_cfg.width = 2* bridge->sensor_cfg.width;
    bridge->bridge_cfg.height= bridge->sensor_cfg.height;
    bridge->bridge_cfg.widthTotal = 2 * bridge->sensor_cfg.widthTotal;
    bridge->bridge_cfg.heightTotal= bridge->sensor_cfg.heightTotal;
    bridge->bridge_cfg.pclk= bridge->sensor_cfg.pclk;
    bridge->bridge_cfg.fps = bridge->sensor_cfg.fps;
    bridge->bridge_cfg.bits = bridge->sensor_cfg.bits;
    bridge->bridge_cfg.i2c_data_bits = bridge->sensor_cfg.i2c_data_bits;
    memcpy(bridge->bridge_cfg.mosaic_type, bridge->sensor_cfg.mosaic_type, 16);

    return 0;
}

int xc9080_mipi_get_hw_cfg(struct _bridge_struct*bridge, unsigned int arg)
{
    int ret  =0;
    int enable =0;
    struct bridge_hw_cfg hw_cfg;
    ret = bridge_dev_get_cfg((void*)&hw_cfg);
    if(ret < 0)
        return ret;

    LOG_INFO("xc9080 get hwcfg mode=%d enable=%d\n",hw_cfg.mode, hw_cfg.enable);
    //TODO
    return 0;
}

int xc9080_mipi_set_hw_cfg(struct _bridge_struct*bridge, unsigned int arg)
{
    int ret;
    struct bridge_hw_cfg hw_cfg;

    if (bridge->bridge_cfg.i2c_forbid)
        hw_cfg.mode = XC9080_KERNAL_INIT_MODE;
    else
        hw_cfg.mode = XC9080_USER_INIT_MODE;

    hw_cfg.enable = arg;
    ret = bridge_dev_set_cfg((void*)&hw_cfg);
    return ret;
}
