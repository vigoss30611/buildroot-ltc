/*
 * library for KnCminer devices
 *
 * Copyright 2014 KnCminer
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 3 of the License, or (at your option)
 * any later version.  See COPYING for more details.
 */

#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <miner.h>
#include <logging.h>

#include "knc-transport.h"

#include "knc-asic.h"

#ifndef MAX
#define MAX(a,b) ((a) < (b) ? (b) : (a))
#endif

/* Control Commands
 *
 * SPI command on channel. 1-
 *   4'd8 4'channel 8'msglen_in_bytes SPI message data
 * Sends the supplied message on selected SPI bus
 *
 * Reset the controller.
 * LED control
 *   4'd1 4'red 4'green 4'blue
 * Sets led colour
 *
 * Clock frequency
 *   4'd2 4'channel 8'msglen_in_bytes 4'die 16'MHz 512'x
 * Configures the hashing clock rate
 *
 * Channel status
 *   4'd3, 4'channel, 8'x -> 32'revision, 8'board_type, 8'board_revision, 48'reserved, 1440'core_available (360' per die)
 * request information about a channel
 *
 * Communication test
 *   16'h0001 16'x
 * Simple test of SPI communication
 *
 * Reset controller
 *   16'h0002
 * Reset the SPI MUX controller
 *
 * Titan Specific Commands
 *
 * Setup SPI
 *   12'h1 4'asic 16'divider 8'preclk 8'declk 8'sslowmin
 * Set up FPGA to ASIC SPI communication parameters
 *
 * Work Request
 *   12'h2 4'asic 4'die 4'slot 16'core_start 16'core_stop 8'resend 608'work 8'dont_care
 * Order FPGA to send work to many cores
 *
 * Work Status
 *   12'h3 4'asic -> 8'hFF 8'num_request_busy 8'num_status_byte_error
 * Get status on work requests currently being sent by FPGA
 *
 */

int knc_transfer_length(int request_length, int response_length)
{
	/* FPGA control, request header, request body/response, CRC(4), ACK(1), EXTRA(3) */
	return 2 + MAX(request_length, 4 + response_length ) + 4 + 1 + 3;
}

int knc_prepare_transfer(uint8_t *txbuf, int offset, int size, int channel, int request_length, const uint8_t *request, int response_length)
{
	/* FPGA control, request header, request body/response, CRC(4), ACK(1), EXTRA(3) */
        int msglen = MAX(request_length, 4 + response_length ) + 4 + 1 + 3;
        int len = 2 + msglen;
	txbuf += offset;

	if (len + offset > size) {
		applog(LOG_DEBUG, "KnC SPI buffer full");
		return -1;
	}
	txbuf[0] = 8 << 4 | (channel + 1);
	txbuf[1] = msglen;
	knc_prepare_neptune_titan_message(request_length, request, txbuf+2);

	return offset + len;
}

/* red, green, blue valid range 0 - 15 */
int knc_prepare_led(uint8_t *txbuf, int offset, int size, int red, int green, int blue)
{
	/* 4'h1 4'red 4'green 4'blue */
        int len = 2;
	txbuf += offset;

	if (len + offset > size) {
		applog(LOG_DEBUG, "KnC SPI buffer full");
		return -1;
	}
	txbuf[0] = 1 << 4 | red;
	txbuf[1] = green << 4 | blue;

	return offset + len;
}

/* reset controller */
int knc_prepare_reset(uint8_t *txbuf, int offset, int size)
{
	/* 16'h2 16'unused */
        int len = 4;
	txbuf += offset;

	if (len + offset > size) {
		applog(LOG_DEBUG, "KnC SPI buffer full");
		return -1;
	}
	txbuf[0] = (0x0002) >> 8;
	txbuf[1] = (0x0002) & 0xff;
	txbuf[2] = 0;
	txbuf[3] = 0;

	return offset + len;
}


/* controller channel status */
int knc_prepare_status(uint8_t *txbuf, int offset, int size, int channel)
{
	/* 4'op=3, 3'channel, 9'x -> 32'revision, 8'board_type, 8'board_revision, 48'reserved, 1440'core_available (360' per die) */
	int len = (16 + 32 + 8 + 8 + 48 + KNC_MAX_CORES_PER_DIE * 4) / 8;
	txbuf += offset;

	if (len + offset > size) {
		applog(LOG_DEBUG, "KnC SPI buffer full");
		return -1;
	}

	memset(txbuf, 0, len);
	txbuf[0] = 3 << 4 | (channel + 1);

	return len;
}

int knc_decode_status(uint8_t *response, struct knc_spimux_status *status)
{
	memcpy(status->revision, response+2, 4);
	status->revision[4] = '\0';
	status->board_type = response[6];
	status->board_rev = response[7];
	if (memcmp(status->revision, "\377\377\377\377", 4) == 0) {
		memset(status, 0, sizeof(*status));
		return -1; /* No FPGA found */
	}
	int die;
	for (die = 0; die < KNC_MAX_DIES_PER_ASIC; die++) {
		int core;
		for (core = 0; core < KNC_MAX_CORES_PER_DIE; core++) {
			int i = die * KNC_MAX_CORES_PER_DIE + core;
			status->core_status[die][core] = 0;
			if (response[14 + i / 8] >> (i % 8))
				status->core_status[die][core] |= KNC_CORE_AVAILABLE;
		}
	}
	return 0;
}

#define FREQ_RESPONSE_PAD 1000
/* controller ASIC clock configuration */
int knc_prepare_freq(uint8_t *txbuf, int offset, int size, int channel, int die, int freq)
{
	/* 4'op=2, 12'length, 4'bus, 4'die, 16'freq, many more clocks */
	int request_len = 4 + 12 + 16 + 4 + 4 + 16;
	int len = (request_len + FREQ_RESPONSE_PAD) / 8;
	txbuf += offset;

	if (2 + len + offset > size) {
		applog(LOG_DEBUG, "KnC SPI buffer full");
		return -1;
	}

	if (freq > 1000000)
		freq = freq / 1000000;  // Assume Hz was given instead of MHz

	memset(txbuf, 0, len);
	txbuf[0] = 2 << 4 | (channel + 1);
	txbuf[1] = len;
	txbuf[2] = (die << 0);
	txbuf[3] = (freq >> 8);
	txbuf[4] = (freq >> 0);

	return len;
}

int knc_decode_freq(uint8_t *response)
{
	/* 4'op=2, 12'length, 4'bus, 4'die, 16'freq, many more clocks */
	int request_len = 4 + 12 + 16 + 4 + 4 + 16;
	int len = (request_len + FREQ_RESPONSE_PAD) / 8;

	int i;
	int freq = -1;
	for (i = request_len / 8; i < len-1; i++) {
		if (response[i] == 0xf1) {
			break;
		} else if (response[i] == 0xf0) {
			freq = response[i+1]<<8 | response[i+2];
			applog(LOG_DEBUG, "KnC: Accepted FREQ=%d", freq);
			i+=2;
		}
	}
	if (response[i] == 0xf1) {
		applog(LOG_INFO, "KnC: Frequency change successful, FREQ=%d", freq);
		return freq;
	} else {
		applog(LOG_ERR, "KnC: Frequency change FAILED!");
		return -1;
	}
}

/* request_length = 0 disables communication checks, i.e. Jupiter protocol */
int knc_decode_response(uint8_t *rxbuf, int request_length, uint8_t **response, int response_length)
{
	if (response) {
		if (response_length > 0) {
			*response = rxbuf + 2 + 4;
		} else {
			*response = NULL;
		}
	}

	if (request_length == 0)
		return 0;

	return knc_check_response(rxbuf + 2 + 4, response_length, rxbuf[2+4+MAX(request_length-4, response_length)+4]);
}

int knc_syncronous_transfer(void *ctx, int channel, int request_length, const uint8_t *request, int response_length, uint8_t *response)
{
	int len = knc_transfer_length(request_length, response_length);
	if (MAX_BYTES_IN_SPI_XSFER < len)
		return -1;
	uint8_t txbuf[len];
	uint8_t rxbuf[len];
	memset(txbuf, 0, len);
	knc_prepare_transfer(txbuf, 0, len, channel, request_length, request, response_length);
	knc_trnsp_transfer(ctx, txbuf, rxbuf, len);

	uint8_t *response_buf;
	int ret = knc_decode_response(rxbuf, request_length, &response_buf, response_length);
	if (response)
		memcpy(response, response_buf, response_length);
	if (ret && memcmp(&rxbuf[len-4], "\377\377\377\377", 4) == 0)
		ret = KNC_ERR_UNAVAIL;
	return ret;
}

void knc_syncronous_transfer_multi(void *ctx, int channel, int *request_lengths, int request_bufsize, const uint8_t *requests, int *response_lengths, int response_bufsize, uint8_t *responses, int *statuses, int num)
{
	if (0 >= num)
		return;
	int i;
	int len = knc_transfer_length(request_bufsize, response_bufsize);
	uint8_t *txbuf = calloc(1, num * len);
	uint8_t *rxbuf = malloc(num * len);
	uint8_t **txbuf_ptrs = malloc(num * sizeof(uint8_t*));
	uint8_t **rxbuf_ptrs = malloc(num * sizeof(uint8_t*));
	int *lengths = malloc(num * sizeof(int));

	if ((NULL == lengths) || (NULL == rxbuf_ptrs) || (NULL == txbuf_ptrs) || (NULL == rxbuf) || (NULL == txbuf))
		goto exit;

	int cur_chunk_base = 0;
	int cur_chunk_num = 0;
	int cur_chunk_len = 0;
	for (i = 0; i < num; ++i) {
		lengths[i] = knc_transfer_length(request_lengths[i], response_lengths[i]);
		if (MAX_BYTES_IN_SPI_XSFER <= (cur_chunk_len + lengths[i])) {
			/* Time to send chunk */
			knc_trnsp_transfer_multi(ctx, &txbuf_ptrs[cur_chunk_base], &rxbuf_ptrs[cur_chunk_base], &lengths[cur_chunk_base], cur_chunk_num);
			cur_chunk_base += cur_chunk_num;
			cur_chunk_num = 0;
			cur_chunk_len = 0;
		}
		knc_prepare_transfer(txbuf, i * len, num * len, channel, request_lengths[i], &requests[i * request_bufsize], response_lengths[i]);
		txbuf_ptrs[i] = &txbuf[i * len];
		rxbuf_ptrs[i] = &rxbuf[i * len];

		++cur_chunk_num;
		cur_chunk_len += lengths[i];
	}
	/* Send last chunk */
	knc_trnsp_transfer_multi(ctx, &txbuf_ptrs[cur_chunk_base], &rxbuf_ptrs[cur_chunk_base], &lengths[cur_chunk_base], cur_chunk_num);

	for (i = 0; i < num; ++i) {
		uint8_t *response_buf;
		int ret = knc_decode_response(rxbuf_ptrs[i], request_lengths[i], &response_buf, response_lengths[i]);
		memcpy(&responses[i * response_bufsize], response_buf, response_lengths[i]);
		if (ret && memcmp(&(rxbuf_ptrs[i][lengths[i] - 4]), "\377\377\377\377", 4) == 0)
			ret = KNC_ERR_UNAVAIL;
		statuses[i] = ret;
	}

exit:
	if (NULL != lengths)
		free(lengths);
	if (NULL != rxbuf_ptrs)
		free(rxbuf_ptrs);
	if (NULL != txbuf_ptrs)
		free(txbuf_ptrs);
	if (NULL != rxbuf)
		free(rxbuf);
	if (NULL != txbuf)
		free(txbuf);
}

int knc_syncronous_transfer_fpga(void *ctx, int request_length, const uint8_t *request, int response_length, uint8_t *response)
{
	int len = request_length + response_length;
	uint8_t rxbuf[len];

	knc_trnsp_transfer(ctx, request, rxbuf, len);

	if (response)
		memcpy(response, rxbuf + request_length, response_length);

	return 0;
}
