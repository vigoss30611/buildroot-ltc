#include "pmbus.h"
#include "i2c.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define PMBUS_OPERATION			0x01
#define PMBUS_ON_OFF_CONFIG		0x02
#define PMBUS_CLEAR_FAULTS		0x03
#define PMBUS_WRITE_PROTECT		0x10
#define PMBUS_STORE_DEFAULT_ALL		0x11
#define PMBUS_RESTORE_DEFAULT_ALL	0x12
#define PMBUS_STORE_DEFAULT_CODE	0x13
#define PMBUS_RESTORE_DEFAULT_CODE	0x14
#define PMBUS_STORE_USER_ALL		0x15
#define PMBUS_RESTORE_USER_ALL		0x16
#define PMBUS_VOUT_MODE			0x20
#define PMBUS_VOUT_COMMAND		0x21
#define PMBUS_VOUT_TRIM			0x22
#define PMBUS_VOUT_MARGIN_HIGH		0x25
#define PMBUS_VOUT_MARGIN_LOW		0x26
#define PMBUS_VOUT_SCALE_LOOP		0x29
#define PMBUS_VIN_ON			0x35
#define PMBUS_VIN_OFF			0x36
#define PMBUS_IOUT_CAL_GAIN		0x38
#define PMBUS_IOUT_CAL_OFFSET		0x39
#define PMBUS_VOUT_OV_FAULT_LIMIT	0x40
#define PMBUS_VOUT_OV_FAULT_RESPONSE	0x41
#define PMBUS_VOUT_UV_FAULT_LIMIT	0x44
#define PMBUS_VOUT_UV_FAULT_RESPONSE	0x45
#define PMBUS_IOUT_OC_FAULT_LIMIT	0x46
#define PMBUS_IOUT_OC_WARN_LIMIT	0x4a
#define PMBUS_POWER_GOOD_ON		0x5e
#define PMBUS_POWER_GOOD_OFF		0x5f
#define PMBUS_TON_RISE			0x61
#define PMBUS_STATUS_BYTE		0x78
#define PMBUS_STATUS_WORD		0x79
#define PMBUS_STATUS_VOUT		0x7a
#define PMBUS_STATUS_IOUT		0x7b
#define PMBUS_STATUS_TEMPERATURE	0x7d
#define PMBUS_STATUS_CML		0x7e
#define PMBUS_READ_VIN			0x88
#define PMBUS_READ_VOUT			0x8b
#define PMBUS_READ_IOUT			0x8c
#define PMBUS_READ_TEMPERATURE_1	0x8d
#define PMBUS_REVISION			0x98
#define PMBUS_MFR_ID			0x99
#define PMBUS_MFR_MODEL			0x9a
#define PMBUS_MFR_REVISION		0x9b
#define PMBUS_MFR_VIN_MIN		0xa0
#define PMBUS_MFR_VOUT_MIN		0xa4
#define PMBUS_MFR_SPECIFIC_00		0xd0
#define PMBUS_VOUT_CAL_OFFSET		0xd4
#define PMBUS_VOUT_CAL_GAIN		0xd5
#define PMBUS_VIN_CAL_OFFSET		0xd6
#define PMBUS_VIN_CAL_GAIN		0xd7

/* MegaDlynx specific */
#define	MEGADLYNX_PMBUS_REVISION_VALUE	0x11
#define	MEGADLYNX_MFR_SPECIFIC_00_VALUE	0x0010

/* Ericsson specific */
#define	ERICSSON_PMBUS_REVISION_VALUE	0x1
#define ERICSSON_MFR_ID "Ericsson Power Modules"

/* Debug info */
#ifdef	DEBUG_INFO
#define	PRINT_DEBUG(...)	fprintf(stderr, "[DEBUG] --- " __VA_ARGS__)
#else
#define	PRINT_DEBUG(...)
#endif

dcdc_mfrid_t pmbus_get_dcdc_mfr_id(int i2c_bus, useconds_t delay_us)
{
	dcdc_mfrid_t mfrid = MFRID_UNDEFINED;
	const size_t buflen = 32;
	unsigned char block[buflen];
	unsigned long funcs;
	int id;

	if (ioctl(i2c_bus, I2C_FUNCS,&funcs) < 0) {
		funcs = 0;
	}

	if (funcs & I2C_FUNC_SMBUS_READ_BLOCK_DATA) {
		id = i2c_smbus_read_block_data(i2c_bus, PMBUS_MFR_ID, block); /* id holds length of data or -1 */
		usleep(delay_us);
		PRINT_DEBUG("Have block data. Status is %d\n", id);
	} else {
		id = -1;
	}

	if (id != -1) { /* MFR_ID successful */
		if (strncmp((char*)block,ERICSSON_MFR_ID,id) == 0) {
			mfrid = MFRID_ERICSSON;
		}
	} else { /* MFR_ID command is not supported, try another way */
		id = pmbus_mfr_specific_00(i2c_bus, delay_us); /* Try MFR_SPECIFIC_00 way (i.e. GE DC/DC) */
		PRINT_DEBUG("Got 0x%x for MFR_SPECIFIC_00 (expecting 0x%x)\n", id, MEGADLYNX_MFR_SPECIFIC_00_VALUE);
		if (MEGADLYNX_MFR_SPECIFIC_00_VALUE == id) { /* MEGADLYNX chip (GE DC/DC) */
			mfrid = MFRID_GE;
		} else {
			id = pmbus_revision(i2c_bus, delay_us); /* XXX: Dirty hack, should find a way to identify properly */
			PRINT_DEBUG("Got 0x%x for PMBUS_REVISION (expecting 0x%x)\n", id, ERICSSON_PMBUS_REVISION_VALUE);
			if (id == ERICSSON_PMBUS_REVISION_VALUE) {
				mfrid = MFRID_ERICSSON;
			}
		}
	}

	return mfrid;
}

void pmbus_set_vout_trim(int i2c_bus, uint16_t trim, useconds_t delay_us)
{
	i2c_smbus_write_word_data(i2c_bus, PMBUS_VOUT_TRIM, trim);
	usleep(delay_us);
}

__s32 pmbus_get_vout_trim(int i2c_bus, useconds_t delay_us)
{
	__s32 ret = i2c_smbus_read_word_data(i2c_bus, PMBUS_VOUT_TRIM);
        usleep(delay_us);
	return ret;
}

void pmbus_set_vout_cmd(int i2c_bus, uint16_t vout, useconds_t delay_us)
{
	i2c_smbus_write_word_data(i2c_bus, PMBUS_VOUT_COMMAND, vout);
	usleep(delay_us);
}

__s32 pmbus_get_vout_cmd_ericsson(int i2c_bus)
{
	return i2c_insist_read_word(i2c_bus, PMBUS_VOUT_COMMAND, ERICSSON_I2C_WORKAROUND_DELAY_us);
}

double pmbus_read_vout(int i2c_bus, useconds_t delay_us)
{
        int vout_mode = i2c_smbus_read_byte_data(i2c_bus, PMBUS_VOUT_MODE);
        usleep(delay_us);
        if (vout_mode < 0)
		return 0.0;
        int vout = i2c_smbus_read_word_data(i2c_bus, PMBUS_READ_VOUT);
        usleep(delay_us);
        if (vout < 0)
		return 0.0;
        return FLOAT_FROM_PMBUS_VOUT(vout, vout_mode);
}

double pmbus_read_vout_ericsson(int i2c_bus)
{
        int vout_mode = i2c_insist_read_byte(i2c_bus, PMBUS_VOUT_MODE,
					     ERICSSON_I2C_WORKAROUND_DELAY_us);
        if (vout_mode < 0)
		return 0.0;
        int vout = i2c_insist_read_word(i2c_bus, PMBUS_READ_VOUT,
					ERICSSON_I2C_WORKAROUND_DELAY_us);
        if (vout < 0)
		return 0.0;
        return FLOAT_FROM_PMBUS_VOUT(vout, vout_mode);
}

double pmbus_read_vin(int i2c_bus, useconds_t delay_us)
{
        int vin = i2c_smbus_read_word_data(i2c_bus, PMBUS_READ_VIN);
        usleep(delay_us);
        if (vin < 0)
		return 0.0;
        return FLOAT_FROM_PMBUS_LINEAR(vin);
}

double pmbus_read_iout(int i2c_bus, useconds_t delay_us)
{
        int iout = i2c_smbus_read_word_data(i2c_bus, PMBUS_READ_IOUT);
        usleep(delay_us);
        if (iout < 0)
		return 0.0;
        return FLOAT_FROM_PMBUS_LINEAR(iout);
}

double pmbus_read_iout_ericsson(int i2c_bus)
{
        int iout = i2c_insist_read_word(i2c_bus, PMBUS_READ_IOUT,
					ERICSSON_I2C_WORKAROUND_DELAY_us);
        if (iout < 0)
		return 0.0;
        return FLOAT_FROM_PMBUS_LINEAR(iout);
}

double pmbus_read_temp_ericsson(int i2c_bus)
{
        int temp = i2c_insist_read_word(i2c_bus, PMBUS_READ_TEMPERATURE_1,
					ERICSSON_I2C_WORKAROUND_DELAY_us);
        if (temp < 0)
		return 0.0;
        return FLOAT_FROM_PMBUS_LINEAR(temp);
}

__s32 pmbus_revision(int i2c_bus, useconds_t delay_us)
{
        return i2c_insist_read_byte(i2c_bus, PMBUS_REVISION, delay_us);
}

__s32 pmbus_mfr_specific_00(int i2c_bus, useconds_t delay_us)
{
        return i2c_insist_read_word(i2c_bus, PMBUS_MFR_SPECIFIC_00, delay_us);
};

void pmbus_on_off_config(int i2c_bus, uint8_t data, useconds_t delay_us)
{
	i2c_smbus_write_byte_data(i2c_bus, PMBUS_ON_OFF_CONFIG,data);
        usleep(delay_us);
}

void pmbus_store_default_all(int i2c_bus, useconds_t delay_us)
{
	i2c_smbus_write_byte(i2c_bus, PMBUS_STORE_DEFAULT_ALL);
        usleep(delay_us);
}

uint8_t pmbus_status_byte(int i2c_bus, useconds_t delay_us)
{
	int ret = i2c_smbus_read_byte_data(i2c_bus, PMBUS_STATUS_BYTE);
        usleep(delay_us);
	return (uint8_t)ret;
}

void pmbus_on(int i2c_bus, useconds_t delay_us)
{
	i2c_smbus_write_byte_data(i2c_bus, PMBUS_OPERATION, 0x80);
	usleep(delay_us);
}

void pmbus_off(int i2c_bus, useconds_t delay_us)
{
	i2c_smbus_write_byte_data(i2c_bus, PMBUS_OPERATION, 0);
	usleep(delay_us);
}

void pmbus_clear_faults(int i2c_bus, useconds_t delay_us)
{
	i2c_smbus_write_byte(i2c_bus, PMBUS_CLEAR_FAULTS);
	usleep(delay_us);
}

void pmbus_ignore_faults(int i2c_bus, useconds_t delay_us)
{
	i2c_smbus_write_byte_data(i2c_bus, PMBUS_VOUT_OV_FAULT_RESPONSE, 0);
	usleep(delay_us);
	i2c_smbus_write_byte_data(i2c_bus, PMBUS_VOUT_UV_FAULT_RESPONSE, 0);
	usleep(delay_us);
}

bool test_ericsson(int i2c_bus, int dev_num, float vin_low_level, float vin_high_level)
{
	int id;
	float vin;

	id = i2c_insist_read_byte(i2c_bus, PMBUS_REVISION,
				  ERICSSON_I2C_WORKAROUND_DELAY_us);
	if (ERICSSON_PMBUS_REVISION_VALUE != id) {
		printf("ERICSSON[%d] error: Bad PMBUS revision 0x%02X\n",
		       dev_num, id);
		return false;
	}

	vin = pmbus_read_vin(i2c_bus, ERICSSON_I2C_WORKAROUND_DELAY_us);
	if ((vin < vin_low_level) || (vin > vin_high_level)) {
		printf("ERICSSON[%d] error: Bad Vin %.2fV (low is %f, high is %f). Trying again...", dev_num, vin, vin_low_level, vin_high_level);
		usleep(300000);
		vin = pmbus_read_vin(i2c_bus, ERICSSON_I2C_WORKAROUND_DELAY_us);
		if ((vin < vin_low_level) || (vin > vin_high_level)) {
			printf("\nERICSSON[%d] error: Bad Vin %.2fV (low is %f, high is %f). Trying again...", dev_num, vin, vin_low_level, vin_high_level);
			usleep(300000);
			vin = pmbus_read_vin(i2c_bus, ERICSSON_I2C_WORKAROUND_DELAY_us);
			if ((vin < vin_low_level) || (vin > vin_high_level)) {
				printf("\nERICSSON[%d] error: Bad Vin %.2fV (low is %f, high is %f)\n", dev_num, vin, vin_low_level, vin_high_level);
				return false;
			} else {
				printf(" OK\n");
			}
		} else {
			printf(" OK\n");
		}
	}

	return true;
}
