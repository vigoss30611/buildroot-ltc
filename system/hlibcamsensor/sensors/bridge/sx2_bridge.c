#include "sensors/bridge/bridge_dev.h"
#include "sensors/bridge/sx2_bridge_ctrl.h"

#define LOG_TAG "BRIDGE_SX2_DVP"
#include <felixcommon/userlog.h>

int sx2_dvp_init(struct _bridge_struct*bridge);
int sx2_dvp_exit(struct _bridge_struct*bridge);
int sx2_dvp_set_cfg(struct _bridge_struct*bridge, unsigned int mode);
int sx2_dvp_get_hw_cfg(struct _bridge_struct*bridge, unsigned int mode);
int sx2_dvp_set_hw_cfg(struct _bridge_struct*bridge, unsigned int mode);
int sx2_dvp_set_cfg(struct _bridge_struct*bridge, unsigned int mode);

BRIDGE_DEV sx2_dvp = {
    .name = "apollo3-sx2",
    .init = sx2_dvp_init,
    .set_cfg = sx2_dvp_set_cfg,
    .get_hw_cfg = sx2_dvp_get_hw_cfg,
    .set_hw_cfg = sx2_dvp_set_hw_cfg,
    .exit = sx2_dvp_exit,
};

void sx2_dvp_register(void)
{
    bridge_dev_register(&sx2_dvp);
}

int sx2_dvp_init(struct _bridge_struct*bridge)
{
    bridge->bridge_cfg.i2c_addr = bridge->info.i2c_addr;
    bridge->bridge_cfg.imager = 1; /*0: MIPI Gasket 1: DVP Gasket*/

    bridge->bridge_cfg.i2c_mode = 1;

    bridge->client.mode = 1;
    bridge->sensor_client.mode = 1;

    bridge->bridge_cfg.hsync = 1;
    bridge->bridge_cfg.vsync = 1;

    strncpy(bridge->bridge_cfg.sensor_name, "apollo3-sx2", MAX_STR_LEN);

    bridge->bridge_cfg.parallel = 1;
    bridge->bridge_cfg.mipi_lanes = 0;

    bridge->bridge_cfg.i2c_data_bits = 16;

    /* It will updata for bridge_sensors */
    strncpy(bridge->bridge_cfg.mosaic_type, "GRBG", 16);
    bridge->bridge_cfg.width = 2560;
    bridge->bridge_cfg.height = 720;

    bridge->bridge_cfg.widthTotal = 4632;
    bridge->bridge_cfg.heightTotal= 1104;
    bridge->bridge_cfg.fps = 30;
    bridge->bridge_cfg.pclk= 96;
    bridge->bridge_cfg.bits = 10;

    return 0;
}

int sx2_dvp_exit(struct _bridge_struct*bridge)
{
    return 0;
}

int sx2_dvp_set_cfg(struct _bridge_struct*bridge, unsigned int mode)
{
    bridge->bridge_cfg.width = bridge->sensor_cfg.width*2;
    bridge->bridge_cfg.height= bridge->sensor_cfg.height;
    bridge->bridge_cfg.widthTotal = bridge->sensor_cfg.widthTotal*2;
    bridge->bridge_cfg.heightTotal= bridge->sensor_cfg.heightTotal;
    bridge->bridge_cfg.pclk = 2 * bridge->sensor_cfg.pclk;
    bridge->bridge_cfg.fps = bridge->sensor_cfg.fps;
    bridge->bridge_cfg.bits = bridge->sensor_cfg.bits;
    bridge->bridge_cfg.i2c_data_bits =  bridge->sensor_cfg.i2c_data_bits;
    memcpy(bridge->bridge_cfg.mosaic_type, bridge->sensor_cfg.mosaic_type, 16);

    return 0;
}

int sx2_dvp_get_hw_cfg(struct _bridge_struct*bridge, unsigned int arg)
{
    int ret  =0;
    struct sx2_init_config cfg;

    ret = bridge_dev_get_cfg((void*)&cfg);
    if(ret < 0)
        return -1;

    //TODO
    return 0;
}

int sx2_dvp_set_hw_cfg(struct _bridge_struct*bridge, unsigned int arg)
{
    struct sx2_init_config cfg;

    cfg.mode = SX2_AUTO_MODE;
    cfg.freq = bridge->bridge_cfg.pclk*1000*1000;
    cfg.in_fmt.h_size = bridge->bridge_cfg.width/2;
    cfg.in_fmt.v_size = bridge->bridge_cfg.height;
    cfg.in_fmt.h_blk = bridge->bridge_cfg.widthTotal/2 - bridge->bridge_cfg.width/2;
    cfg.in_fmt.v_blk = bridge->bridge_cfg.heightTotal- bridge->bridge_cfg.height;

    return bridge_dev_set_cfg((void*)&cfg);
}

