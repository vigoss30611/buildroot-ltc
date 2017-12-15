#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>

#include "i2c.h"

#ifdef _DEBUG
#	include <stdio.h>
#	define DEBUG(...) fprintf(stderr,"[DEBUG] --- " __VA_ARGS__)
#else
#	define DEBUG(...)
#endif

int i2c_connect(int adapter_nr)
{
	int fd;
	char filename[20];

	snprintf(filename, 19, "/dev/i2c-%d", adapter_nr);
	fd = open(filename, O_RDWR);
	DEBUG("Connected %s on fd %d\n",filename, fd);
	if(fd < 0)
		fprintf(stderr, "Error opening %s: %m\n", filename);
	return fd;
}

void i2c_disconnect(int fd)
{
	DEBUG("Disconnecting fd %d\n", fd);
	close(fd);
}

bool i2c_set_slave_device_addr(int fd, int addr)
{
	if (ioctl(fd, I2C_SLAVE, addr) < 0) {
		fprintf(stderr, "Error setting slave device address 0x%x: %m\n",addr);
		return false;
	}	
	return true;
}

int i2c_insist_read_byte(int bus, __u8 addr, useconds_t delay_us)
{
	int data;
	usleep(delay_us);
	data = i2c_smbus_read_byte_data(bus, addr);
	if ((0 > data) || (0xFF == data)) {
		usleep(delay_us);
		data = i2c_smbus_read_byte_data(bus, addr);
		if ((0 > data) || (0xFF == data)) {
			usleep(delay_us);
			data = i2c_smbus_read_byte_data(bus, addr);
			if ((0 > data) || (0xFF == data)) {
				fprintf(stderr, "i2c_smbus_read_byte_data failed: addr 0x%02X\n", addr);
				return -1;
			}
		}
	}
	return data;
}

int i2c_insist_read_word(int bus, __u8 addr, useconds_t delay_us)
{
	int data;
	usleep(delay_us);
	data = i2c_smbus_read_word_data(bus, addr);
	if ((0 > data) || (0xFFFF == data)) {
		usleep(delay_us);
		data = i2c_smbus_read_word_data(bus, addr);
		if ((0 > data) || (0xFFFF == data)) {
			usleep(delay_us);
			data = i2c_smbus_read_word_data(bus, addr);
			if ((0 > data) || (0xFFFF == data)) {
				fprintf(stderr, "i2c_smbus_read_word_data failed: addr 0x%02X\n", addr);
				return -1;
			}
		}
	}
	return data;
}

/***************************************************************
 * User-space bitbang I2C implementation                       *
 ***************************************************************/

#define	I2C_CLK_FILE	"/sys/class/gpio/gpio25/direction"
#define	I2C_SDA_FILE	"/sys/class/gpio/gpio16/direction"
#define	I2C_SDA_READ_FILE "/sys/class/gpio/gpio16/value"

static bool get_signal(const char *fname,  int *value)
{
	char buf[16];
	int cnt, fd = open(fname, O_RDONLY);
	if (0 > fd) {
		fprintf(stderr, "Can not open file %s:%m\n", fname);
		return false;
	}

	cnt = read(fd, buf, sizeof(buf) - 1);
	if (0 >= cnt) {
		fprintf(stderr, "Can not read file %s\n", fname);
		close(fd);
		return false;
	}
	close(fd);

	switch (buf[0]) {
		case '1':
			*value = 1;
			break;
		case '0':
			*value = 0;
			break;
		default:
			buf[cnt] = 0;
			fprintf(stderr, "Wrong value (%s) in file %s\n",
				buf, fname);
			return false;
	}

	return true;
}

static bool set_signal(const char *fname, int value)
{
	char buf[16];
	int len, cnt, fd = open(fname, O_WRONLY);
	if (0 > fd) {
		fprintf(stderr, "Can not open file %s:%m\n", fname);
		return false;
	}

	if (value > 0) {
		strcpy(buf, "high");
		len = 4;
	} else if (value == 0) {
		strcpy(buf, "low");
		len = 3;
	} else {
		strcpy(buf, "in");
		len = 2;
	}

	cnt = write(fd, buf, len);
	if (len != cnt) {
		fprintf(stderr, "Can not write file %s\n", fname);
		close(fd);
		return false;
	}
	close(fd);

	return true;
}

static inline void i2c_clk(int value)
{
	set_signal(I2C_CLK_FILE, value);
}

static inline void i2c_data(int value)
{
	set_signal(I2C_SDA_FILE, value);
}

static inline void i2c_data_dir_in(void)
{
	set_signal(I2C_SDA_FILE, -1);
}

static inline int i2c_getbit(void)
{
	int value;
	if (!get_signal(I2C_SDA_READ_FILE, &value))
		return 1;
	return value;
}

static uint8_t i2c_inbyte(bool ack)
{
	uint8_t	value = 0;
	int bitvalue;
	int i;

	i2c_clk(0);
	i2c_data_dir_in();

	for (i = 0; i < 8; ++i)	{
		i2c_clk(1);
		bitvalue = i2c_getbit();
		value |= bitvalue;
		if (i < 7)
			value <<= 1;
		i2c_clk(0);
	}

	if (ack)
		i2c_data(0);

	i2c_clk(1);
	i2c_clk(0);

	i2c_data_dir_in();
	return value;
}

static void i2c_start(void)
{
	i2c_clk(0);
	i2c_data(1);
	i2c_clk(1);
	i2c_data(0);
}

static void i2c_stop(void)
{
	i2c_clk(0);
	i2c_data(0);
	i2c_clk(1);
	i2c_data(1);
}

static bool i2c_outbyte(uint8_t x)
{
	int i;
	int ack;

	i2c_clk(0);

	for (i = 0; i < 8; ++i) {
		if (x & 0x80)
			i2c_data(1);
		else
			i2c_data(0);
		i2c_clk(1);
		i2c_clk(0);
		x <<= 1;
	}

	i2c_data_dir_in();
	i2c_clk(1);
	ack = i2c_getbit();
	i2c_clk(0);

	return !ack;
}


__s32 bitbang_read_byte_data(__u8 dev_addr, __u8 command)
{
	uint8_t ret;

	i2c_start();
	/* Device address & Write */
	if (!i2c_outbyte(dev_addr << 1))
		return -1;
	/* Command */
	if (!i2c_outbyte(command))
		return -1;
	/* Repeated Start */
	i2c_start();
	/* Device address & Read */
	if (!i2c_outbyte((dev_addr << 1) | 1))
		return -1;
	ret = i2c_inbyte(false);
	i2c_stop();

	return ret;
}

__s32 bitbang_write_byte_data(__u8 dev_addr, __u8 command, __u8 value)
{
	i2c_start();
	/* Device address & Write */
	if (!i2c_outbyte(dev_addr << 1))
		return -1;
	/* Command */
	if (!i2c_outbyte(command))
		return -1;
	/* Value */
	if (!i2c_outbyte(value))
		return -1;
	i2c_stop();

	return value;
}
