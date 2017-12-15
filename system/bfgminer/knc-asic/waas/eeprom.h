#ifndef __NEPTUNE_TEST_EEPROM_H_
#define __NEPTUNE_TEST_EEPROM_H_

#include <stdbool.h>
#include <stdint.h>

#include "knc-asic.h"

#define	CONTROL_BOARD_EEPROM_FILE	"/sys/bus/i2c/devices/1-0054/eeprom"
#define	BBB_EEPROM_FILE			"/sys/bus/i2c/devices/0-0050/eeprom"

#define	EEPROM_FILE	"/sys/bus/i2c/devices/%d-0050/eeprom"
#define	EEPROM_SIZE	1024

#define	ENTRIES_IN_ENA_MAP	(KNC_MAX_CORES_PER_DIE / 8)

struct __attribute__ ((__packed__)) eeprom_neptune {
    /* 0: */
	uint8_t reserved_1[32];
    /* 32: */
	uint8_t SN[32];
    /* 64: */
	uint8_t enabled_coremap[ENTRIES_IN_ENA_MAP];
    /* 244: */
	uint8_t board_revision;
    /* 245: */
	uint8_t control_data[11];
    /* 256: */
	uint8_t asic_test_coremap[ENTRIES_IN_ENA_MAP];
    /* 436: */
	uint8_t reserved_3[556];
    /* 992: */
	uint8_t reserved_2[32];
};

bool read_serial_num_from_eeprom(int port_num,
				 uint8_t *serial_num, int serial_num_size);

#endif /* __NEPTUNE_TEST_EEPROM_H_ */
