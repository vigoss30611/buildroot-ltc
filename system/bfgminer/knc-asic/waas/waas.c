/***********************************************************************
 * WAAS = Work-Arounds and Advanced Settings
 * Command-line options:
 *	-c <config-file>
 *		Specify the config file name to read on start. If this
 *		option is not used, waas uses default config file:
 *		 (see DEFAULT_CONFIG_FILE below)
 *	-g <info-request>
 *		Get data from device: voltage outputs, current outputs etc.
 *		Output is printed in JSON format. Valid info-request values:
 *	    all-asic-info :
 *		Print outpotu voltage and current for each DC/DC. Print
 *		temperature for the ASIC board.
 *	-i <info-request>
 *		Print requested info and exit. Valid info-request values:
 *	    valid-die-voltages :
 *		Print list of valid DC/DC voltage offsets
 *	    valid-die-frequencies :
 *		Print list of valid die PLL frequencies
 *	    valid-ranges :
 *		Print all of the above in JSON format
 *	-o <output-file>
 *		Specify the name of the file where to print running config info.
 *		If this option is not used, waas prints running config to the
 *		standard output.
 *	-d
 *		Do not read config file, use default values instead
 *	-r
 *		Read only. Do not read config file, do not change running values;
 *		waas will only print running config end exit
 *	-a
 *		Auto-adjust PLL frequency based on the DC/DC temperatures.
 *		Works only for Ericsson devices.
 * 	-s asic:die
 * 		apply settings to a single asic/die
 ***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <sys/stat.h>
#include <linux/limits.h>

#include "jsmn.h"

#include "asic.h"
#include "pmbus.h"
#include "lm75.h"

#include "knc-asic.h"

#include "knc-transport.h"

#define	DEFAULT_CONFIG_FILE	"/config/advanced.conf"
#define	WAAS_CURRENT_FREQ	"/var/run/.waas_curfreq"
#define	WAAS_AUTOPLL_TIMESTAMP	"/var/run/.waas_t1"
#ifdef CONTROLLER_BOARD_RPI
#define	EXPECTED_PERFORMANCE_FILE	"/var/run/expected_performance"
#define	REVISION_FILE			"/var/run/revision"
#else
#define	EXPECTED_PERFORMANCE_FILE	"/etc/expected_performance"
#define	REVISION_FILE			"/etc/revision"
#endif

/* Factory-programmed VOUT value, 0.85 V */
#define	ERICSON_FACTORY_VOUT_VALUE	0x1b33
/* GE default trim values: 0xff67 = -0.15V, 0x0033 = 0.05V */
#define	DEFAULT_4DCDC_VOUT_TRIM	0xff67
#define	DEFAULT_8DCDC_VOUT_TRIM	0x0033

#define DEFAULT_TITAN_VOUT_TRIM 0xfed4
#define DEFAULT_NEPTUNE_VOUT_TRIM 0xfed4
#define DEFAULT_JUPITER_VOUT_TRIM (-420)

#define	MIN_DCDC_VOUT_TRIM	(-200)
#define	MAX_DCDC_VOUT_TRIM	200
#define	DCDC_VOUT_TRIM_STEP	10
#define	DCDC_VOLTAGE_FROM_OFFSET_GE(offs)	((3.0 * (offs)) / 4096.0)
#define	DCDC_VOLTAGE_FROM_OFFSET_ERICSSON(offs)	((offs) / 8192.0)
#define	DCDC_VOLTAGE_OFFSET_ERICSSON_MULTIPLIER	6

#define	IOUT_NORMAL_LEVEL	(5.0)

#define	MIN_DIE_FREQ_JUPITER_REVA	400
#define	MAX_DIE_FREQ_JUPITER_REVA	775
#define	DEFAULT_DIE_FREQ_JUPITER_REVA	750

#define	MIN_DIE_FREQ_JUPITER_REVB	400
#define	MAX_DIE_FREQ_JUPITER_REVB	1200
#define	DEFAULT_DIE_FREQ_JUPITER_REVB	900

#define	MIN_DIE_FREQ_NEPTUNE		50
#define	MAX_DIE_FREQ_NEPTUNE		500
#define	DEFAULT_DIE_FREQ_NEPTUNE	475

#define	MIN_DIE_FREQ_TITAN		50
#define	MAX_DIE_FREQ_TITAN			325
#define	DEFAULT_DIE_FREQ_TITAN		300

#define	DIE_FREQ_STEP			25

/* Defaults for Auto-PLL parameters */
#define	AUTOPLL_TEMP_OK_LOW_LEVEL	70
#define	AUTOPLL_TEMP_OK_HIGH_LEVEL	80
#define	AUTOPLL_FREQ_STEP_UP		(DIE_FREQ_STEP)
#define	AUTOPLL_FREQ_STEP_DOWN		(DIE_FREQ_STEP)
#define	AUTOPLL_MINIMUM_CHANGE_INTERVAL	300
#define	AUTOPLL_GOOD_CORES_TOLERANCE	10
/* How fast we update NormalCores. 1.0 = immediately, 0.0 = never */
#define	AUTOPLL_NORMALCORES_ALPHA	0.1

struct device_t {
	brd_type_t dev_type;
	asic_board_t boards[KNC_MAX_ASICS];
	int freq_start[KNC_MAX_ASICS];
	int freq_end[KNC_MAX_ASICS];
	int freq_default[KNC_MAX_ASICS];
};

struct advanced_config {
	int16_t dcdc_V_offset[KNC_MAX_ASICS][KNC_MAX_DIES_PER_ASIC];
	int die_freq[KNC_MAX_ASICS][KNC_MAX_DIES_PER_ASIC];
};

static FILE *fopen_temp_file(const char *target_file_name, char **temp_file_name, const char *mode)
{
	FILE *f;
	int i;
	char file_name[1024];

	/* try to create temp file up to 10 times, then give up */
	for (i = 0; i < 10; ++i) {
		sprintf(file_name, "%s.%d.%ld%d", target_file_name, getpid(), (long)time(NULL), rand());
		f = fopen(file_name, mode);
		if (NULL != f) {
			*temp_file_name = strdup(file_name);
			return f;
		}
	}

	*temp_file_name = NULL;
	return NULL;
}

static void detect_device_type(struct device_t *dev, bool write_to_file)
{
	int asic;
	FILE *f = NULL;
	char *temp_file_name = NULL;

	if (write_to_file) {
		f = fopen_temp_file(REVISION_FILE, &temp_file_name, "w");
		if (NULL == f)
			fprintf(stderr, "Can not open file %s: %m\n", REVISION_FILE);
	}

	dev->dev_type = ASIC_BOARD_UNDEFINED;
	for (asic = 0; asic < KNC_MAX_ASICS; ++asic) {
		asic_board_t *board = &(dev->boards[asic]);
		board->id = asic;
		if (asic_board_read_info(board)) {
			if (board->type > dev->dev_type)
				dev->dev_type = board->type;
			if (NULL != f)
				fprintf(f, "BOARD%d=%s\n", asic, get_str_from_board_type(board->type));
		} else {
			board->type = ASIC_BOARD_UNDEFINED;
			if (NULL != f)
				fprintf(f, "BOARD%d=OFF\n", asic);
		}

		switch (board->type) {
		case ASIC_BOARD_TITAN:
			dev->freq_start[asic] = MIN_DIE_FREQ_TITAN;
			dev->freq_end[asic] = MAX_DIE_FREQ_TITAN;
			dev->freq_default[asic] = DEFAULT_DIE_FREQ_TITAN;
			break;
		case ASIC_BOARD_NEPTUNE:
			dev->freq_start[asic] = MIN_DIE_FREQ_NEPTUNE;
			dev->freq_end[asic] = MAX_DIE_FREQ_NEPTUNE;
			dev->freq_default[asic] = DEFAULT_DIE_FREQ_NEPTUNE;
			break;
		case ASIC_BOARD_JUPITER_REVB:
		case ASIC_BOARD_JUPITER_ERICSSON:
			dev->freq_start[asic] = MIN_DIE_FREQ_JUPITER_REVB;
			dev->freq_end[asic] = MAX_DIE_FREQ_JUPITER_REVB;
			dev->freq_default[asic] = DEFAULT_DIE_FREQ_JUPITER_REVB;
			break;
		case ASIC_BOARD_JUPITER_REVA:
			dev->freq_start[asic] = MIN_DIE_FREQ_JUPITER_REVA;
			dev->freq_end[asic] = MAX_DIE_FREQ_JUPITER_REVA;
			dev->freq_default[asic] = DEFAULT_DIE_FREQ_JUPITER_REVA;
			break;
		default:
			fprintf(stderr, "ERROR: decect unhandled board type %d\n", board->type);
			break;
		case ASIC_BOARD_UNDEFINED:
			dev->freq_start[asic] = 0;
			dev->freq_end[asic] = 0;
			dev->freq_default[asic] = 0;
			break;
		}
	}

	if (NULL != f) {
		fprintf(f, "DEVICE=%s\n", get_str_from_board_type(dev->dev_type));
		fclose(f);
		rename(temp_file_name, REVISION_FILE);
		free(temp_file_name);
	}
}

static inline float dcdc_voltage_from_offset(dcdc_mfrid_t mfr_id, int16_t offs)
{
	if (MFRID_ERICSSON == mfr_id)
		return DCDC_VOLTAGE_FROM_OFFSET_ERICSSON(offs);
	if (MFRID_GE == mfr_id)
		return DCDC_VOLTAGE_FROM_OFFSET_GE(offs);
	return 0.0;
}

static inline bool file_is_present(const char *fname)
{
	struct stat sb;
	if (0 != stat(fname, &sb))
		return false;
	if (!S_ISREG(sb.st_mode))
		return false;
	return true;
}

static int read_die_freq(int asic, int die)
{
	int cur_asic, cur_die;
	int freq = -1;
	FILE *f;

	if ((KNC_MAX_ASICS <= asic) || (KNC_MAX_DIES_PER_ASIC <= die))
		return -1;

	f = fopen(WAAS_CURRENT_FREQ, "r");
	if (NULL == f)
		return -1;

	for (cur_asic = 0; cur_asic < asic; ++cur_asic) {
		for (cur_die = 0; cur_die < KNC_MAX_DIES_PER_ASIC; ++cur_die) {
			if (1 != fscanf(f, "%i", &freq)) {
				fclose(f);
				return -1;
			}
		}
	}

	for (cur_die = 0; cur_die <= die; ++cur_die) {
		if (1 != fscanf(f, "%i", &freq)) {
			fclose(f);
			return -1;
		}
	}

	fclose(f);
	return freq;
}

static void do_freq(void *ctx, int channel, int die, int freq)
{
	uint8_t request[2048];
	uint8_t response[2048];


	int len = knc_prepare_freq(request, 0, sizeof(request), channel, die, freq);

	knc_trnsp_transfer(ctx, request, response, len);

	int accepted = knc_decode_freq(response);

#ifdef DEBUG_INFO
	if (accepted > 0)
		printf("KnC %d-%d: Accepted FREQ=%d\n", channel, die, accepted);
#endif /* DEBUG_INFO */
	if (accepted < 0)
		fprintf(stderr, "KnC %d-%d: Frequency change FAILED!", channel, die);
}

static bool set_die_freq(int asic, int die, int freq)
{
	int cur_asic, cur_die;
	int cur_freq[KNC_MAX_ASICS][KNC_MAX_DIES_PER_ASIC];
	FILE *f;
	char *temp_file_name = NULL;

	if ((KNC_MAX_ASICS <= asic) || (KNC_MAX_DIES_PER_ASIC <= die))
		return false;

	for (cur_asic = 0; cur_asic < KNC_MAX_ASICS; ++cur_asic)
		for (cur_die = 0; cur_die < KNC_MAX_DIES_PER_ASIC; ++cur_die)
			cur_freq[cur_asic][cur_die] = read_die_freq(cur_asic, cur_die);

	cur_freq[asic][die] = freq;

	/* Set die frequency in FPGA */
	void *ctx = knc_trnsp_new(0);
	if (NULL != ctx) {
		do_freq(ctx, asic, die, freq);
		knc_trnsp_free(ctx);
	}

	f = fopen_temp_file(WAAS_CURRENT_FREQ, &temp_file_name, "w");
	if (NULL == f)
		return false;

	for (cur_asic = 0; cur_asic < KNC_MAX_ASICS; ++cur_asic)
		for (cur_die = 0; cur_die < KNC_MAX_DIES_PER_ASIC; ++cur_die)
			fprintf(f, "%i ", cur_freq[cur_asic][cur_die]);
	fclose(f);
	/* Atomically move file to the proper position */
	rename(temp_file_name, WAAS_CURRENT_FREQ);
	free(temp_file_name);
	return true;
}

/* Print out running operating values in JSON format */
static int do_print_running_info(FILE *f, struct device_t *dev, int start_asic, int end_asic, int start_die, int end_die)
{
	int asic, die, dcdc;
	int i2c_bus;
	int id;
	int16_t temp_s;
	float temp, vout, iout;
	bool dcdc_present;
	int die_freq[KNC_MAX_DIES_PER_ASIC];

	fprintf(f, "{\n");
	for (asic = start_asic; asic <= end_asic; ++asic)
	{
		asic_board_t *board = &(dev->boards[asic]);
		dcdc_mfrid_t board_mfrid = mfrid_from_board_type(board->type);
		if (0 > (i2c_bus = i2c_connect(FIRST_ASIC_I2C_BUS + asic)))
			continue;
		fprintf(f, "\"asic_%d\": {\n", asic + 1);
		int start_dcdc = start_die * 2 + 0;
		int end_dcdc = end_die * 2 + 1;
		for (dcdc = start_dcdc; dcdc <= end_dcdc; ++dcdc) {
			i2c_set_slave_device_addr(i2c_bus, DCDC_ADDR(dcdc));
			dcdc_present = false;
			if ((MFRID_GE == board_mfrid) || (MFRID_ERICSSON == board_mfrid)) {
				dcdc_present = (board_mfrid == pmbus_get_dcdc_mfr_id(i2c_bus, ERICSSON_I2C_WORKAROUND_DELAY_us));
			}
			if (!dcdc_present) {
				fprintf(f, "\t\"dcdc%d_Vout\": \"\",\n", dcdc);
				fprintf(f, "\t\"dcdc%d_Iout\": \"\",\n", dcdc);
				fprintf(f, "\t\"dcdc%d_Temp\": \"\",\n", dcdc);
				continue;
			}
			if (MFRID_ERICSSON == board_mfrid)
				vout = pmbus_read_vout_ericsson(i2c_bus);
			else
				vout = pmbus_read_vout(i2c_bus, 0);
			if ((vout < 0.0) || (vout > 30.0))
				vout = 0.0;
			fprintf(f, "\t\"dcdc%d_Vout\": \"%1.4f\",\n", dcdc, vout);
			if (MFRID_ERICSSON == board_mfrid)
				iout = pmbus_read_iout_ericsson(i2c_bus);
			else
				iout = pmbus_read_iout(i2c_bus, 0);
			if ((iout < 0.0) || (iout > 100.0))
				iout = 0.0;
			fprintf(f, "\t\"dcdc%d_Iout\": \"%1.4f\",\n", dcdc, iout);
			if (MFRID_ERICSSON == board_mfrid) {
				temp = pmbus_read_temp_ericsson(i2c_bus);
				fprintf(f, "\t\"dcdc%d_Temp\": \"%1.4f\",\n", dcdc, temp);
			} else {
				fprintf(f, "\t\"dcdc%d_Temp\": \"\",\n", dcdc);
			}
		}
		i2c_set_slave_device_addr(i2c_bus, 0x48);
		id = i2c_smbus_read_byte_data(i2c_bus, LM75_ID);
		if (LM75_ID_HIGH_NIBBLE == (id & 0xF0)) {
			temp_s = i2c_smbus_read_word_data_bswap(i2c_bus,
							LM75_TEMPERATURE);
			temp = LM75_TEMP_FROM_INT(temp_s);
		} else {
			temp = -1000.0;
		}
		i2c_disconnect(i2c_bus);
		/* Read die frequencies and offsets */
		if ((MFRID_GE == board_mfrid) || (MFRID_ERICSSON == board_mfrid)) {
			for (die = start_die; die <= end_die; ++die)
				die_freq[die] = read_die_freq(asic, die);
			i2c_bus = i2c_connect(FIRST_ASIC_I2C_BUS + asic);
			for (die = start_die; die <= end_die; ++die) {
				int data = die_freq[die];
				if ((data < dev->freq_start[asic]) || (data > dev->freq_end[asic]))
					data = dev->freq_default[asic];
				fprintf(f, "\t\"die%d_Freq\": \"%d\",\n", die, data);

				bool voffset_valid = false;
				if (0 <= i2c_bus) {
					dcdc = get_primary_dcdc_addr_for_die(die) - DCDC_BASE_ADDR;
					i2c_set_slave_device_addr(i2c_bus, DCDC_ADDR(dcdc));
					voffset_valid = true;
					if (MFRID_ERICSSON == board_mfrid) {
						data = pmbus_get_vout_cmd_ericsson(i2c_bus);
						voffset_valid = (0 <= data);
						data -= ERICSON_FACTORY_VOUT_VALUE;
					} else {
						data = pmbus_get_vout_trim(i2c_bus, 0);
					}
				}
				if (voffset_valid)
					fprintf(f, "\t\"die%d_Voffset\": \"%1.4f\",\n", die, dcdc_voltage_from_offset(board_mfrid, data));
				else
					fprintf(f, "\t\"die%d_Voffset\": \"\",\n", die);
			}
			if (0 <= i2c_bus)
				i2c_disconnect(i2c_bus);
		} else {
			for (die = start_die; die <= end_die; ++die) {
				fprintf(f, "\t\"die%d_Freq\": \"\",\n", die);
				fprintf(f, "\t\"die%d_Voffset\": \"\",\n", die);
			}
		}
		/* Print out previously read temperature */
		if (-500.0 < temp)
			fprintf(f, "\t\"temperature\": \"%.1f\"\n", temp);
		else
			fprintf(f, "\t\"temperature\": \"\"\n");
		if (asic < end_asic)
			fprintf(f, "},\n");
		else
			fprintf(f, "}\n");
	}
	fprintf(f, "}\n");

	return 0;
}

static int nearest_valid_asic_freq(brd_type_t type, int fr)
{
	int mod;
	int start, end;

	switch (type) {
	case ASIC_BOARD_TITAN:
		start = MIN_DIE_FREQ_TITAN;
		end = MAX_DIE_FREQ_TITAN;
		break;
	case ASIC_BOARD_NEPTUNE:
		start = MIN_DIE_FREQ_NEPTUNE;
		end = MAX_DIE_FREQ_NEPTUNE;
		break;
	case ASIC_BOARD_JUPITER_REVB:
	case ASIC_BOARD_JUPITER_ERICSSON:
		start = MIN_DIE_FREQ_JUPITER_REVB;
		end = MAX_DIE_FREQ_JUPITER_REVB;
		break;
	case ASIC_BOARD_JUPITER_REVA:
	default:
		start = MIN_DIE_FREQ_JUPITER_REVA;
		end = MAX_DIE_FREQ_JUPITER_REVA;
		break;
	}

	mod = fr % DIE_FREQ_STEP;
	fr -= mod;
	if (mod > (DIE_FREQ_STEP / 2))
		fr += DIE_FREQ_STEP;

	if (fr < start)
		fr = start;
	else if (fr > end)
		fr = end;

	return fr;
}

static int16_t find_offset_from_voltage(dcdc_mfrid_t mfr_id, float vf)
{
	float mindiff, diff;
	int16_t offs, good_offs = 0;
	int start = MIN_DCDC_VOUT_TRIM;
	int end = MAX_DCDC_VOUT_TRIM;
	int step = DCDC_VOUT_TRIM_STEP;

	mindiff = dcdc_voltage_from_offset(mfr_id, good_offs) - vf;
	if (mindiff < 0.0)
		mindiff = -mindiff;

	if (MFRID_ERICSSON == mfr_id) {
		start *= DCDC_VOLTAGE_OFFSET_ERICSSON_MULTIPLIER;
		end *= DCDC_VOLTAGE_OFFSET_ERICSSON_MULTIPLIER;
		step *= DCDC_VOLTAGE_OFFSET_ERICSSON_MULTIPLIER;
	}

	for (offs = start; offs <= end; offs += step) {
		diff = dcdc_voltage_from_offset(mfr_id, offs) - vf;
		if (diff < 0.0)
			diff = -diff;
		if (diff < mindiff) {
			mindiff = diff;
			good_offs = offs;
		}
	}

	return good_offs;
}

static int default_dcdc_vout_trim(asic_board_t *board)
{
	switch(board->type) {
	case ASIC_BOARD_JUPITER_REVA:
	case ASIC_BOARD_JUPITER_REVB:
		return (board->num_dcdc <= 4) ?
		       DEFAULT_4DCDC_VOUT_TRIM :
		       DEFAULT_8DCDC_VOUT_TRIM ;
		break;
	case ASIC_BOARD_JUPITER_ERICSSON:
		return DEFAULT_JUPITER_VOUT_TRIM;
		break;
	case ASIC_BOARD_NEPTUNE:
		return DEFAULT_NEPTUNE_VOUT_TRIM;
		break;
	case ASIC_BOARD_TITAN:
		return DEFAULT_TITAN_VOUT_TRIM;
		break;
	default:
		fprintf(stderr, "INTERNAL ERROR: v_offset unhandled ASIC type %d\n", board->type);
		return 0;
	}
}

static void read_running_settings(struct advanced_config * cfg,
				  struct device_t *dev, bool use_defaults, int start_asic, int end_asic, int start_die, int end_die)
{
	int i2c_bus;
	int data;
	int asic, die, dcdc;
	dcdc_mfrid_t mfrids[KNC_MAX_DCDC_DEVICES];

	/* Die voltage offsets */
	for (asic = start_asic; asic <= end_asic; ++asic)
	{
		asic_board_t *board = &(dev->boards[asic]);
		dcdc_mfrid_t board_mfrid = mfrid_from_board_type(board->type);
		if (0 > (i2c_bus = i2c_connect(FIRST_ASIC_I2C_BUS + asic))) {
			for (die = start_die; die <= end_die; ++die) {
				cfg->dcdc_V_offset[asic][die] = 0x7FFF;
			}
			continue;
		}
		board->num_dcdc = 0;
		if (MFRID_UNDEFINED != board_mfrid) {
			for (dcdc = 0; dcdc < KNC_MAX_DCDC_DEVICES; ++dcdc) {
				i2c_set_slave_device_addr(i2c_bus, DCDC_ADDR(dcdc));
				mfrids[dcdc] = pmbus_get_dcdc_mfr_id(i2c_bus, ERICSSON_I2C_WORKAROUND_DELAY_us);
				if (mfrids[dcdc] == board_mfrid)
					board->num_dcdc += 1;
			}
		}
		for (die = start_die; die <= end_die; ++die) {
			if (0 == board->num_dcdc) {
				cfg->dcdc_V_offset[asic][die] = 0x7FFF;
				continue;
			}
			dcdc = get_primary_dcdc_addr_for_die(die) - DCDC_BASE_ADDR;
			i2c_set_slave_device_addr(i2c_bus, DCDC_ADDR(dcdc));
			if ((!use_defaults) && (mfrids[dcdc] == board_mfrid)) {
				if (MFRID_ERICSSON == board_mfrid) {
					data = pmbus_get_vout_cmd_ericsson(i2c_bus);
					data -= ERICSON_FACTORY_VOUT_VALUE;
				} else {
					data = pmbus_get_vout_trim(i2c_bus, 0);
				}
			} else {
				data = default_dcdc_vout_trim(board);
			}
			cfg->dcdc_V_offset[asic][die] = find_offset_from_voltage(board_mfrid, dcdc_voltage_from_offset(board_mfrid, data));
		}
		i2c_disconnect(i2c_bus);
	}

	/* Die frequencies */
	for (asic = start_asic; asic <= end_asic; ++asic)
	{
		asic_board_t *board = &(dev->boards[asic]);
		for (die = start_die; die <= end_die; ++die) {
			int data = -1;
			if (ASIC_BOARD_UNDEFINED != board->type) {
				if (use_defaults) {
					data = dev->freq_default[asic];
				} else {
					data = read_die_freq(asic, die);
				}
			}
			cfg->die_freq[asic][die] = data;
		}
	}
}

struct MemoryStruct {
	char *memory;
	size_t size;
};

static int parse_config_file(char *file_name, struct advanced_config * cfg, struct device_t *dev)
{
	FILE *f;
	int i, len;
	struct MemoryStruct chunk;
	jsmn_parser parser;
	jsmntok_t tokens[256];
	int json_tokens;
	char str[4096], tmp_str[4096];
	int asic_v, asic_f, die, tmp;

	f = fopen(file_name, "r");
	if (NULL == f) {
		fprintf(stderr, "Can not open config file %s: %m\n", file_name);
		return -1;
	}

	chunk.memory = malloc(1);
	chunk.size = 0;
	while (NULL != fgets(str, sizeof(str), f)) {
		len = strlen(str);
		chunk.memory = realloc(chunk.memory, chunk.size + len + 1);
		if (NULL == chunk.memory) {
			fprintf(stderr, "realloc failed\n");
			fclose(f);
			return -2;
		}
		memcpy(&(chunk.memory[chunk.size]), str, len);
		chunk.size += len;
		chunk.memory[chunk.size] = '\0';
	}
	fclose(f);

	jsmn_init(&parser);
	json_tokens = jsmn_parse(&parser, chunk.memory, tokens, ARRAY_SIZE(tokens));
	if (json_tokens < 0) {
		fprintf(stderr, "Can not parse JSON in config file %s: %d\n",
			file_name, json_tokens);
		if(chunk.memory)
			free(chunk.memory);
		return -3;
	}

	asic_v = asic_f = -1;

	for (i = 0; i < json_tokens; ++i) {
		chunk.memory[tokens[i].end] = 0;
		if (0 == i)
			continue;
		tmp = -1;
		if (2 == sscanf(&chunk.memory[tokens[i].start],
				"asic_%d_%s", &tmp, tmp_str)) {
			if ((tmp > 0) && (tmp <= KNC_MAX_ASICS)) {
				asic_f = asic_v = -1;
				if (0 == strcmp("voltage", tmp_str)) {
					asic_v = tmp - 1;
				} else if (0 == strcmp("frequency", tmp_str)) {
					asic_f = tmp - 1;
				}
			}
			continue;
		}
		die = tmp = -1;
		if (1 == sscanf(&chunk.memory[tokens[i - 1].start],
				"die%d", &tmp)) {
			if ((tmp > 0) && (tmp <= KNC_MAX_DIES_PER_ASIC))
				die = tmp - 1;
		}
		if (die >= 0 && dev->boards[asic_v].enabled) {
			if (asic_v >= 0) {
				float vf = -1000.0;
				if (1 == sscanf(&chunk.memory[tokens[i].start],
						"%f", &vf)) {
					if (vf > -500.0) {
						cfg->dcdc_V_offset[asic_v][die] = find_offset_from_voltage(mfrid_from_board_type(dev->boards[asic_v].type), vf);
					}
				}
				if (vf <= -500.0)
					cfg->dcdc_V_offset[asic_v][die] = default_dcdc_vout_trim(&dev->boards[asic_v]);
			} else if (asic_f >= 0) {
				int asic_fr = -1;
				if (1 == sscanf(&chunk.memory[tokens[i].start],
								"%d", &asic_fr)) {
					cfg->die_freq[asic_f][die] = nearest_valid_asic_freq(dev->boards[asic_f].type, asic_fr);
				}
				if (0 >= asic_fr) {
					cfg->die_freq[asic_f][die] = 0;
				}
			}
		}
	}

	if(chunk.memory)
		free(chunk.memory);
	return 0;
}

/* die_freq - array of frequencies; if NULL - do not set frequency, only reset
 * only_this_die - set frequency and/or reset only this die. If < 0 then set/reset all dies
 */
static bool asic_set_die_frequencies(asic_board_t *board, int *die_freq, int start_die, int end_die)
{
	int die;
	bool success;

	if (ASIC_BOARD_UNDEFINED == board->type) {
		return false;
	}

	success = true;
	for (die = start_die; die <= end_die; ++die) {
		int freq = (0 >= die_freq[die]) ? 0 : nearest_valid_asic_freq(board->type, die_freq[die]);
		if (!set_die_freq(board->id, die, freq))
			success = false;
	}
	return success;
}

static bool implement_settings(struct advanced_config * cfg,
			       struct device_t *dev, int start_asic, int end_asic, int start_die, int end_die)
{
	int i2c_bus;
	bool success = true;
	int asic, dcdc, die;
	int16_t data;

	/* Die voltage offsets */
	for (asic = start_asic; asic <= end_asic; ++asic) {
		asic_board_t *board = &(dev->boards[asic]);
		if (ASIC_BOARD_UNDEFINED == board->type) {
			continue;
		}
		if (0 > (i2c_bus = i2c_connect(FIRST_ASIC_I2C_BUS + asic))) {
			continue;
		}
		dcdc_mfrid_t board_mfrid = mfrid_from_board_type(board->type);
		int start_dcdc = start_die * 2 + 0;
		int end_dcdc = end_die * 2 + 1;
		int start = MIN_DCDC_VOUT_TRIM;
		int end = MAX_DCDC_VOUT_TRIM;
		if (MFRID_ERICSSON == board_mfrid) {
			start *= DCDC_VOLTAGE_OFFSET_ERICSSON_MULTIPLIER;
			end *= DCDC_VOLTAGE_OFFSET_ERICSSON_MULTIPLIER;
		}
		if (MFRID_ERICSSON == board_mfrid) {
			for (dcdc = start_dcdc; dcdc <= end_dcdc; ++dcdc) {
				i2c_set_slave_device_addr(i2c_bus, DCDC_ADDR(dcdc));
				pmbus_off(i2c_bus, ERICSSON_SAFE_BIG_DELAY);
			}
		}
		for (dcdc = start_dcdc; dcdc <= end_dcdc; ++dcdc) {
			die = dcdc / 2;
			data = cfg->dcdc_V_offset[asic][die];
			if (data < start)
				data = start;
			if (data > end)
				data = end;
			i2c_set_slave_device_addr(i2c_bus, DCDC_ADDR(dcdc));
			if (MFRID_ERICSSON == board_mfrid) {
				pmbus_set_vout_cmd(i2c_bus, ERICSON_FACTORY_VOUT_VALUE + data, ERICSSON_SAFE_BIG_DELAY);
			} else {
				if (dcdc_is_ok(board, dcdc))
					pmbus_set_vout_trim(i2c_bus, data, 0);
				else
					pmbus_on_off_config(i2c_bus, PMBUS_ON_OFF_SHUTDOWN, 0);
			}
		}
		if (MFRID_ERICSSON == board_mfrid) {
			for (dcdc = start_dcdc; dcdc <= end_dcdc; ++dcdc) {
				die = dcdc / 2;
				if (0 < cfg->die_freq[asic][die]) {
					i2c_set_slave_device_addr(i2c_bus, DCDC_ADDR(dcdc));
					pmbus_on(i2c_bus, ERICSSON_SAFE_BIG_DELAY);
				}
			}
		}
		i2c_disconnect(i2c_bus);
	}

	/* Die PLL frequency */
	for (asic = start_asic; asic <= end_asic; ++asic) {
		asic_board_t *board = &(dev->boards[asic]);
		asic_set_die_frequencies(board, cfg->die_freq[asic], start_die, end_die);
	}

	return success;
}

static void write_expected_performance(struct advanced_config *cfg, struct device_t *dev)
{
	int sum_freq = 0;
	int asic, die;
	char *temp_file_name = NULL;
	FILE *f = fopen_temp_file(EXPECTED_PERFORMANCE_FILE, &temp_file_name, "w");
	if (NULL == f)
		return;
	for (asic = 0; asic < KNC_MAX_ASICS; ++asic) {
		for (die = 0; die < KNC_MAX_DIES_PER_ASIC; ++die) {
			int data = cfg->die_freq[asic][die];
			if ((data < dev->freq_start[asic]) || (data > dev->freq_end[asic]))
				data = dev->freq_default[asic];
			sum_freq += data;
		}
	}
	/* Write down expected hashing rate in MHs */
	fprintf(f, "%u", sum_freq * KNC_MAX_CORES_PER_DIE);
	fclose(f);
#ifdef DEBUG_INFO
	printf("[%lu] EXPECTED_PERF: %u MHs\n", (unsigned long)time(NULL), sum_freq * KNC_MAX_CORES_PER_DIE);
#endif /* DEBUG_INFO */
	/* Atomically move file to the proper position */
	rename(temp_file_name, EXPECTED_PERFORMANCE_FILE);
	free(temp_file_name);
	return;
}

/* Delays in this restart procedure must be long enough */
static void reset_pair_of_ericsson_dcdcs(int i2c_bus, uint8_t DCDC_addr_1, uint8_t DCDC_addr_2)
{
	/* PMBus OFF */
	i2c_set_slave_device_addr(i2c_bus, DCDC_addr_1);
	pmbus_off(i2c_bus, ERICSSON_SAFE_BIG_DELAY);
	i2c_set_slave_device_addr(i2c_bus, DCDC_addr_2);
	pmbus_off(i2c_bus, ERICSSON_SAFE_BIG_DELAY);
	/* Ignore Faults */
	i2c_set_slave_device_addr(i2c_bus, DCDC_addr_1);
	pmbus_ignore_faults(i2c_bus, ERICSSON_SAFE_BIG_DELAY);
	i2c_set_slave_device_addr(i2c_bus, DCDC_addr_2);
	pmbus_ignore_faults(i2c_bus, ERICSSON_SAFE_BIG_DELAY);
	/* Clear errors */
	i2c_set_slave_device_addr(i2c_bus, DCDC_addr_1);
	pmbus_clear_faults(i2c_bus, ERICSSON_SAFE_BIG_DELAY);
	i2c_set_slave_device_addr(i2c_bus, DCDC_addr_2);
	pmbus_clear_faults(i2c_bus, ERICSSON_SAFE_BIG_DELAY);
	/* PMBus ON */
	i2c_set_slave_device_addr(i2c_bus, DCDC_addr_1);
	pmbus_on(i2c_bus, ERICSSON_SAFE_BIG_DELAY);
	i2c_set_slave_device_addr(i2c_bus, DCDC_addr_2);
	pmbus_on(i2c_bus, ERICSSON_SAFE_BIG_DELAY);
}

static bool get_good_cores(int asic, int *result, bool *bad_dies)
{
	(void)asic;
	(void)bad_dies;
	*result = 0;
	return false;
}

/*
 * only_this_die - enable/disable only this die. If < 0 then enable/disable all dies
 */
static bool enable_all_cores(int asic, bool ena, int wait_us, int only_this_die)
{
	(void)asic;
	(void)ena;
	(void)wait_us;
	(void)only_this_die;
	return false;
}

static int do_auto_PLL(struct device_t *dev,
		       unsigned int minimum_PLL_change_interval,
		       float temp_ok_low,
		       float temp_ok_high,
		       unsigned int freq_step_up,
		       unsigned int freq_step_down)
{
	int i2c_bus;
	int asic, die, dcdc, num_dcdc;
	float temp;
	int freq_step;
	time_t prev_PLL_change = 0;
	struct timespec curtime = {0,0};
	FILE *f;
	float normal_number_of_cores[KNC_MAX_ASICS] = {0};
	int cur_good_cores[KNC_MAX_ASICS] = {0};
	int bad_freq[KNC_MAX_ASICS];
	bool freq_changed = false;

	for (asic = 0; asic < KNC_MAX_ASICS; ++asic)
		bad_freq[asic] = MAX_DIE_FREQ_JUPITER_REVB * 2;

#ifdef DEBUG_INFO
	printf("[%lu] AUTOPLL: int=%u, T_low=%f, T_high=%f, F_up=%u, F_down=%u\n", (unsigned long)time(NULL), minimum_PLL_change_interval, temp_ok_low, temp_ok_high, freq_step_up, freq_step_down);
#endif /* DEBUG_INFO */

	f = fopen(WAAS_AUTOPLL_TIMESTAMP, "r");
	if (NULL != f) {
		(void)fscanf(f, "%lu %f %f %f %f %f %f %i %i %i %i %i %i", &prev_PLL_change,
		       &(normal_number_of_cores[0]), &(normal_number_of_cores[1]),
		       &(normal_number_of_cores[2]), &(normal_number_of_cores[3]),
		       &(normal_number_of_cores[4]), &(normal_number_of_cores[5]),
		       &(bad_freq[0]), &(bad_freq[1]), &(bad_freq[2]),
		       &(bad_freq[3]), &(bad_freq[4]), &(bad_freq[5]) );
		fclose(f);
	}

	clock_gettime(CLOCK_MONOTONIC, &curtime);
#ifdef DEBUG_INFO
	printf("[%lu] AUTOPLL: prev tstamp %lu, cur tstamp %lu, diff %f\n", (unsigned long)time(NULL), prev_PLL_change, curtime.tv_sec, difftime(curtime.tv_sec, prev_PLL_change));
#endif /* DEBUG_INFO */
	if (minimum_PLL_change_interval >= difftime(curtime.tv_sec, prev_PLL_change))
		return 0;

	for (asic = 0; asic < KNC_MAX_ASICS; ++asic) {
		int data = -1;
		int good_dies[KNC_MAX_DIES_PER_ASIC] = {0};
		bool bad_dies[KNC_MAX_DIES_PER_ASIC], many_failed_cores = false;
		asic_board_t *board = &(dev->boards[asic]);
		dcdc_mfrid_t board_mfrid = mfrid_from_board_type(board->type);
		/* Only Ericsson devices have temp sensors */
		if (MFRID_ERICSSON != board_mfrid)
			continue;
		if (0 > (i2c_bus = i2c_connect(FIRST_ASIC_I2C_BUS + asic)))
			continue;

		/* Get the average temperature of DC/DCs */
		num_dcdc = 0;
		temp = 0;
		for (dcdc = 0; dcdc < KNC_MAX_DCDC_DEVICES; ++dcdc) {
			float cur_iout, cur_temp; 
			i2c_set_slave_device_addr(i2c_bus, DCDC_ADDR(dcdc));
			if (MFRID_ERICSSON != pmbus_get_dcdc_mfr_id(i2c_bus, ERICSSON_I2C_WORKAROUND_DELAY_us)) {
#ifdef DEBUG_INFO
				printf("[%lu] AUTOPLL: ASIC[%d] DCDC[%d] doesn't answer\n", (unsigned long)time(NULL), asic, dcdc);
#endif /* DEBUG_INFO */
				continue;
			}
			/* If die is dead and not consuming power - don't count that dcdc */
			cur_iout = pmbus_read_iout_ericsson(i2c_bus);
#ifdef DEBUG_INFO
			printf("[%lu] AUTOPLL: ASIC[%d] DCDC[%d] Iout = %f\n", (unsigned long)time(NULL), asic, dcdc, cur_iout);
#endif /* DEBUG_INFO */
			if (IOUT_NORMAL_LEVEL > cur_iout)
				continue;
			++num_dcdc;
			cur_temp = pmbus_read_temp_ericsson(i2c_bus);
#ifdef DEBUG_INFO
			printf("[%lu] AUTOPLL: ASIC[%d] DCDC[%d] Temp = %f\n", (unsigned long)time(NULL), asic, dcdc, cur_temp);
#endif /* DEBUG_INFO */
			temp += cur_temp;
			++(good_dies[dcdc / 2]);
		}
		i2c_disconnect(i2c_bus);
		for (die = 0; die < KNC_MAX_DIES_PER_ASIC; ++die)
			bad_dies[die] = (0 == good_dies[die]);

		/* Get the frequency adjustment */
		if (0 == num_dcdc)
			temp = 0;
		else
			temp = (temp / num_dcdc);
		printf("[%lu] AUTOPLL: ASIC[%d] temp = %f\n", (unsigned long)time(NULL), asic, temp);
		get_good_cores(asic, &(cur_good_cores[asic]), bad_dies);
		/* Adjust Normal number slightly */
		normal_number_of_cores[asic] -= AUTOPLL_NORMALCORES_ALPHA * (normal_number_of_cores[asic] - cur_good_cores[asic]);
		printf("[%lu] AUTOPLL: ASIC[%d] normal_cores=%f, cur_good_cores=%i\n", (unsigned long)time(NULL), asic, normal_number_of_cores[asic], cur_good_cores[asic]);
		freq_step = 0;
		if (temp > temp_ok_high) {
			freq_step = -freq_step_down;
		} else {
			if (0 == cur_good_cores[asic]) {
				/* Something is terribly wrong. Decrease the frequency! */
				freq_step = -freq_step_down;
			} else if (cur_good_cores[asic] >= normal_number_of_cores[asic]) {
				if (temp < temp_ok_low)
					freq_step = freq_step_up;
			} else if (cur_good_cores[asic] < (normal_number_of_cores[asic] - AUTOPLL_GOOD_CORES_TOLERANCE)) {
				/* The number of good cores decreased significantly! */
				freq_step = -freq_step_down;
			}
		}
		if (cur_good_cores[asic] < (normal_number_of_cores[asic] - AUTOPLL_GOOD_CORES_TOLERANCE))
			many_failed_cores = true;

		/* Set new PLL frequency */
		/* We want to set the same frequency for all dies. So, read the current PLL
		 * frequency from die 0 and use it as a reference point for all dies.
		 */
		die = 0;
		data = read_die_freq(asic, die);
		int die_freq[KNC_MAX_DIES_PER_ASIC];
		for (die = 0; die < KNC_MAX_DIES_PER_ASIC; ++die)
			die_freq[die] = data;

		/* Update bad frequency */
		if (many_failed_cores) {
			bad_freq[asic] = data - freq_step_down + 1;
		} else {
			/* Age out detected bad frequency, but slowly */
			++(bad_freq[asic]);
		}
		if ((data + freq_step) >= bad_freq[asic]) {
			if (data >= bad_freq[asic])
				freq_step = (bad_freq[asic] - freq_step_down) - data;
			else
				freq_step = 0;
		}

		/* Normalize freq_step */
		freq_step = nearest_valid_asic_freq(board->type, data + freq_step) - data;
		if (0 != freq_step) {
			data += freq_step;
			for (die = 0; die < KNC_MAX_DIES_PER_ASIC; ++die)
				die_freq[die] = data;
			for (die = 0; die < KNC_MAX_DIES_PER_ASIC; ++die) {
				enable_all_cores(asic, false, 0, die); /* disable al cores */
				sleep(10);  /* wait for current works to finish */
				asic_set_die_frequencies(board, die_freq, die, die);
				enable_all_cores(asic, true, 50000, die); /* enable al cores */
			}
			freq_changed = true;
		}
		printf("[%lu] AUTOPLL: ASIC[%d] new PLL freq = %d [%d]\n", (unsigned long)time(NULL), asic, data, freq_step);
		/* Restart all dies which are down */
		if (0 > (i2c_bus = i2c_connect(FIRST_ASIC_I2C_BUS + asic)))
			continue;
		for (die = 0; die < KNC_MAX_DIES_PER_ASIC; ++die) {
			float iout1, iout2 = 0;
			int dcdc1 = die * 2 + 0;
			int dcdc2 = die * 2 + 1;
			i2c_set_slave_device_addr(i2c_bus, DCDC_ADDR(dcdc1));
			iout1 = pmbus_read_iout_ericsson(i2c_bus);
			if (IOUT_NORMAL_LEVEL <= iout1) {
				i2c_set_slave_device_addr(i2c_bus, DCDC_ADDR(dcdc2));
				iout2 = pmbus_read_iout_ericsson(i2c_bus);
			}
			if ((IOUT_NORMAL_LEVEL > iout1) || (IOUT_NORMAL_LEVEL > iout2)) {
#ifdef DEBUG_INFO
				printf("[%lu] AUTOPLL: ASIC[%d] DIE[%d] reset (Iout.0 = %f; Iout.1 = %f)\n", (unsigned long)time(NULL), asic, die, iout1, iout2);
#endif /* DEBUG_INFO */
				reset_pair_of_ericsson_dcdcs(i2c_bus, DCDC_ADDR(dcdc1), DCDC_ADDR(dcdc2));
				usleep(ERICSSON_SAFE_BIG_DELAY);
				asic_set_die_frequencies(board, die_freq, die, die);
				sleep(20);
			}
		}
		i2c_disconnect(i2c_bus);
	}

	if (freq_changed) {
		struct advanced_config running_settings;
		read_running_settings(&running_settings, dev, false, 0, KNC_MAX_ASICS - 1, 0, KNC_MAX_DIES_PER_ASIC - 1);
		write_expected_performance(&running_settings, dev);
		clock_gettime(CLOCK_MONOTONIC, &curtime);
		prev_PLL_change = curtime.tv_sec;
	}

	f = fopen(WAAS_AUTOPLL_TIMESTAMP, "w");
	if (NULL != f) {
		fprintf(f, "%lu %f %f %f %f %f %f %i %i %i %i %i %i", prev_PLL_change,
			normal_number_of_cores[0], normal_number_of_cores[1],
			normal_number_of_cores[2], normal_number_of_cores[3],
			normal_number_of_cores[4], normal_number_of_cores[5],
			bad_freq[0], bad_freq[1], bad_freq[2],
			bad_freq[3], bad_freq[4], bad_freq[5] );
		fclose(f);
	}
	return 0;
}

int main(int argc, char *argv[])
{
	int opt, ret = 0, parse_config_res;
	char config_file_name[PATH_MAX];
	struct advanced_config running_settings, new_settings;
	int asic, die;
	FILE *f = stdout;
	bool use_defaults = false, read_only = false;
	bool print_running_info = false;
	bool do_auto_pll_adj = false;
	int i, a;
	struct device_t dev;
	dcdc_mfrid_t dev_mfrid;
	int single_asic = -1, single_die = -1;

	strcpy(config_file_name, DEFAULT_CONFIG_FILE);

	detect_device_type(&dev, true);
	dev_mfrid = mfrid_from_board_type(dev.dev_type);

	while (-1 != (opt = getopt(argc, argv, "c:i:o:g:s:dra"))) {
		switch (opt) {
		case 'c':
			strncpy(config_file_name, optarg,
				sizeof(config_file_name) - 1);
			config_file_name[sizeof(config_file_name) - 1] = '\0';
			break;
		case 'o':
			f = fopen(optarg, "w");
			if (NULL == f) {
				fprintf(stderr,
					"Can not open output file %s: %m\n",
					optarg);
				return -5;
			}
			break;
		case 'i':
			if (0 == strcmp(optarg, "valid-die-voltages")) {
				int offs_i;
				float offs_f;
				int start = MIN_DCDC_VOUT_TRIM;
				int end = MAX_DCDC_VOUT_TRIM;
				int step = DCDC_VOUT_TRIM_STEP;
				if (MFRID_ERICSSON == dev_mfrid) {
					start *= DCDC_VOLTAGE_OFFSET_ERICSSON_MULTIPLIER;
					end *= DCDC_VOLTAGE_OFFSET_ERICSSON_MULTIPLIER;
					step *= DCDC_VOLTAGE_OFFSET_ERICSSON_MULTIPLIER;
				}
				for (offs_i = start; offs_i <= end; offs_i += step) {
					offs_f =
					    dcdc_voltage_from_offset(dev_mfrid, offs_i);
					printf("%1.4f\n", offs_f);
				}
				return 0;
			}
			if (0 == strcmp(optarg, "valid-die-frequencies")) {
				int freq;
				int start = 0;
				int end = 0;
				int step = DIE_FREQ_STEP;
				switch (dev.dev_type) {
				case ASIC_BOARD_TITAN:
					start = MIN_DIE_FREQ_TITAN;
					end = MAX_DIE_FREQ_TITAN;
					break;
				case ASIC_BOARD_NEPTUNE:
					start = MIN_DIE_FREQ_NEPTUNE;
					end = MAX_DIE_FREQ_NEPTUNE;
					break;
				case ASIC_BOARD_JUPITER_REVB:
				case ASIC_BOARD_JUPITER_ERICSSON:
					start = MIN_DIE_FREQ_JUPITER_REVB;
					end = MAX_DIE_FREQ_JUPITER_REVB;
					break;
				case ASIC_BOARD_JUPITER_REVA:
					start = MIN_DIE_FREQ_JUPITER_REVA;
					end = MAX_DIE_FREQ_JUPITER_REVB;
					break;
				default:
					break;
				}
				printf("OFF\n");
				for (freq = start; freq <= end; freq += step) {
					printf("%d\n", freq);
				}
				return 0;
			}

			if (0 == strcmp(optarg, "valid-ranges")) {
				int offs_i;
				float offs_f;
				int freq;

				/* valid-die-voltages */
				printf("\"valid_die_voltages\" : \n[\n");
				int start = MIN_DCDC_VOUT_TRIM;
				int end = MAX_DCDC_VOUT_TRIM;
				int step = DCDC_VOUT_TRIM_STEP;
				if (MFRID_ERICSSON == dev_mfrid) {
					start *= DCDC_VOLTAGE_OFFSET_ERICSSON_MULTIPLIER;
					end *= DCDC_VOLTAGE_OFFSET_ERICSSON_MULTIPLIER;
					step *= DCDC_VOLTAGE_OFFSET_ERICSSON_MULTIPLIER;
				}
				for (offs_i = start; offs_i <= end; offs_i += step) {
					offs_f =
					    dcdc_voltage_from_offset(dev_mfrid, offs_i);
					if (offs_i < end)
						printf("\"%1.4f\",\n", offs_f);
					else
						printf("\"%1.4f\"\n],\n", offs_f);
				}

				/* valid-die-frequencies */
				printf("\"valid_die_frequencies\" : \n[\n");
				step = DIE_FREQ_STEP;
				switch (dev.dev_type) {
				case ASIC_BOARD_TITAN:
					start = MIN_DIE_FREQ_TITAN;
					end = MAX_DIE_FREQ_TITAN;
					break;
				case ASIC_BOARD_NEPTUNE:
					start = MIN_DIE_FREQ_NEPTUNE;
					end = MAX_DIE_FREQ_NEPTUNE;
					break;
				case ASIC_BOARD_JUPITER_REVB:
				case ASIC_BOARD_JUPITER_ERICSSON:
					start = MIN_DIE_FREQ_JUPITER_REVB;
					end = MAX_DIE_FREQ_JUPITER_REVB;
					break;
				case ASIC_BOARD_JUPITER_REVA:
					start = MIN_DIE_FREQ_JUPITER_REVA;
					end = MAX_DIE_FREQ_JUPITER_REVA;
					break;
				default:
					start = 0;
					end = 0;
					break;
				}
				printf("\"OFF\",\n");
				for (freq = start; freq <= end; freq += step) {
					if (freq < end)
						printf("\"%d\",\n", freq);
					else
						printf("\"%d\"\n]", freq);					  
				}

				return 0;
			}

			fprintf(stderr, "Unknown info requested: %s\n", optarg);
			return -4;
		case 'g':
			if (0 == strcmp(optarg, "all-asic-info")) {
				print_running_info = true;
				break;
			}

			fprintf(stderr, "Unknown running info requested: %s\n",
				optarg);
			return -6;
		case 's':
			if (2 != sscanf(optarg, "%d:%d", &single_asic, &single_die)) {
				fprintf(stderr, "Wrong argument for \'s\' option: %s\n", optarg);
				return -7;
			}
			break;
		case 'd':
			use_defaults = true;
			break;
		case 'r':
			read_only = true;
			break;
		case 'a':
			do_auto_pll_adj = true;
			break;
		}
	}

	int start_asic, end_asic, start_die, end_die;
	if ((0 <= single_asic) && (KNC_MAX_ASICS > single_asic)) {
		start_asic = single_asic;
		end_asic = single_asic;
		if ((0 <= single_die) && (KNC_MAX_DIES_PER_ASIC > single_die)) {
			start_die = single_die;
			end_die = single_die;
		} else {
			start_die = 0;
			end_die = KNC_MAX_DIES_PER_ASIC - 1;
		}
	} else {
		start_asic = 0;
		end_asic = KNC_MAX_ASICS - 1;
		start_die = 0;
		end_die = KNC_MAX_DIES_PER_ASIC - 1;
	}

	if (do_auto_pll_adj) {
		unsigned int autopll_param_minimum_change_interval = AUTOPLL_MINIMUM_CHANGE_INTERVAL;
		float autopll_param_temp_ok_low = AUTOPLL_TEMP_OK_LOW_LEVEL;
		float autopll_param_temp_ok_high = AUTOPLL_TEMP_OK_HIGH_LEVEL;
		unsigned int autopll_param_freq_step_up = AUTOPLL_FREQ_STEP_UP;
		unsigned int autopll_param_freq_step_down = AUTOPLL_FREQ_STEP_DOWN;
		for (i = optind; i < argc; ++i) {
			a = i - optind;
			switch (a) {
			case 0:
				sscanf(argv[i], "%u", &autopll_param_minimum_change_interval);
				break;
			case 1:
				sscanf(argv[i], "%f", &autopll_param_temp_ok_low);
				break;
			case 2:
				sscanf(argv[i], "%f", &autopll_param_temp_ok_high);
				break;
			case 3:
				sscanf(argv[i], "%u", &autopll_param_freq_step_up);
				break;
			case 4:
				sscanf(argv[i], "%u", &autopll_param_freq_step_down);
				break;
			}
		}
		ret = do_auto_PLL(&dev,
			autopll_param_minimum_change_interval,
			autopll_param_temp_ok_low,
			autopll_param_temp_ok_high,
			autopll_param_freq_step_up,
			autopll_param_freq_step_down);
		if (print_running_info)
			ret = do_print_running_info(f, &dev, start_asic, end_asic, start_die, end_die);
		return ret;
	}

	if (print_running_info) {
		ret = do_print_running_info(f, &dev, start_asic, end_asic, start_die, end_die);
		return ret;
	}

	read_running_settings(&running_settings, &dev, use_defaults, start_asic, end_asic, start_die, end_die);

	if (!read_only) {
		memcpy(&new_settings, &running_settings, sizeof(new_settings));
		if (!use_defaults) {
			parse_config_res = parse_config_file(config_file_name, &new_settings, &dev);
			if (0 != parse_config_res)
				return parse_config_res;
		}
		implement_settings(&new_settings, &dev, start_asic, end_asic, start_die, end_die);
		read_running_settings(&running_settings, &dev, false, start_asic, end_asic, start_die, end_die);
	}

	write_expected_performance(&running_settings, &dev);

	/* Print out running settings in JSON format */
	fprintf(f, "{\n");
	for (asic = start_asic; asic <= end_asic; ++asic)
	{
		asic_board_t *board = &(dev.boards[asic]);
		dcdc_mfrid_t board_mfrid = mfrid_from_board_type(board->type);
		fprintf(f, "\"asic_%d_voltage\": {\n", asic + 1);
		for (die = start_die; die <= end_die; ++die) {
			int16_t data =
				running_settings.dcdc_V_offset[asic][die];
			int start = MIN_DCDC_VOUT_TRIM;
			int end = MAX_DCDC_VOUT_TRIM;
			if (MFRID_ERICSSON == board_mfrid) {
				start *= DCDC_VOLTAGE_OFFSET_ERICSSON_MULTIPLIER;
				end *= DCDC_VOLTAGE_OFFSET_ERICSSON_MULTIPLIER;
			}
			if ((data >= start) && (data <= end)) {
				if (die < end_die) {
					fprintf(f, "\t\"die%d\": \"%1.4f\",\n",
						die + 1,
						dcdc_voltage_from_offset(board_mfrid, data));
				} else {
					fprintf(f, "\t\"die%d\": \"%1.4f\"\n",
						die + 1,
						dcdc_voltage_from_offset(board_mfrid, data));
				}
			} else {
				if (die < end_die)
					fprintf(f, "\t\"die%d\": \"\",\n", die + 1);
				else 
					fprintf(f, "\t\"die%d\": \"\"\n", die + 1);
			}
		}
		fprintf(f, "},\n");
		fprintf(f, "\"asic_%d_frequency\": {\n", asic + 1);
		for (die = start_die; die <= end_die; ++die) {
			int data = running_settings.die_freq[asic][die];
			if ((data >= dev.freq_start[asic]) && (data <= dev.freq_end[asic])) {
				if (die < end_die) {
					fprintf(f, "\t\"die%d\": \"%d\",\n",
						die + 1, data);
				} else {
					fprintf(f, "\t\"die%d\": \"%d\"\n",
						die + 1, data);
				}
			} else {
				if (die < end_die)
					fprintf(f, "\t\"die%d\": \"\",\n", die + 1);
				else 
					fprintf(f, "\t\"die%d\": \"\"\n", die + 1);
			}
		}
		if (asic < end_asic)
			fprintf(f, "},\n");
		else
			fprintf(f, "}\n");
	}
	fprintf(f, "}\n");
	return ret;
}
