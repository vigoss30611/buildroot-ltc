#ifndef __BRIDGE_I2C_H
#define __BRIDGE_I2C_H

#include <img_types.h>
#include <img_errors.h>

typedef struct bridge_i2c_client {
    int i2c;
    int i2c_addr;
    int i2c_data_bits;
	int i2c_addr_bits;
    int chn;
    int mode;
    int exist;
}BRIDGE_I2C_CLIENT;

IMG_RESULT bridge_i2c_write(BRIDGE_I2C_CLIENT* client, IMG_UINT16 offset,
    IMG_UINT16 data);

IMG_RESULT bridge_i2c_read(BRIDGE_I2C_CLIENT* client, IMG_UINT16 offset,
    IMG_UINT16* data);

IMG_RESULT bridge_i2c_sync(BRIDGE_I2C_CLIENT* client, IMG_UINT16 offset,
    IMG_UINT16 data);

IMG_RESULT bridge_i2c_client_copy(BRIDGE_I2C_CLIENT* dst,
    int new_addr, BRIDGE_I2C_CLIENT* src);

#endif