#ifndef _KNC_ASIC_H
#define _KNC_ASIC_H
#include <stdint.h>
#include <miner.h>

#define	BLOCK_HEADER_BYTES			80
#define	BLOCK_HEADER_BYTES_WITHOUT_NONCE	(BLOCK_HEADER_BYTES - 4)

/* FPGA Command codes */
#define KNC_FPGA_CMD_SETUP               0x01
#define KNC_FPGA_CMD_WORK_REQUEST        0x02
#define KNC_FPGA_CMD_WORK_STATUS         0x03

/* ASIC Command codes */
#define	KNC_ASIC_CMD_GETINFO             0x80
#define KNC_ASIC_CMD_SETWORK             0x81
#define KNC_ASIC_CMD_SETWORK_CLEAN       0x83        /* Neptune */
#define KNC_ASIC_CMD_HALT                0x83        /* Jupiter */
#define KNC_ASIC_CMD_REPORT              0x82
#define KNC_ASIC_CMD_SETUP_CORE          0x87

/* Status byte */
#define KNC_ASIC_ACK_CRC                    (1<<5)
#define KNC_ASIC_ACK_ACCEPT                 (1<<2)
#define KNC_ASIC_ACK_MASK                   (~(KNC_ASIC_ACK_CRC|KNC_ASIC_ACK_ACCEPT))
#define KNC_ASIC_ACK_MATCH                  ((1<<7)|(1<<0))

/* Version word */
#define KNC_ASIC_VERSION_JUPITER            0xa001
#define KNC_ASIC_VERSION_NEPTUNE            0xa002
#define KNC_ASIC_VERSION_TITAN              0xa102

/* Limits of current chips & I/O board */
#define KNC_MAX_ASICS			6
#define KNC_MAX_DIES_PER_ASIC		4
#define KNC_CORES_PER_DIE_JUPITER	48
#define KNC_CORES_PER_DIE_NEPTUNE	360
#define KNC_CORES_PER_DIE_TITAN		571
#define KNC_MAX_CORES_PER_DIE		(KNC_CORES_PER_DIE_TITAN)
#define	KNC_TITAN_THREADS_PER_CORE	8

enum asic_version {
	KNC_VERSION_UNKNOWN = 0,
	KNC_VERSION_JUPITER,
	KNC_VERSION_NEPTUNE,
	KNC_VERSION_TITAN,
} version;

struct knc_die_info {
	enum asic_version version;
	char want_work[KNC_MAX_CORES_PER_DIE];
	char has_report[KNC_MAX_CORES_PER_DIE];
	int cores;
	int pll_locked;
	int hash_reset_n;
	int pll_reset_n;
	int pll_power_down;
};

#define KNC_NONCES_PER_REPORT 5
#define KNC_STATUS_BYTE_ERROR_COUNTERS 4

struct knc_report {
	int next_state;
	int state;
	int next_slot;
	int active_slot;
	uint32_t progress;
	struct {
		int slot;
		uint32_t nonce;
	} nonce[KNC_NONCES_PER_REPORT];
};

struct titan_setup_core_params {
	uint16_t bad_address_mask[2];
	uint16_t bad_address_match[2];
	uint8_t difficulty;
	uint8_t thread_enable;
	uint16_t thread_base_address[KNC_TITAN_THREADS_PER_CORE];
	uint16_t lookup_gap_mask[KNC_TITAN_THREADS_PER_CORE];
	uint16_t N_mask[KNC_TITAN_THREADS_PER_CORE];
	uint8_t N_shift[KNC_TITAN_THREADS_PER_CORE];
	uint32_t nonce_top;
	uint32_t nonce_bottom;
};

int knc_prepare_info(uint8_t *request, int die, struct knc_die_info *die_info, int *response_size);
int knc_prepare_report(uint8_t *request, int die, int core);
int knc_prepare_titan_setwork(uint8_t *request, int die, int core, int slot, struct work *work, int clean);
int knc_prepare_neptune_setwork(uint8_t *request, int die, int core, int slot, struct work *work, int clean);
int knc_prepare_jupiter_setwork(uint8_t *request, int die, int core, int slot, struct work *work);
int knc_prepare_jupiter_halt(uint8_t *request, int die, int core);
int knc_prepare_neptune_halt(uint8_t *request, int die, int core);
int knc_prepare_titan_halt(uint8_t *request, int die, int core);
int knc_prepare_titan_setup(uint8_t *request, int asic, int divider, int preclk, int declk, int sslowmin);
int knc_prepare_titan_work_request(uint8_t *request, int asic, int die, int slot, int core_range_start, int core_range_stop, int resend, struct work *work);
int knc_prepare_titan_work_status(uint8_t *request, int asic);

int knc_check_response(uint8_t *response, int response_length, uint8_t ack);

int knc_decode_info(uint8_t *response, struct knc_die_info *die_info);
int knc_decode_report(uint8_t *response, struct knc_report *report, int version);
int knc_decode_work_status(uint8_t *response, uint8_t *num_request_busy, uint16_t *num_status_byte_error);

void knc_prepare_neptune_titan_message(int request_length, const uint8_t *request, uint8_t *buffer);

bool fill_in_thread_params(int num_threads, struct titan_setup_core_params *params);
bool knc_titan_setup_core(void * const ctx, int channel, int die, int core, struct titan_setup_core_params *params);
bool knc_titan_setup_core_(int log_level, void * const ctx, int channel, int die, int core, struct titan_setup_core_params *params);

#define KNC_ACCEPTED    (1<<0)
#define KNC_ERR_CRC     (1<<1)
#define KNC_ERR_ACK     (1<<2)
#define KNC_ERR_CRCACK  (1<<3)
#define KNC_ERR_UNAVAIL (1<<4)
#define KNC_ERR_MASK	(~(KNC_ACCEPTED))
#define KNC_IS_ERROR(x) (((x) & KNC_ERR_MASK) != 0)

int knc_prepare_transfer(uint8_t *txbuf, int offset, int size, int channel, int request_length, const uint8_t *request, int response_length);
int knc_decode_response(uint8_t *rxbuf, int request_length, uint8_t **response, int response_length);
int knc_syncronous_transfer(void *ctx, int channel, int request_length, const uint8_t *request, int response_length, uint8_t *response);
void knc_syncronous_transfer_multi(void *ctx, int channel, int *request_lengths, int request_bufsize, const uint8_t *requests, int *response_lengths, int response_bufsize, uint8_t *responses, int *statuses, int num);
int knc_syncronous_transfer_fpga(void *ctx, int request_length, const uint8_t *request, int response_length, uint8_t *response);

/* Detect ASIC DIE version */
int knc_detect_die(void *ctx, int channel, int die, struct knc_die_info *die_info);
int knc_detect_die_(int log_level, void *ctx, int channel, int die, struct knc_die_info *die_info);
char * get_asicname_from_version(enum asic_version version);

/* Controller channel status */
struct knc_spimux_status
{
	char revision[5];
	int board_type;
	int board_rev;
	int core_status[KNC_MAX_DIES_PER_ASIC][KNC_MAX_CORES_PER_DIE];
};
#define KNC_CORE_AVAILABLE (1<<0)
int knc_prepare_status(uint8_t *txbuf, int offset, int size, int channel);
int knc_decode_status(uint8_t *response, struct knc_spimux_status *status);

/* controller ASIC clock configuration */
int knc_prepare_freq(uint8_t *txbuf, int offset, int size, int channel, int die, int freq);
int knc_decode_freq(uint8_t *response);

/* red, green, blue valid range 0 - 15. No response or checksum from controller */
int knc_prepare_led(uint8_t *txbuf, int offset, int size, int red, int green, int blue);

/* Reset controller */
int knc_prepare_reset(uint8_t *txbuf, int offset, int size);

#ifdef CONTROLLER_BOARD_BACKPLANE
#define FIRST_ASIC_I2C_BUS	1
#endif

#ifdef CONTROLLER_BOARD_RPI
#define FIRST_ASIC_I2C_BUS	2
#endif

#ifdef CONTROLLER_BOARD_BBB
#define FIRST_ASIC_I2C_BUS	3
#endif

#ifndef FIRST_ASIC_I2C_BUS
#warning Unspecified controller board
#define FIRST_ASIC_I2C_BUS 1
#endif

#endif /* _KNC_ASIC_H */
