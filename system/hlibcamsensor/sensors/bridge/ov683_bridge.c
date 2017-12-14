#include "sensors/bridge/bridge_dev.h"

#define LOG_TAG "BRIDGE_OV683_MIPI"
#include <felixcommon/userlog.h>

int ov683_mipi_init(struct _bridge_struct*bridge);
int ov683_mipi_i2c_switch(struct _bridge_struct*bridge, unsigned int mode);
int ov683_mipi_set_cfg(struct _bridge_struct*bridge, unsigned int arg);
int ov683_mipi_get_hw_cfg(struct _bridge_struct*bridge, unsigned int mode);
int ov683_mipi_set_hw_cfg(struct _bridge_struct*bridge, unsigned int mode);
int ov683_mipi_exit(struct _bridge_struct*bridge);

BRIDGE_DEV ov683_mipi = {
    .name = "ov683",
    .init = ov683_mipi_init,
    .i2c_switch = ov683_mipi_i2c_switch,
    .set_cfg = ov683_mipi_set_cfg,
    .get_hw_cfg = ov683_mipi_get_hw_cfg,
    .set_hw_cfg = ov683_mipi_set_hw_cfg,
    .exit = ov683_mipi_exit,
};

void ov683_mipi_register(void)
{
    LOG_DEBUG("%s\n", __func__);
    bridge_dev_register(&ov683_mipi);
}

int ov683_mipi_init(struct _bridge_struct*bridge)
{
    LOG_DEBUG("%s\n", __func__);
    bridge->bridge_cfg.i2c_addr = bridge->info.i2c_addr;
    bridge->bridge_cfg.imager = 0; /*0: MIPI Gasket 1 : For DVP Gasket*/
    bridge->bridge_cfg.i2c_data_bits = 8;
    bridge->bridge_cfg.i2c_mode = 0;
    bridge->bridge_cfg.hsync = 0;
    bridge->bridge_cfg.vsync = 0;

    strncpy(bridge->bridge_cfg.sensor_name, "ov683", MAX_STR_LEN);

    bridge->bridge_cfg.parallel = 0;
    bridge->bridge_cfg.mipi_lanes = 4;
    strncpy(bridge->bridge_cfg.mosaic_type, "BGGR", 16);

    /* It will update for bridge_sensors */
    bridge->bridge_cfg.width = 3040; /*1520 *2*/
    bridge->bridge_cfg.height = 1520;

    bridge->bridge_cfg.widthTotal =5136; /*(0xA08)2568  * 2*/
    bridge->bridge_cfg.heightTotal= 1556; /*0x5F0*/
    bridge->bridge_cfg.fps = 30;
    bridge->bridge_cfg.pclk= 120.0;
    bridge->bridge_cfg.bits = 10;

    return 0;
}

int ov683_mipi_exit(struct _bridge_struct*bridge)
{
    return 0;
}

int ov683_mipi_i2c_switch(struct _bridge_struct*bridge, unsigned int mode)
{
    int ret = 0;
    IMG_UINT16 sensor_offline[2] = {
        0x020b, 0x08,
    };

    IMG_UINT16 sensor_online[2] = {
        0x020b, 0x09,
    };

    LOG_DEBUG("%s mode=%d\n", __func__, mode);

    if (mode == I2C_CHN_SWITCH_TO_SENSOR) //switch to sensors i2c channel
    {
        ret = bridge_i2c_write(&bridge->client, sensor_online[0], sensor_online[1]);
    }
    else if (mode == I2C_CHN_SWITCH_TO_BRIDGE) //switch to bridge i2c channel
    {
        ret = bridge_i2c_write(&bridge->client, sensor_offline[0], sensor_offline[1]);
    }
    else
    {
        return -1;
    }

    return ret;
}

int ov683_mipi_set_cfg(struct _bridge_struct*bridge, unsigned int arg)
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

int ov683_mipi_get_hw_cfg(struct _bridge_struct*bridge, unsigned int arg)
{
    (void)bridge;
    (void)arg;
    return 0;
}

int ov683_mipi_set_hw_cfg(struct _bridge_struct*bridge, unsigned int arg)
{
    int ret;
    IMG_UINT16 enable_reg[2] = {
        0x6f01, 0x90,
    };

    LOG_DEBUG("%s mode=%d\n", __func__, arg);
    ret = ov683_mipi_i2c_switch(bridge, 0);
    if (arg) // stream on
    {
        enable_reg[1] = 0x90;
        ret = bridge_i2c_write(&bridge->client, enable_reg[0], enable_reg[1]);
    }
    else // stream off
    {
        enable_reg[1] = 0x00;
        ret = bridge_i2c_write(&bridge->client, enable_reg[0], enable_reg[1]);
    }
    ret = ov683_mipi_i2c_switch(bridge, 1);

    return ret;
}

