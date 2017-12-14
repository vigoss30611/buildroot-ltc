#ifndef __BRIDGE_H_
#define __BRIDGE_H_

#include <stdint.h>
#include <linux/ioctl.h>

#include <img_types.h>
#include <img_errors.h>

#include "sensors/bridge/bridge_sensors.h"
#include "sensors/bridge/bridge_i2c.h"

#define MAX_STR_LEN (64)
#define MAX_SENSORS_NUM  (2)
#define MAX_BRIDGE_SENSORS_NUM (2)

struct bsensor_uinfo {
    char name[MAX_STR_LEN];
    int i2c_addr;
    int chn;
    int exist;
    int mipi_lanes;
    /* char config[128]; */
};

struct bridge_uinfo {
    char name[MAX_STR_LEN];
    int i2c_addr;
    int chn;
    int imager;
    int mipi_lanes;
    int sensor_num;
    int i2c_exist;
    struct bsensor_uinfo bsinfo[MAX_BRIDGE_SENSORS_NUM];
};

#define XC9080_USER_INIT_MODE 0
#define XC9080_KERNAL_INIT_MODE 1

struct bridge_hw_cfg {
    int mode;
    int enable;
};

#define BRIDGE_CTRL_MAGIC    ('B')

#define BRIDGE_G_INFO    _IOW(BRIDGE_CTRL_MAGIC, 0x31, struct bridge_uinfo)
#define BRIDGE_G_CFG    _IOW(BRIDGE_CTRL_MAGIC, 0x32, unsigned int)
#define BRIDGE_S_CFG    _IOW(BRIDGE_CTRL_MAGIC, 0x33, unsigned int)

typedef struct _sensor_config {
    IMG_UINT32 i2c_addr;
    IMG_UINT32 imager ; /*1 : For DVP Gasket*/
    IMG_UINT32 i2c_data_bits;
    IMG_UINT32 i2c_mode;
    IMG_UINT32 i2c_forbid;
	IMG_UINT32 i2c_addr_bits;
    IMG_UINT32 hsync;
    IMG_UINT32 vsync;

    char config_file[MAX_STR_LEN];
    char sensor_name[MAX_STR_LEN];
    char mosaic_type[16];

    IMG_UINT32 parallel;
    IMG_UINT32 mipi_lanes;

    IMG_UINT32 width;
    IMG_UINT32 height;
    IMG_UINT32 widthTotal;
    IMG_UINT32 heightTotal;
    IMG_UINT32 fps;
    double pclk;
    IMG_UINT8 bits;
}SENSOR_CONFIG;

#define I2C_CHN_SWITCH_TO_BRIDGE 0
#define I2C_CHN_SWITCH_TO_SENSOR 1

struct _bridge_struct;
struct _bridge_dev;

typedef struct _bridge_dev {
    char name[MAX_STR_LEN];
    IMG_RESULT (*init)(struct _bridge_struct*);
    IMG_RESULT (*i2c_switch)(struct _bridge_struct*, unsigned int);
    IMG_RESULT (*set_cfg)(struct _bridge_struct*, unsigned int);
    IMG_RESULT (*get_hw_cfg)(struct _bridge_struct*, unsigned int);
    IMG_RESULT (*set_hw_cfg)(struct _bridge_struct*, unsigned int);
    IMG_RESULT (*exit)(struct _bridge_struct*);
    struct _bridge_dev *next, *prev;
} BRIDGE_DEV;

typedef struct _bridge_struct {
    struct bridge_uinfo info;

    BRIDGE_I2C_CLIENT client;
    BRIDGE_I2C_CLIENT sensor_client;

    SENSOR_CONFIG bridge_cfg;
    SENSOR_CONFIG sensor_cfg;

    BRIDGE_SENSOR* sensor;
    BRIDGE_DEV *dev;
} BRIDGE_STRUCT;

typedef void (*BridgeDevInitFunc)(void);

void bridge_dev_register(BRIDGE_DEV *dev);

int bridge_dev_get_cfg(void* arg);
int bridge_dev_set_cfg(void* arg);

BRIDGE_STRUCT* Bridge_Create(void);
int Bridge_DoSetup(void);
int Bridge_Enable(int enable);
void Bridge_Destory(void);

#endif