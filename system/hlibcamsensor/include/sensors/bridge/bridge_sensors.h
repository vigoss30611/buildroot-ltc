#ifndef BRIDGE_SENSORS_H
#define BRIDGE_SENSORS_H

#include "img_types.h"
#include <img_errors.h>
#include "sensorapi/sensorapi.h"
#include "sensors/bridge/bridge_i2c.h"

typedef struct _bridge_sensor
{
    char name[64];
    IMG_RESULT (*set_def_mode)(BRIDGE_I2C_CLIENT* i2c, SENSOR_MODE *info);
    IMG_RESULT (*set_exposure)(BRIDGE_I2C_CLIENT* i2c, IMG_UINT32 exp, SENSOR_MODE *info);
    IMG_RESULT (*set_gain)(BRIDGE_I2C_CLIENT* i2c,double gain, SENSOR_MODE *info);
    IMG_RESULT (*get_exposure)(BRIDGE_I2C_CLIENT* i2c,IMG_UINT32* exp, SENSOR_MODE *info);
    IMG_RESULT (*get_gain)(BRIDGE_I2C_CLIENT* i2c,double* gain, SENSOR_MODE *info);
    IMG_RESULT (*get_gain_range)(BRIDGE_I2C_CLIENT* i2c, double *min,double *max, SENSOR_MODE *info);
    IMG_RESULT (*get_exposure_range)(BRIDGE_I2C_CLIENT* i2c, IMG_UINT32 *min, IMG_UINT32 *max, SENSOR_MODE *info);
    IMG_RESULT (*set_exposure_gain)(BRIDGE_I2C_CLIENT* i2c,IMG_UINT32 exp, double gain, SENSOR_MODE *info);
    IMG_RESULT (*check_id)(BRIDGE_I2C_CLIENT* i2c);
    IMG_RESULT (*enable)(BRIDGE_I2C_CLIENT* i2c,int on);

    IMG_RESULT (*get_cur_focus)(BRIDGE_I2C_CLIENT* i2c,IMG_UINT32* focus, SENSOR_MODE *info);
    IMG_RESULT (*get_focus_range)(BRIDGE_I2C_CLIENT* i2c, IMG_UINT32 *min, IMG_UINT32 *max, SENSOR_MODE *info);
    IMG_RESULT (*set_focus)(BRIDGE_I2C_CLIENT* i2c, IMG_UINT32 focus, SENSOR_MODE *info);

    IMG_RESULT (*set_fps)(BRIDGE_I2C_CLIENT* i2c, double fps, SENSOR_MODE *info);
    IMG_RESULT (*get_fixedfps)(BRIDGE_I2C_CLIENT* i2c, double* fps, SENSOR_MODE *info);
    IMG_RESULT (*reset)(BRIDGE_I2C_CLIENT* i2c, SENSOR_MODE *info);

    IMG_UINT8* (*read_calib_data)(BRIDGE_I2C_CLIENT *i2c, int product, int idx, IMG_FLOAT gain, IMG_UINT16 *version, SENSOR_MODE *info);
    IMG_UINT8* (*read_calib_version)(BRIDGE_I2C_CLIENT *i2c, int product, int idx, IMG_UINT16 *version, SENSOR_MODE *info);
    IMG_RESULT (*update_wb_gain)(BRIDGE_I2C_CLIENT *i2c, int product, IMG_FLOAT gain, IMG_UINT16 version);
    IMG_RESULT (*exchange_calib_direction)(SENSOR_HANDLE hHandle, int flag);
    struct _bridge_sensor *next, *prev;

} BRIDGE_SENSOR;

typedef void (*BridgeSensorsInitFunc)(void);

IMG_RESULT bridge_i2c_sync(BRIDGE_I2C_CLIENT* i2c, IMG_UINT16 offset,IMG_UINT16 data);
IMG_RESULT bridge_i2c_write(BRIDGE_I2C_CLIENT* i2c, IMG_UINT16 offset, IMG_UINT16 data);
IMG_RESULT bridge_i2c_read(BRIDGE_I2C_CLIENT* i2c, IMG_UINT16 offset, IMG_UINT16* data);

void     bridge_sensor_register(BRIDGE_SENSOR *sensor);
BRIDGE_SENSOR* bridge_sensor_get_instance(const char* sensor_name);

#endif