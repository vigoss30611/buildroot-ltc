#ifndef __NEPTUNE_I2C_
#define __NEPTUNE_I2C_

#include <unistd.h>
#include <stdbool.h>
#include "i2c-dev.h"

#include "tps65217.h"

static inline __s32 i2c_smbus_read_word_data_bswap(int file, __u8 command)
{
	__s32 data = i2c_smbus_read_word_data(file, command);
	if (data < 0)
		return data;
	return ((data & 0xff) << 8) | (data >> 8);
}

int i2c_connect(int adapter_nr);
void i2c_disconnect(int fd);
bool i2c_set_slave_device_addr(int fd, int addr);
int i2c_insist_read_byte(int bus, __u8 addr, useconds_t delay_us);
int i2c_insist_read_word(int bus, __u8 addr, useconds_t delay_us);

__s32 bitbang_read_byte_data(__u8 dev_addr, __u8 command);
__s32 bitbang_write_byte_data(__u8 dev_addr, __u8 command, __u8 value);

static inline __s32 I2C_read_byte_data(int file, __u8 command)
{
	if (0 > file)
		return bitbang_read_byte_data(TPS65217_BUS_ADDRESS, command);
	return i2c_smbus_read_byte_data(file, command);
}

static inline __s32 I2C_write_byte_data(int file, __u8 command, __u8 value)
{
	if (0 > file)
		return bitbang_write_byte_data(TPS65217_BUS_ADDRESS, command, value);
	return i2c_smbus_write_byte_data(file, command, value);
}

#endif /* __NEPTUNE_I2C_ */
