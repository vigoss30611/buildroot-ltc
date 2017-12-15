#include <stdio.h>
#include <sys/time.h>
#include <sched.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>

#include "i2c.h"
#include "tps65217.h"

#ifdef CONTROLLER_BOARD_BBB
/* BeagleBone Black */
#define	PWR_EN_FILE		"/sys/class/gpio/gpio49/direction"
#define	DCDC_RESET_FILE		"/sys/class/gpio/gpio76/direction"
#endif

#ifdef CONTROLLER_BOARD_RPI
/* Raspberry Pi */
#define	PWR_EN_FILE		"/sys/class/gpio/gpio24/direction"
#define	DCDC_RESET_FILE		"/sys/class/gpio/gpio23/direction"
#endif

static bool reset_dcdc(int i2c_bus, bool on);

static bool test_tps65217_once(int i2c_bus)
{
	int id, rev, status;
	char modification;
	
	id = I2C_read_byte_data(i2c_bus, TPS65217_CHIPID);
	switch (id >> 4) {
		case 0x7:
			modification = 'A';
			break;
		case 0xF:
			modification = 'B';
			break;
		case 0xE:
			modification = 'C';
			break;
		case 0x6:
			modification = 'D';
			break;
		default:
			fprintf(stderr, "TPS65217 error: Bad ID 0x%02X\n", id);
			return false;
	}
	rev = id & 0x0F;

	status = I2C_read_byte_data(i2c_bus, TPS65217_STATUS);
	if ( (!(status & TPS65217_STATUS_BIT_ACPWR)) ||
	     (status & TPS65217_STATUS_BIT_USBPWR) ||
	     (status & TPS65217_STATUS_BIT_OFF)
	   ) {	
		fprintf(stderr, "TPS65217 error: Bad status 0x%02X\n", status);
		return false;
	}

	printf("TPS65217 OK. Modification %c, revision 1.%d\n",
	       modification, rev);
	return true;
}

bool test_tps65217(int i2c_bus)
{
	if (test_tps65217_once(i2c_bus))
		return true;
	/* Maybe we are in a strange state, where I2C is locked? */
	fprintf(stderr, "Trying to reset TPS65217...\n");
	reset_dcdc(i2c_bus, true);
	usleep(100000);
	return test_tps65217_once(i2c_bus);
}

static void tps65217_write_level1_reg(int i2c_bus, uint8_t reg, uint8_t value)
{
	I2C_write_byte_data(i2c_bus, TPS65217_PASSWORD, TPS65217_PASSWORD_VALUE ^ reg);
	I2C_write_byte_data(i2c_bus, reg, value);
}

static void tps65217_write_level2_reg(int i2c_bus, uint8_t reg, uint8_t value)
{
	I2C_write_byte_data(i2c_bus, TPS65217_PASSWORD, TPS65217_PASSWORD_VALUE ^ reg);
	I2C_write_byte_data(i2c_bus, reg, value);
	I2C_write_byte_data(i2c_bus, TPS65217_PASSWORD, TPS65217_PASSWORD_VALUE ^ reg);
	I2C_write_byte_data(i2c_bus, reg, value);
}

static bool reset_dcdc(int i2c_bus, bool on)
{
	int fd, exp_cnt, cnt;

	// Hack to get rid of compiler error due to unused variable.
	cnt = i2c_bus;

	if (0 > (fd = open(DCDC_RESET_FILE, O_RDWR))) {
		fprintf(stderr,
			"Can not open %s: %m\n", DCDC_RESET_FILE);
		return false;		
	}
	
	/* Enable power output */
	if (on) {
		exp_cnt = 3;
		cnt = write(fd, "low", exp_cnt);
	} else {
		exp_cnt = 4;
		cnt = write(fd, "high", exp_cnt);
	};
	
	close(fd);
	return (cnt == exp_cnt);
}

static bool pwren_dcdc(int i2c_bus, bool on)
{
	int fd, exp_cnt, cnt;

	// Hack to get rid of compiler error due to unused variable.
	cnt = i2c_bus;

	if (0 > (fd = open(PWR_EN_FILE, O_RDWR))) {
		fprintf(stderr,
			"Can not open %s: %m\n", PWR_EN_FILE);
		return false;		
	}
	
	/* Enable power output */
	if (on) {
		exp_cnt = 4;
		cnt = write(fd, "high", exp_cnt);
	} else {
		exp_cnt = 3;
		cnt = write(fd, "low", exp_cnt);
	};
	
	close(fd);
	return (cnt == exp_cnt);
}

bool configure_tps65217(int i2c_bus)
{
	struct timeval start, end, runtime;
	int prev_policy;
	struct sched_param prev_params, params;
	int cnt = 0;
	int data;
	int fd;

	power_down_spi_connector(i2c_bus);
	usleep(100000);
	pwren_dcdc(i2c_bus, false);
	usleep(500000);

	/* Synchronize to the beginning of 6-second reset cycle */
	reset_dcdc(i2c_bus, true);
	sleep(1);
	reset_dcdc(i2c_bus, false);
	sleep(1);

 	/* Synchronize to the beginning of 6-second reset cycle */
	I2C_write_byte_data(i2c_bus, TPS65217_MUXCTRL, 0x07);
	/* Wait for the register to reset, but no more than 6 seconds */
 	gettimeofday(&start, NULL);
	while (0x07 == I2C_read_byte_data(i2c_bus, TPS65217_MUXCTRL)) {
		gettimeofday(&start, NULL);
		if (0 == usleep(100000)) /* if not interrupted by signal */
			++cnt;
		if (cnt > 60)
			break;
	}
	/* Reset cycle boundary found. Check that it is not a mere communication
	 * error.
	 */
	I2C_write_byte_data(i2c_bus, TPS65217_MUXCTRL, 0x05);
	if (0x05 != I2C_read_byte_data(i2c_bus, TPS65217_MUXCTRL))
		return false;

	/* Configure the device */
	tps65217_write_level2_reg(i2c_bus, TPS65217_DEFDCDC1,
				  DEFAULT_SPI_VOLTAGE);
	tps65217_write_level2_reg(i2c_bus, TPS65217_DEFDCDC2,
				  TPS65217_DEFDCDC_VALUE_3_3V);
	tps65217_write_level2_reg(i2c_bus, TPS65217_DEFDCDC3,
				  TPS65217_DEFDCDC_VALUE_1_2V);
	tps65217_write_level2_reg(i2c_bus, TPS65217_DEFSLEW,
				  TPS65217_DEFSLEW_GO_FAST);
	tps65217_write_level2_reg(i2c_bus, TPS65217_DEFLDO1,
				  TPS65217_DEFLDO1_VALUE_2_5V);
	tps65217_write_level2_reg(i2c_bus, TPS65217_DEFLDO2,
				  TPS65217_DEFLDO2_VALUE_1_2V);
	tps65217_write_level2_reg(i2c_bus, TPS65217_DEFLDO3,
				  TPS65217_DEFLDO3_VALUE_LS_3_3V);
#ifdef NOT_REAL_ASIC_BUT_FPGA
	tps65217_write_level2_reg(i2c_bus, TPS65217_DEFLDO4,
				  TPS65217_DEFLDO4_VALUE_LDO_2_5V);
#else /* real ASIC board */
	tps65217_write_level2_reg(i2c_bus, TPS65217_DEFLDO4,
				  TPS65217_DEFLDO4_VALUE_LDO_1_8V);
#endif
	tps65217_write_level1_reg(i2c_bus, TPS65217_ENABLE, TPS65217_ENABLE_VALUE);

	/* Check the configuration */
	data = I2C_read_byte_data(i2c_bus, TPS65217_DEFDCDC1);
	if ((data ^ DEFAULT_SPI_VOLTAGE) & 0xBF) {
		fprintf(stderr, "Wrong DEFDCDC1 value 0x%02X\n", data);
		return false;		
	}

	data = I2C_read_byte_data(i2c_bus, TPS65217_DEFDCDC2);
	if ((data ^ TPS65217_DEFDCDC_VALUE_3_3V) & 0xBF) {
		fprintf(stderr, "Wrong DEFDCDC1 value 0x%02X\n", data);
		return false;		
	}

	data = I2C_read_byte_data(i2c_bus, TPS65217_DEFDCDC3);
	if ((data ^ TPS65217_DEFDCDC_VALUE_1_2V) & 0xBF) {
		fprintf(stderr, "Wrong DEFDCDC3 value 0x%02X\n", data);
		return false;		
	}

	data = I2C_read_byte_data(i2c_bus, TPS65217_DEFSLEW);
	if ((data ^ TPS65217_DEFSLEW_GO_FAST) & 0x7F) {
		fprintf(stderr, "Wrong DEFSLEW value 0x%02X\n", data);
		return false;		
	}

	data = I2C_read_byte_data(i2c_bus, TPS65217_DEFLDO1);
	if ((data ^ TPS65217_DEFLDO1_VALUE_2_5V) & 0x0F) {
		fprintf(stderr, "Wrong DEFLDO1 value 0x%02X\n", data);
		return false;		
	}

	data = I2C_read_byte_data(i2c_bus, TPS65217_DEFLDO2);
	if ((data ^ TPS65217_DEFLDO2_VALUE_1_2V) & 0x7F) {
		fprintf(stderr, "Wrong DEFLDO2 value 0x%02X\n", data);
		return false;		
	}

	data = I2C_read_byte_data(i2c_bus, TPS65217_DEFLDO3);
	if ((data ^ TPS65217_DEFLDO3_VALUE_LS_3_3V) & 0x3F) {
		fprintf(stderr, "Wrong DEFLDO3 value 0x%02X\n", data);
		return false;		
	}

	data = I2C_read_byte_data(i2c_bus, TPS65217_DEFLDO4);
#ifdef NOT_REAL_ASIC_BUT_FPGA
	if ((data ^ TPS65217_DEFLDO4_VALUE_LDO_2_5V) & 0x3F) {
#else /* real ASIC board */
	if ((data ^ TPS65217_DEFLDO4_VALUE_LDO_1_8V) & 0x3F) {
#endif
		fprintf(stderr, "Wrong DEFLDO4 value 0x%02X\n", data);
		return false;		
	}

	/* The default power-up sequence is not suitable for us,
	 * so we'll enable SPI power rails later, manually */
	if (!power_down_spi_connector(i2c_bus))
		return false;

	if (0 > (fd = open(PWR_EN_FILE, O_RDWR))) {
		fprintf(stderr,
			"Can not open %s: %m\n", PWR_EN_FILE);
		return false;		
	}

	/* Check that we stay within 6-second cycle. To be on the safe side,
	 * reduce time window to some value between 1 and 2 seconds.
	 */
	prev_policy = sched_getscheduler(0);
	sched_getparam(0, &prev_params);
	params = prev_params;

	params.sched_priority = sched_get_priority_max(SCHED_FIFO);
	sched_setscheduler(0, SCHED_FIFO, &params);
	
	gettimeofday(&end, NULL);
	timersub(&end, &start, &runtime);
	if (runtime.tv_sec >= 2) {
		sched_setscheduler(0, prev_policy, &prev_params);
		close(fd);
		return false;
	}
	
	/* Enable power output */
	cnt = write(fd, "high", 4);

	sched_setscheduler(0, prev_policy, &prev_params);
	
	close(fd);
	if (4 != cnt) {
		fprintf(stderr,
			"Write failed to %s\n", PWR_EN_FILE);
		return false;	
	}

	if (!power_up_spi_connector(i2c_bus))
		return false;
	
	/* Wait for the POWER GOOD indication */
	cnt = 0;
	while (0x7F != (0x7F & I2C_read_byte_data(i2c_bus, TPS65217_PGOOD))) {
		if (0 == usleep(1000)) /* if not interrupted by signal */
			++cnt;
		if (cnt > 100) {
			fprintf(stderr, "Waiting PGOOD for too long\n");
			return false;	
		}
	}

	return true;
}

bool power_down_spi_connector(int i2c_bus)
{
	int data, read_data;

	/* Disable LDO4 = 1V8A */
	read_data = I2C_read_byte_data(i2c_bus, TPS65217_ENABLE);
	if (0 > read_data) {
		fprintf(stderr, "Can not read ENABLE register\n");
		return false;		
	}

	data = read_data & (~TPS65217_ENABLE_LDO4);
	tps65217_write_level1_reg(i2c_bus, TPS65217_ENABLE, data);
	
	read_data = I2C_read_byte_data(i2c_bus, TPS65217_ENABLE);
	if ((data ^ read_data) & 0x7F) {
		fprintf(stderr, "Wrong ENABLE value 0x%02X\n", read_data);
		return false;		
	}

	/* Disable LDO4 in Sequencer */
	read_data = I2C_read_byte_data(i2c_bus, TPS65217_SEQ4);
	if (0 > read_data) {
		fprintf(stderr, "Can not read SEQ4 register\n");
		return false;		
	}

	data = read_data & (~(TPS65217_SEQ4_LDO4_MASK));
	tps65217_write_level1_reg(i2c_bus, TPS65217_SEQ4, data);
	
	read_data = I2C_read_byte_data(i2c_bus, TPS65217_SEQ4);
	if ((data ^ read_data) & TPS65217_SEQ4_MASK) {
		fprintf(stderr, "Wrong SEQ4 value 0x%02X\n", read_data);
		return false;		
	}

	usleep(100000);

	/* Disable DCDC1 = 1V8 */
	read_data = I2C_read_byte_data(i2c_bus, TPS65217_ENABLE);
	if (0 > read_data) {
		fprintf(stderr, "Can not read ENABLE register\n");
		return false;		
	}

	data = read_data & (~(TPS65217_ENABLE_LDO4 | TPS65217_ENABLE_DCDC1));
	tps65217_write_level1_reg(i2c_bus, TPS65217_ENABLE, data);
	
	read_data = I2C_read_byte_data(i2c_bus, TPS65217_ENABLE);
	if ((data ^ read_data) & 0x7F) {
		fprintf(stderr, "Wrong ENABLE value 0x%02X\n", read_data);
		return false;		
	}

	/* Disable DCDC1 in Sequencer */
	read_data = I2C_read_byte_data(i2c_bus, TPS65217_SEQ1);
	if (0 > read_data) {
		fprintf(stderr, "Can not read SEQ1 register\n");
		return false;		
	}

	data = read_data & (~(TPS65217_SEQ1_DCDC1_MASK));
	tps65217_write_level1_reg(i2c_bus, TPS65217_SEQ1, data);
	
	read_data = I2C_read_byte_data(i2c_bus, TPS65217_SEQ1);
	if ((data ^ read_data) & TPS65217_SEQ1_MASK) {
		fprintf(stderr, "Wrong SEQ1 value 0x%02X\n", read_data);
		return false;		
	}

	usleep(100000);

	/* Disable LDO3 = 3V3 */
	read_data = I2C_read_byte_data(i2c_bus, TPS65217_ENABLE);
	if (0 > read_data) {
		fprintf(stderr, "Can not read ENABLE register\n");
		return false;		
	}

	data = read_data & (~(TPS65217_ENABLE_LDO4 | TPS65217_ENABLE_LDO3 |
			      TPS65217_ENABLE_DCDC1));
	tps65217_write_level1_reg(i2c_bus, TPS65217_ENABLE, data);
	
	read_data = I2C_read_byte_data(i2c_bus, TPS65217_ENABLE);
	if ((data ^ read_data) & 0x7F) {
		fprintf(stderr, "Wrong ENABLE value 0x%02X\n", read_data);
		return false;		
	}

	/* Disable LDO3 in Sequencer */
	read_data = I2C_read_byte_data(i2c_bus, TPS65217_SEQ3);
	if (0 > read_data) {
		fprintf(stderr, "Can not read SEQ3 register\n");
		return false;		
	}

	data = read_data & (~(TPS65217_SEQ3_LDO3_MASK));
	tps65217_write_level1_reg(i2c_bus, TPS65217_SEQ3, data);
	
	read_data = I2C_read_byte_data(i2c_bus, TPS65217_SEQ3);
	if ((data ^ read_data) & TPS65217_SEQ3_MASK) {
		fprintf(stderr, "Wrong SEQ3 value 0x%02X\n", read_data);
		return false;		
	}

	return true;
}

bool power_up_spi_connector(int i2c_bus)
{
	int data, read_data;

	/* Enable LDO3 = 3V3 */
	read_data = I2C_read_byte_data(i2c_bus, TPS65217_ENABLE);
	if (0 > read_data) {
		fprintf(stderr, "Can not read ENABLE register\n");
		return false;		
	}

	data = read_data | TPS65217_ENABLE_LDO3;
	tps65217_write_level1_reg(i2c_bus, TPS65217_ENABLE, data);
	
	read_data = I2C_read_byte_data(i2c_bus, TPS65217_ENABLE);
	if ((data ^ read_data) & 0x7F) {
		fprintf(stderr, "Wrong ENABLE value 0x%02X\n", read_data);
		return false;		
	}

	usleep(100000);

	/* Enable DCDC1 = 1V8 */
	read_data = I2C_read_byte_data(i2c_bus, TPS65217_ENABLE);
	if (0 > read_data) {
		fprintf(stderr, "Can not read ENABLE register\n");
		return false;		
	}

	data = read_data | (TPS65217_ENABLE_DCDC1 | TPS65217_ENABLE_LDO3);
	tps65217_write_level1_reg(i2c_bus, TPS65217_ENABLE, data);
	
	read_data = I2C_read_byte_data(i2c_bus, TPS65217_ENABLE);
	if ((data ^ read_data) & 0x7F) {
		fprintf(stderr, "Wrong ENABLE value 0x%02X\n", read_data);
		return false;		
	}

	usleep(100000);

	/* Enable LDO4 = 1V8A */
	read_data = I2C_read_byte_data(i2c_bus, TPS65217_ENABLE);
	if (0 > read_data) {
		fprintf(stderr, "Can not read ENABLE register\n");
		return false;		
	}

	data = read_data | (TPS65217_ENABLE_DCDC1 | TPS65217_ENABLE_LDO3 |
			    TPS65217_ENABLE_LDO4);
	tps65217_write_level1_reg(i2c_bus, TPS65217_ENABLE, data);
	
	read_data = I2C_read_byte_data(i2c_bus, TPS65217_ENABLE);
	if ((data ^ read_data) & 0x7F) {
		fprintf(stderr, "Wrong ENABLE value 0x%02X\n", read_data);
		return false;		
	}

	usleep(100000);

	return true;
}

int tps65217_get_digital_SPI_voltage(int i2c_bus)
{
	return I2C_read_byte_data(i2c_bus, TPS65217_DEFDCDC1);
}

bool tps65217_set_digital_SPI_voltage(int i2c_bus, int voltage)
{
	int data;
	FILE *f;

	data = I2C_read_byte_data(i2c_bus, TPS65217_DEFDCDC1);
	if (!((data ^ voltage) & 0xBF))
		return true;

	if (TPS65217_DEFDCDC_VALUE_1_95V == voltage) {
		if (NULL != (f = fopen("/config/.wa3.3v.down", "w"))) {
			fprintf(f, "0\n");
			fclose(f);
		}
	}

	tps65217_write_level2_reg(i2c_bus, TPS65217_DEFDCDC1, voltage);
	tps65217_write_level2_reg(i2c_bus, TPS65217_DEFSLEW,
				  TPS65217_DEFSLEW_GO_FAST);

	data = I2C_read_byte_data(i2c_bus, TPS65217_DEFDCDC1);
	if ((data ^ voltage) & 0xBF) {
		fprintf(stderr, "Wrong DEFDCDC1 value 0x%02X\n", data);
		return false;		
	}

	data = I2C_read_byte_data(i2c_bus, TPS65217_DEFSLEW);
	if ((data ^ TPS65217_DEFSLEW_GO_FAST) & 0x7F) {
		fprintf(stderr, "Wrong DEFSLEW value 0x%02X\n", data);
		return false;		
	}

	return true;
}

uint8_t tps65217_nearest_valid_spi_voltage(float user_voltage)
{
	if (user_voltage < 1.725)
		return 0x1C;
	if (user_voltage < 1.775)
		return 0x1D;
	if (user_voltage < 1.825)
		return 0x1E;
	if (user_voltage < 1.875)
		return 0x1F;
	if (user_voltage < 1.925)
		return 0x20;
	if (user_voltage < 1.975)
		return 0x21;
	if (user_voltage < 2.025)
		return 0x22;
	if (user_voltage < 2.075)
		return 0x23;
	if (user_voltage < 2.125)
		return 0x24;
	if (user_voltage < 2.175)
		return 0x25;
	if (user_voltage < 2.225)
		return 0x26;
	if (user_voltage < 2.275)
		return 0x27;
	if (user_voltage < 2.325)
		return 0x28;
	if (user_voltage < 2.375)
		return 0x29;
	if (user_voltage < 2.425)
		return 0x2A;
	if (user_voltage < 2.475)
		return 0x2B;
	if (user_voltage < 2.525)
		return 0x2C;
	if (user_voltage < 2.575)
		return 0x2D;
	if (user_voltage < 2.625)
		return 0x2E;
	if (user_voltage < 2.675)
		return 0x2F;
	if (user_voltage < 2.725)
		return 0x30;
	if (user_voltage < 2.775)
		return 0x31;
	if (user_voltage < 2.825)
		return 0x32;
	if (user_voltage < 2.875)
		return 0x33;
	if (user_voltage < 2.95)
		return 0x34;
	if (user_voltage < 3.05)
		return 0x35;
	if (user_voltage < 3.15)
		return 0x36;
	if (user_voltage < 3.25)
		return 0x37;
	return 0x38;
}
