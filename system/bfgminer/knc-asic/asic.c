#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "knc-asic.h"

#include "knc-transport.h"

#include <logging.h>

#define UNUSED __attribute__((unused))

struct knc_die_info die_info;

#define chip_version (die_info.version)

static void do_halt_(void *ctx, int channel, int die, int core);

static void detect_chip(void *ctx, int channel, int die)
{
	if (chip_version != KNC_VERSION_UNKNOWN)
		return;

	if (knc_detect_die(ctx, channel, die, &die_info) != 0) {
		printf("ERROR: No asic found\n");
		exit(1);
	}
	chip_version = die_info.version;
}

static void do_info(void *ctx, int channel, int die, UNUSED int argc, UNUSED char **args)
{
	if (knc_detect_die(ctx, channel, die, &die_info) != 0) {
		printf("ERROR: No asic found\n");
		exit(1);
	}

	printf("Version: %s\n", get_asicname_from_version(die_info.version));
	printf("Cores: %d\n", die_info.cores);
	if (die_info.pll_power_down >= 0) printf("PLL power: %s\n", die_info.pll_power_down ? "DOWN" : "UP");
	if (die_info.pll_locked >= 0)     printf("PLL status: %s\n", die_info.pll_locked ? "GOOD" : "BAD");
	if (die_info.hash_reset_n >= 0)   printf("HASH reset: %s\n", die_info.hash_reset_n ? "NORMAL" : "RESET");
	if (die_info.pll_reset_n >= 0)    printf("PLL reset: %s\n", die_info.pll_reset_n ? "NORMAL" : "RESET");
	printf("Want work: ");
	int core;
	for (core = 0; core < die_info.cores; core++) {
		putchar(die_info.want_work[core] == 0 ? (die_info.has_report[core] == 1 ? 'o' : '.') :
		        die_info.want_work[core] == 1 ? (die_info.has_report[core] == 1 ? '0' : '*') :
		        '?');
	}
	putchar('\n');
	chip_version = die_info.version;
}

static int hex_decode(uint8_t *dst, const char *src, size_t max_len)
{
	size_t len = 0;
	int dir = 1;
	memset(dst, 0, max_len);
	if (strncmp(src, "0x", 2) == 0 || strncmp(src, "0X", 2) == 0) {
		src += 2;
		dst = dst + max_len - 1;
		dir = -1;
	}
	while (*src && len < max_len) {
		char octet[3] = {'0','0', 0};
		octet[1] = *src++;
		if (*src) {
			octet[0] = octet[1];
			octet[1] = *src++;
		}
		*dst = strtoul(octet, NULL, 16);
		dst += dir;
		len++;
	}
	return len;
}

static void handle_report(uint8_t *response)
{
	struct knc_report report;
	int nonces = 0;
	knc_decode_report(response, &report, chip_version);

	switch(chip_version) {
	case KNC_VERSION_JUPITER:
		nonces = 1;
		printf("Next    : %s\n", report.next_state ? "LOADED" : "FREE");
		printf("Current : 0x%x\n", report.active_slot);
		break;
	case KNC_VERSION_NEPTUNE:
	case KNC_VERSION_TITAN:
		nonces = 5;
		printf("Next    : 0x%x %s\n", report.next_slot, report.next_state ? "LOADED" : "FREE");
		printf("Current : 0x%x %s\n", report.active_slot, report.state ? "HASHING" : "IDLE");
		break;
	case KNC_VERSION_UNKNOWN:
		break; /* To keep GCC happy */
	}
	printf("Progress: 0x%08x\n", report.progress);
	int n;
	for (n = 0; n < nonces; n++) {
		printf("Nonce %d : 0x%x %08x\n", n, report.nonce[n].slot, report.nonce[n].nonce);
	}
}

static void do_setwork(void *ctx, int channel, int die, UNUSED int argc, char **args)
{
	uint8_t midstate[8*4];
	uint8_t data[BLOCK_HEADER_BYTES_WITHOUT_NONCE];
	int request_length = 4 + 1 + BLOCK_HEADER_BYTES_WITHOUT_NONCE;
	uint8_t request[request_length];
	int response_length = 1 + 1 + (1 + 4) * 5;
	uint8_t response[response_length];
	struct work work = {
		.midstate = midstate,
		.data = data
	};

	int core = strtoul(*args++, NULL, 0);
	int slot = strtoul(*args++, NULL, 0);
	int clean = strtoul(*args++, NULL, 0);
	memset(data, 0, sizeof(data));

	detect_chip(ctx, channel, die);

	switch(chip_version) {
	case KNC_VERSION_JUPITER:
		hex_decode(midstate, *args++, sizeof(midstate));
		hex_decode(data+16*4, *args++, sizeof(data)-16*4);

		if (clean) {
			/* Double halt to get rid of any previous queued work */
			request_length = knc_prepare_jupiter_halt(request, die, core);
			knc_syncronous_transfer(ctx, channel, request_length, request, 0, NULL);
			knc_syncronous_transfer(ctx, channel, request_length, request, 0, NULL);
		}
		request_length = knc_prepare_jupiter_setwork(request, die, core, slot, &work);
		knc_syncronous_transfer(ctx, channel, request_length, request, 1, response);
		if (response[0] == 0x7f)
			applog(LOG_ERR, "KnC %d-%d: Core disabled", channel, die);
		break;
	case KNC_VERSION_NEPTUNE:
	case KNC_VERSION_TITAN:
		if (KNC_VERSION_NEPTUNE == chip_version) {
			hex_decode(midstate, *args++, sizeof(midstate));
			hex_decode(data+16*4, *args++, sizeof(data)-16*4);

			request_length = knc_prepare_neptune_setwork(request, die, core, slot, &work, clean);
		} else {
			/* KNC_VERSION_TITAN */
			++args; /* ignore midstate */
			hex_decode(data, *args++, sizeof(data));

			request_length = knc_prepare_titan_setwork(request, die, core, slot, &work, clean);
		}

		int status = knc_syncronous_transfer(ctx, channel, request_length, request, response_length, response);
		if (status != KNC_ACCEPTED) {
			if (response[0] == 0x7f) {
				applog(LOG_ERR, "KnC %d-%d: Core disabled", channel, die);
				return;
			}
			if (status & KNC_ERR_MASK) {
				applog(LOG_ERR, "KnC %d-%d: Failed to set work state (%x)", channel, die, status);
				return;
			}
			if (!(status & KNC_ERR_MASK)) {
				/* !KNC_ERRMASK */
				applog(LOG_ERR, "KnC %d-%d: Core busy", channel, die);
			}
		}
		handle_report(response);
		break;
	case KNC_VERSION_UNKNOWN:
		break; /* To keep GCC happy */
	}
}

static void do_report(void *ctx, int channel, int die, UNUSED int argc, char **args)
{
	int core = strtoul(*args++, NULL, 0);
	uint8_t request[4];
	int request_length;
	int response_length = 1 + 1 + (1 + 4) * 5;
	uint8_t response[response_length];
	int status;

	detect_chip(ctx, channel, die);

	request_length = knc_prepare_report(request, die, core);

	switch(chip_version) {
	case KNC_VERSION_JUPITER:
		response_length = 1 + 1 + (1 + 4);
		knc_syncronous_transfer(ctx, channel, request_length, request, response_length, response);
		break;
	case KNC_VERSION_NEPTUNE:
	case KNC_VERSION_TITAN:
		status = knc_syncronous_transfer(ctx, channel, request_length, request, response_length, response);
		if (status) {
			applog(LOG_ERR, "KnC %d-%d: Failed (%x)", channel, die, status);
			return;
		}
	case KNC_VERSION_UNKNOWN:
		break; /* To keep GCC happy */
	}
	handle_report(response);
}

static void do_titan_setup(void *ctx, int channel, int die, UNUSED int argc, char **args)
{
#define	DEFAULT_DIFF_FILTERING_ZEROES	12
	struct titan_setup_core_params setup_params = {
		.bad_address_mask = {0, 0},
		.bad_address_match = {0x3FF, 0x3FF},
		.difficulty = DEFAULT_DIFF_FILTERING_ZEROES,
		.thread_enable = 0xFF,
		.thread_base_address = {0, 1, 2, 3, 4, 5, 6, 7},
		.lookup_gap_mask = {0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7},
		.N_mask = {0, 0, 0, 0, 0, 0, 0, 0},
		.N_shift = {0, 0, 0, 0, 0, 0, 0, 0},
		.nonce_bottom = 0,
		.nonce_top = 0xFFFFFFFF,
	};

	int core = strtoul(*args++, NULL, 0);
	int num_threads = strtoul(*args++, NULL, 0);
	setup_params.nonce_bottom = strtoul(*args++, NULL, 0);
	setup_params.nonce_top = strtoul(*args++, NULL, 0);

	if (!fill_in_thread_params(num_threads, &setup_params))
		return;

	detect_chip(ctx, channel, die);

	switch(chip_version) {
	case KNC_VERSION_JUPITER:
	case KNC_VERSION_NEPTUNE:
		applog(LOG_ERR, "This command is for Titan only!");
		return;
	case KNC_VERSION_TITAN:
		do_halt_(ctx, channel, die, core);
		knc_titan_setup_core(ctx, channel, die, core, &setup_params);
		break;
	case KNC_VERSION_UNKNOWN:
		break; /* To keep GCC happy */
	}
}

static void do_halt_(void *ctx, int channel, int die, int core)
{
	int request_length = 4 + 1 + BLOCK_HEADER_BYTES_WITHOUT_NONCE;
	uint8_t request[request_length];
	int response_length = 1 + 1 + (1 + 4) * 5;
	uint8_t response[response_length];
	int status;

	detect_chip(ctx, channel, die);

	switch(chip_version) {
	case KNC_VERSION_JUPITER:
		request_length = knc_prepare_jupiter_setwork(request, die, core, 0, NULL);
		knc_syncronous_transfer(ctx, channel, request_length, request, 0, NULL);
		request_length = knc_prepare_jupiter_halt(request, die, core);
		knc_syncronous_transfer(ctx, channel, request_length, request, 0, NULL);
		request_length = knc_prepare_jupiter_halt(request, die, core);
		knc_syncronous_transfer(ctx, channel, request_length, request, 0, NULL);
		break;
	case KNC_VERSION_NEPTUNE:
	case KNC_VERSION_TITAN:
		if (KNC_VERSION_NEPTUNE == chip_version) {
			request_length = knc_prepare_neptune_halt(request, die, core);
		} else {
			/* KNC_VERSION_TITAN */
			request_length = knc_prepare_titan_halt(request, die, core);
		}
		status = knc_syncronous_transfer(ctx, channel, request_length, request, response_length, response);
		if (status != KNC_ACCEPTED) {
			if (response[0] == 0x7f) {
				applog(LOG_ERR, "KnC %d-%d: Core disabled", channel, die);
				return;
			}
			if (status & KNC_ERR_MASK) {
				applog(LOG_ERR, "KnC %d-%d: Failed to set work state (%x)", channel, die, status);
				return;
			}
		}
		break;
	case KNC_VERSION_UNKNOWN:
		break; /* To keep GCC happy */
	}
}

static inline void do_halt(void *ctx, int channel, int die, UNUSED int argc, char **args)
{
	do_halt_(ctx, channel, die, strtoul(*args, NULL, 0));
}

static void do_raw(void *ctx, int channel, int die, UNUSED int argc, char **args)
{
	uint8_t response[256];
	uint8_t request[256];

	int response_length = strtoul(*args++, NULL, 0);
	int request_length = hex_decode(request, *args++, sizeof(request));

	int status = knc_syncronous_transfer(ctx, channel, request_length, request, response_length, response);
	applog(LOG_ERR, "KnC %d-%d: STATUS=%x\n", channel, die, status);
}

static void do_freq(void *ctx, int channel, int die, UNUSED int argc, char **args)
{
	uint8_t request[2048];
	uint8_t response[2048];

	int freq = strtoul(*args++, NULL, 0);

	int len = knc_prepare_freq(request, 0, sizeof(request), channel, die, freq);

	knc_trnsp_transfer(ctx, request, response, len);

	freq = knc_decode_freq(response);

	if (freq >= 0)
		applog(LOG_NOTICE, "KnC %d-%d: Frequency change successful, FREQ=%d", channel, die, freq);
	else
		applog(LOG_ERR, "KnC %d-%d: Frequency change FAILED!", channel, die);
}

/* TODO: Move bits to knc-asic.[ch] */
static void do_status(void *ctx, UNUSED int argc, char **args)
{
	uint8_t request[1024];
	uint8_t response[1024];
	struct knc_spimux_status status;

	int channel = atoi(*args++); argc--;

	int len = knc_prepare_status(request, 0, sizeof(request), channel);

	knc_trnsp_transfer(ctx, request, response, len);

	if (knc_decode_status(response, &status) < 0) {
		applog(LOG_ERR, "KnC %d: No controller found", channel);
		return;
	}

	printf("FPGA Version : %s\n", status.revision);
	printf("Board Type   : %02X\n", status.board_type);
	printf("Board Rev    : %02X\n", status.board_rev);
	printf("Core map     : ");
	int cores = 0;
	int die, core;
	for (die = 0; die < KNC_MAX_DIES_PER_ASIC; die++) {
		for (core = 0; core < KNC_MAX_CORES_PER_DIE; core++) {
			if (status.core_status[die][core]) {
				printf("+");
				cores++;
			} else {
				printf("-");
			}
		}
	}
	printf("\n");
	printf("Cores       : %d\n", cores);
}

static void do_reset(void *ctx, UNUSED int argc, UNUSED char **args)
{
	uint8_t request[256];
	uint8_t response[256];

	int len = knc_prepare_reset(request, 0, sizeof(request));

	knc_trnsp_transfer(ctx, request, response, len);

}

static void do_led(void *ctx, UNUSED int arg, char **args)
{
	uint8_t request[256];
	uint8_t response[256];

	uint32_t red = strtoul(*args++, NULL, 0);
	uint32_t green = strtoul(*args++, NULL, 0);
	uint32_t blue = strtoul(*args++, NULL, 0);

	int len = knc_prepare_led(request, 0, sizeof(request), red, green, blue);

	knc_trnsp_transfer(ctx, request, response, len);
}

struct knc_asic_command {
	const char *name;
	const char *args;
	const char *description;
	int nargs;
	void (*handler)(void *ctx, int channel, int die, int nargs, char **args);
} knc_asic_commands[] = {
	{"info", "", "ASIC version & info", 0, do_info},
	{"setwork", "core slot(1-15) clean(0/1) midstate data", "Set work vector (for Titan midstate is ignored)", 5, do_setwork},
	{"report", "core", "Get nonce report", 1, do_report},
	{"halt", "core", "Halt core", 1, do_halt},
	{"freq", "frequency", "Set core frequency", 1, do_freq},
	{"titan", "core num_threads nonce_bottom nonce_top", "Setup Titan core", 4, do_titan_setup},
	{"raw", "response_length request_data", "Send raw ASIC request", 2, do_raw},
	{NULL, NULL, NULL, 0, NULL}
};

struct knc_ctrl_command {
	const char *name;
	const char *args;
	const char *description;
	int nargs;
	void (*handler)(void *ctx, int nargs, char **args);
} knc_ctrl_commands[] = {
	{"status", "channel", "ASIC status", 1, do_status},
	{"led", "red green blue", "Controller LED color", 3, do_led},
	{"reset", "", "Reset controller", 0, do_reset},
	{NULL, NULL, NULL, 0, NULL}
};

void usage(char *program_name)
{
	fprintf(stderr, "Usage: %s command arguments..\n", program_name);
	{
		struct knc_ctrl_command *cmd;
		for (cmd = knc_ctrl_commands; cmd->name; cmd++)
			fprintf(stderr, "  %s %s\n	%s\n", cmd->name, cmd->args, cmd->description);
	}
	{
		struct knc_asic_command *cmd;
		for (cmd = knc_asic_commands; cmd->name; cmd++)
			fprintf(stderr, "  %s channel die %s\n	%s\n", cmd->name, cmd->args, cmd->description);
	}
}
int main(int argc, char **argv)
{
	int channel, die;
	char *command;
	char **args = &argv[1];
	const char *devname = NULL;
	
	for (;argc > 1 && *args[0] == '-'; argc--, args++) {
		if (strcmp(*args, "-n") == 0) {
			die_info.version = KNC_VERSION_NEPTUNE;
			die_info.cores = KNC_CORES_PER_DIE_NEPTUNE;
			continue;
		}
		if (strcmp(*args, "-t") == 0) {
			die_info.version = KNC_VERSION_TITAN;
			die_info.cores = KNC_CORES_PER_DIE_TITAN;
			continue;
		}
		if (strcmp(*args, "-c") == 0 && argc > 2)  {
			die_info.cores = atoi(args[1]);
			argc--;
			args++;
			continue;
		}
		if (strcmp(*args, "-j") == 0) {
			die_info.version = KNC_VERSION_JUPITER;
			die_info.cores = KNC_CORES_PER_DIE_JUPITER;
			continue;
		}
		if (strcmp(*args, "-i") == 0 && argc > 2)  {
			devname = args[1];
			argc--;
			args++;
			continue;
		}
		if (strcmp(*args, "-d") == 0)
			debug_level = LOG_DEBUG;
	}
	argc--;
	if (argc < 1) {
		usage(argv[0]);
		exit(1);
	}

	void *ctx = knc_trnsp_new(devname);
	if (!ctx)
		exit(1);

	command = *args++; argc--;
	{
		struct knc_ctrl_command *cmd;
		for (cmd = knc_ctrl_commands; cmd->name; cmd++) {
			if (strcmp(cmd->name, command) == 0) {
				if (argc != cmd->nargs && cmd->nargs != -1) {
					fprintf(stderr, "ERROR: Invalid arguments");
					exit(1);
				}
				cmd->handler(ctx, argc, args);
				goto done;
			}
		}
	}

	if (argc < 2) {
		usage(argv[0]);
		exit(1);
	}
	if (isdigit(*command)) {
		channel = atoi(command);
	} else {
		channel = atoi(*args++); argc--;
	}
	die = atoi(*args++); argc--;
	if (isdigit(*command)) {
		command = *args++; argc--;
	}

	{
		struct knc_asic_command *cmd;
		for (cmd = knc_asic_commands; cmd->name; cmd++) {
			if (strcmp(cmd->name, command) == 0) {
				if (argc != cmd->nargs && cmd->nargs != -1) {
					fprintf(stderr, "ERROR: Invalid arguments");
					exit(1);
				}
				cmd->handler(ctx, channel, die, argc, args);
				goto done;
			}
		}
	}

	knc_trnsp_free(ctx);

	fprintf(stderr, "ERROR: Unknown command %s\n", command);
	exit(1);

done:
	knc_trnsp_free(ctx);
	
	return 0;
	
}
