#ifndef PMBUS_H_
#define PMBUS_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "asic.h"
#include "i2c.h"

static inline double FLOAT_FROM_PMBUS_LINEAR(int v)
{
	int16_t tmp_v = (v);
	int16_t exp = tmp_v >> 11;
	int16_t mant1 = tmp_v << 5;
	int16_t mant = mant1 >> 5;
	double res;
	if (exp < 0)
		res = ((double)mant) / ((double)(1 << (-exp)));
	else
		res = ((double)mant) * ((double)(1 << exp));
	return res;
}

static inline double FLOAT_FROM_PMBUS_VOUT(int mant, int mode)
{
	int8_t exp1 = mode << 3;
	int exp = exp1 >> 3;
	double res;
	if (exp < 0)
		res = ((double)mant) / ((double)(1 << (-exp)));
	else
		res = ((double)mant) * ((double)(1 << exp));
	return res;
}

#define	TEMP_CORRECTED_IOUT(t, i)	(		\
		(i) / (1 + ((t) - 30) * 0.00393)	\
	)

dcdc_mfrid_t pmbus_get_dcdc_mfr_id(int i2c_bus, useconds_t delay_us);

void pmbus_set_vout_trim(int i2c_bus, uint16_t trim, useconds_t delay_us);
__s32 pmbus_get_vout_trim(int i2c_bus, useconds_t delay_us);
__s32 pmbus_get_vout_cmd_ericsson(int i2c_bus);
void pmbus_set_vout_cmd(int i2c_bus, uint16_t vout, useconds_t delay_us);
double pmbus_read_vin(int i2c_bus, useconds_t delay_us);
double pmbus_read_vout(int i2c_bus, useconds_t delay_us);
double pmbus_read_vout_ericsson(int i2c_bus);
double pmbus_read_iout(int i2c_bus, useconds_t delay_us);
double pmbus_read_iout_ericsson(int i2c_bus);
double pmbus_read_temp_ericsson(int i2c_bus);
__s32 pmbus_revision(int i2c_bus, useconds_t delay_us);
__s32 pmbus_mfr_specific_00(int i2c_bus, useconds_t delay_us);
void pmbus_on(int i2c_bus, useconds_t delay_us);
void pmbus_off(int i2c_bus, useconds_t delay_us);
void pmbus_clear_faults(int i2c_bus, useconds_t delay_us);
void pmbus_ignore_faults(int i2c_bus, useconds_t delay_us);

#define PMBUS_ON_OFF_POWER_ON 		( 1 << 4 )
#define PMBUS_ON_OFF_REQUIRE_ON_OFF 	( 1 << 3 )
#define PMBUS_ON_OFF_REQUIRE_CONTROL 	( 1 << 2 )
#define PMBUS_ON_OFF_CONTROL_POLARITY 	( 1 << 1 )
#define PMBUS_ON_OFF_FAST_TURN_OFF 	( 1 << 0 )
#define PMBUS_ON_OFF_SHUTDOWN 		0x1F
void pmbus_on_off_config(int i2c_bus, uint8_t data, useconds_t delay_us);

void pmbus_store_default_all(int i2c_bus, useconds_t delay_us);

#define PMBUS_STATUS_BUSY 		(1 << 7)
#define PMBUS_STATUS_OFF 		(1 << 6)
#define PMBUS_STATUS_VOUT_OV 		(1 << 5)
#define PMBUS_STATUS_IOUT_OC 		(1 << 4)
#define PMBUS_STATUS_VIN_UV 		(1 << 3)
#define PMBUS_STATUS_TEMPERATUREB 	(1 << 2)
#define PMBUS_STATUS_CMLB 		(1 << 1)
#define PMBUS_STATUS_NONE 		(1 << 0)
uint8_t pmbus_status_byte(int i2c_bus, useconds_t delay_us);

bool test_ericsson(int i2c_bus, int dev_num, float vin_low_level, float vin_high_level);

#endif /* PMBUS_H_ */
