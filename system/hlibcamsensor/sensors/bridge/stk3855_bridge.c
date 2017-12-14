#include "sensors/bridge/bridge_dev.h"

int stk3855_dvp_init(struct _bridge_struct*bridge);
int stk3855_dvp_exit(struct _bridge_struct*bridge);
int stk3855_dvp_set_cfg(struct _bridge_struct*bridge, unsigned int arg);
int stk3855_dvp_get_hw_cfg(struct _bridge_struct*bridge, unsigned int mode);
int stk3855_dvp_set_hw_cfg(struct _bridge_struct*bridge, unsigned int mode);

BRIDGE_DEV stk3855_dvp = {
    .name = "stk3855",
    .init = stk3855_dvp_init,
    .set_cfg = stk3855_dvp_set_cfg,
    .get_hw_cfg = stk3855_dvp_get_hw_cfg,
    .set_hw_cfg = stk3855_dvp_set_hw_cfg,
    .exit = stk3855_dvp_exit,
};

void stk3855_dvp_register(void)
{
    bridge_dev_register(&stk3855_dvp);
}

int stk3855_dvp_init(struct _bridge_struct*bridge)
{
    return 0;
}

int stk3855_dvp_exit(struct _bridge_struct*bridge)
{
    return 0;
}

int stk3855_dvp_set_cfg(struct _bridge_struct*bridge, unsigned int arg)
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

int stk3855_dvp_get_hw_cfg(struct _bridge_struct*bridge, unsigned int arg)
{
    int enable =0;
    int ret  =0;
    ret = bridge_dev_get_cfg((void*)&enable);
    if(ret < 0)
        return ret;

    *((int*)arg) = enable;
    return 0;
}

int stk3855_dvp_set_hw_cfg(struct _bridge_struct*bridge, unsigned int arg)
{
    return bridge_dev_set_cfg((void*)&arg);
}

