#include <stdbool.h>
#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "knc-asic.h"
#include "asic.h"
#include "eeprom.h"

static const char brdtypestr_JUPITER_REVA[] = "AG";
static const char brdtypestr_JUPITER_REVB[] = "BG";
static const char brdtypestr_JUPITER_ERICSSON[] = "BE";
static const char brdtypestr_NEPTUNE_ERICSSON[] = "NE";
static const char brdtypestr_TITAN_ERICSSON[] = "TI";

const char *get_str_from_board_type(brd_type_t brd_type)
{
	switch (brd_type) {
	case ASIC_BOARD_TITAN:
		return brdtypestr_TITAN_ERICSSON;
	case ASIC_BOARD_NEPTUNE:
		return brdtypestr_NEPTUNE_ERICSSON;
	case ASIC_BOARD_JUPITER_REVB:
		return brdtypestr_JUPITER_REVB;
	case ASIC_BOARD_JUPITER_ERICSSON:
		return brdtypestr_JUPITER_ERICSSON;
	case ASIC_BOARD_JUPITER_REVA:
		return brdtypestr_JUPITER_REVA;
	default:
		return "";
	}
}

brd_type_t asic_boardtype_from_serial(char *serial)
{
	if (strncmp(serial, "NE", 2) == 0)
		return ASIC_BOARD_NEPTUNE;
	if (strncmp(serial, "TI", 2) == 0)
		return ASIC_BOARD_TITAN;
	if (serial[0] == 'A') {
		switch (serial[1]) {
		case '1':
		case '2':
		case 'S':
			return ASIC_BOARD_JUPITER_REVA;
			break;
		case 'G':
			return ASIC_BOARD_JUPITER_REVB;
			break;
		case 'E':
			return ASIC_BOARD_JUPITER_ERICSSON;
			break;
		default:
			return ASIC_BOARD_UNDEFINED;
			break;
		}
	}
	return ASIC_BOARD_UNDEFINED;
}

unsigned int asic_init_boards(asic_board_t** boards)
{
	int idx;
	unsigned int good_boards = 0;
	for (idx = 0; idx < KNC_MAX_ASICS; ++idx) {
		asic_board_t * board = (asic_board_t*)calloc(1, sizeof(asic_board_t));
		board->id = idx;
		if (asic_init_board(board) && board->enabled) {
			++good_boards;
		}
		boards[idx] = board;
	}
	return good_boards;
}

void asic_release_boards(asic_board_t** boards)
{
	int idx;
	for (idx = 0; idx < KNC_MAX_ASICS; ++idx) {
		asic_board_t * board = boards[idx];
		asic_release_board(board);
		free(board);
	}
}

bool asic_board_read_info(asic_board_t *board)
{
	board->enabled = read_serial_num_from_eeprom(board->id, board->serial_num, sizeof(board->serial_num)-1);

	board->type = ASIC_BOARD_UNDEFINED;

	if (!board->enabled) {
		fprintf(stderr,
			"ASIC board #%d is non-functional: Bad EEPROM data\n",
			board->id);
		return false;
	}

	if ('X' == board->serial_num[31]) {
		fprintf(stderr,
			"ASIC board #%d is marked with TEST_FAIL flag\n",
			board->id);
	}
	board->serial_num[sizeof(board->serial_num) - 1] = '\0';

	board->type = asic_boardtype_from_serial((char *)board->serial_num);
	return true;	
}

bool asic_init_board(asic_board_t *board)
{
	asic_board_read_info(board);
	if (board->enabled) {
		printf("ASIC board #%d: sn = %s type = %s\n", board->id,
		       board->serial_num, get_str_from_board_type(board->type));

		board->num_dcdc = 0;
		board->i2c_bus = i2c_connect(FIRST_ASIC_I2C_BUS + board->id);
	}
	return true;
}

void asic_release_board(asic_board_t* board)
{
	i2c_disconnect(board->i2c_bus);
	board->i2c_bus = -1;
}

bool dcdc_is_ok(asic_board_t *board, int dcdc)
{
	if ((0 > dcdc) || (KNC_MAX_DCDC_DEVICES <= dcdc))
		return false;

	switch (board->type) {
	case ASIC_BOARD_JUPITER_REVB:
	case ASIC_BOARD_JUPITER_ERICSSON:
		return true;
	case ASIC_BOARD_JUPITER_REVA:
	default:
		if ((0 == dcdc) || (2 == dcdc) || (4 == dcdc) || (7 == dcdc))
			return true;
	}

	return false;
}

uint8_t get_primary_dcdc_addr_for_die(int die)
{
	switch (die) {
	case 0:
		return DCDC_ADDR(0);
	case 1:
		return DCDC_ADDR(2);
	case 2:
		return DCDC_ADDR(4);
	case 3:
		return DCDC_ADDR(7);
	default:
		return DCDC_ADDR(0);
	}
}
