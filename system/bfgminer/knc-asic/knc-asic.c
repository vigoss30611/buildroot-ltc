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

#include "knc-asic.h"
#include "knc-transport.h"

/* ASIC Command structure
 * command      8 bits
 * chip         8 bits
 * core         16 bits
 * data         [command dependent]
 * CRC32        32 bits (Neptune & Titan)
 *
 * ASIC response starts immediately after core address bits.
 *
 * response data
 * CRC32        32 bits (Neptune & Titan)
 * STATUS       8 bits   1 0 ~CRC_OK 0 0 ACCEPTED_WORK 0 1 (Neptune & Titan)
 *
 * Work Submission
 * ===============
 *
 * 0x81 SETWORK (Jupiter)
 * ----------------------
 *
 * slot | 0xf0  8 bits
 * midstate     256 bits
 * data         96 bits
 *
 * No response
 *
 * 0x81 SETWORK (Neptune)
 * 0x83 SETWORK_CLEAN (Neptune)
 * -------------------------------------------
 *
 * slot | 0xf0  8 bits
 * precalc_midstate  192 bits
 * precalc_data 96 bits
 * midstate     256 bits
 *
 * SETWORK_CLEAN replaces any current work
 *
 * Returns REPORT response
 *
 * 0x81 SETWORK (Titan)
 * 0x83 SETWORK_CLEAN (Titan)
 * -------------------------------------------
 *
 * slot | 0xf0  8 bits
 * data         608 bits (Block header without last 4 bytes of nonce)
 *
 * SETWORK_CLEAN replaces any current work
 *
 * Returns REPORT response
 *
 * 0x83 HALT (Jupiter)
 * -------------------
 *
 * tells the core to discard current work
 *
 * Status queries
 * ==============
 *
 * 0x80 GETINFO
 * ------------
 *
 * (core field unused)
 *
 * cores        16 bits
 * version      16 bits
 * reserved     60 bits         (Neptune)
 * die_status    4 bits		(Neptune)
 *               1' pll_locked
 *               1' hash_reset_n	1 if cores have been reset since last report
 *               1' pll_reset_n		1 if PLL have been reset since last report
 *               1' pll_power_down
 * core_status  cores * 2 bits  (Neptune & Titan) rounded up to bytes
 *                              1' want_work 
 *                              1' has_report (Titan only, unreliable on Neptune)
 *
 * 0x82 REPORT
 * -----------
 *
 * reserved     2 bits
 * next_state   1 bit   next work state loaded
 * state        1 bit   hashing  (0 on Jupiter)
 * next_slot    4 bit   slot id of next work state (0 on Jupiter)
 * progress     8 bits  upper 8 bits of nonce counter
 * active_slot  4 bits  slot id of current work state
 * nonce_slot   4 bits  slot id of found nonce
 * nonce        32 bits
 * 
 * reserved     4 bits
 * nonce_slot   4 bits
 * nonce        32 bits
 *
 * repeat for 5 nonce entries in total on Neptune & Titan
 * Jupiter only has first nonce entry
 */

// Precalculate first 3 rounds of SHA256 - as much as possible	
// Macro routines copied from sha2.c
static void knc_prepare_neptune_work(unsigned char *out, struct work *work) {
        const uint8_t *midstate = work->midstate;
        const uint8_t *data = work->data + 16*4;

#ifndef GET_ULONG_BE
#define GET_ULONG_BE(b,i)                             \
		(( (uint32_t) (b)[(i)    ] << 24 )	\
                | ( (uint32_t) (b)[(i) + 1] << 16 )	\
                | ( (uint32_t) (b)[(i) + 2] <<  8 )	\
                | ( (uint32_t) (b)[(i) + 3]       ))
#endif

#ifndef GET_ULONG_LE
#define GET_ULONG_LE(b,i)                             \
		(( (uint32_t) (b)[(i) + 3] << 24 )	\
                | ( (uint32_t) (b)[(i) + 2] << 16 )	\
                | ( (uint32_t) (b)[(i) + 1] <<  8 )	\
                | ( (uint32_t) (b)[(i) + 0]       ))
#endif

#ifndef PUT_ULONG_BE
#define PUT_ULONG_BE(n,b,i)                             \
	{						\
		(b)[(i)    ] = (unsigned char) ( (n) >> 24 );	\
		(b)[(i) + 1] = (unsigned char) ( (n) >> 16 );	\
		(b)[(i) + 2] = (unsigned char) ( (n) >>  8 );	\
		(b)[(i) + 3] = (unsigned char) ( (n)       );	\
	}
#endif

#ifndef PUT_ULONG_LE
#define PUT_ULONG_LE(n,b,i)                             \
	{						\
		(b)[(i) + 3] = (unsigned char) ( (n) >> 24 );	\
		(b)[(i) + 2] = (unsigned char) ( (n) >> 16 );	\
		(b)[(i) + 1] = (unsigned char) ( (n) >>  8 );	\
		(b)[(i) + 0] = (unsigned char) ( (n)       );	\
	}
#endif

#define  SHR(x,n) ((x & 0xFFFFFFFF) >> n)
#define ROTR(x,n) (SHR(x,n) | (x << (32 - n)))

#define S0(x) (ROTR(x, 7) ^ ROTR(x,18) ^  SHR(x, 3))
#define S1(x) (ROTR(x,17) ^ ROTR(x,19) ^  SHR(x,10))

#define S2(x) (ROTR(x, 2) ^ ROTR(x,13) ^ ROTR(x,22))
#define S3(x) (ROTR(x, 6) ^ ROTR(x,11) ^ ROTR(x,25))

#define F0(x,y,z) ((x & y) | (z & (x | y)))
#define F1(x,y,z) (z ^ (x & (y ^ z)))

#define R(t)                                    \
(                                               \
    W[t] = S1(W[t -  2]) + W[t -  7] +          \
           S0(W[t - 15]) + W[t - 16]            \
)

#define P(a,b,c,d,e,f,g,h,x,K)                  \
	{					\
		temp1 = h + S3(e) + F1(e,f,g) + K + x;	\
		temp2 = S2(a) + F0(a,b,c);		\
		d += temp1; h = temp1 + temp2;		\
	}

    uint32_t temp1, temp2, W[16+3];
    uint32_t A, B, C, D, E, F, G, H;

    W[0] = GET_ULONG_LE(data,  0*4 );
    W[1] = GET_ULONG_LE(data,  1*4 );
    W[2] = GET_ULONG_LE(data,  2*4 );
    W[3] = 0;                 // since S0(0)==0, this must be 0. S0(nonce) is added in hardware.
    W[4] = 0x80000000;
    W[5] = 0;
    W[6] = 0;
    W[7] = 0;
    W[8] = 0;
    W[9] = 0;
    W[10] = 0;
    W[11] = 0;
    W[12] = 0;
    W[13] = 0;
    W[14] = 0;
    W[15] = 0x00000280;
    R(16);  // Expand W 14, 9, 1, 0
    R(17);  //          15, 10, 2, 1
    R(18);  //          16, 11, 3, 2

    A = GET_ULONG_LE(midstate, 0*4 );
    B = GET_ULONG_LE(midstate, 1*4 );
    C = GET_ULONG_LE(midstate, 2*4 );
    D = GET_ULONG_LE(midstate, 3*4 );
    E = GET_ULONG_LE(midstate, 4*4 );
    F = GET_ULONG_LE(midstate, 5*4 );
    G = GET_ULONG_LE(midstate, 6*4 );
    H = GET_ULONG_LE(midstate, 7*4 );

    uint32_t D_ = D, H_ = H;
    P( A, B, C, D_, E, F, G, H_, W[ 0], 0x428A2F98 );
    uint32_t C_ = C, G_ = G;
    P( H_, A, B, C_, D_, E, F, G_, W[ 1], 0x71374491 );
    uint32_t B_ = B, F_ = F;
    P( G_, H_, A, B_, C_, D_, E, F_, W[ 2], 0xB5C0FBCF );

    PUT_ULONG_BE( D_, out, 0*4 );
    PUT_ULONG_BE( C_, out, 1*4 );
    PUT_ULONG_BE( B_, out, 2*4 );
    PUT_ULONG_BE( H_, out, 3*4 );
    PUT_ULONG_BE( G_, out, 4*4 );
    PUT_ULONG_BE( F_, out, 5*4 );
    PUT_ULONG_BE( W[18], out, 6*4 );  // This is partial S0(nonce) added by hardware
    PUT_ULONG_BE( W[17], out, 7*4 );
    PUT_ULONG_BE( W[16], out, 8*4 );
    PUT_ULONG_BE( H, out, 9*4 );
    PUT_ULONG_BE( G, out, 10*4 );
    PUT_ULONG_BE( F, out, 11*4 );
    PUT_ULONG_BE( E, out, 12*4 );
    PUT_ULONG_BE( D, out, 13*4 );
    PUT_ULONG_BE( C, out, 14*4 );
    PUT_ULONG_BE( B, out, 15*4 );
    PUT_ULONG_BE( A, out, 16*4 );
}

static void knc_prepare_jupiter_work(unsigned char *out, struct work *work) {
        int i;
        for (i = 0; i < 8 * 4; i++)
                out[i] = work->midstate[8 * 4 - i - 1];
        for (i = 0; i < 3 * 4; i++)
                out[8 * 4 + i] = work->data[16 * 4 + 3 * 4 - i - 1];
}

static void knc_prepare_titan_work(unsigned char *out, struct work *work)
{
        int i;
	unsigned char *in = work->data;
	for (i = 0; i < BLOCK_HEADER_BYTES_WITHOUT_NONCE; i += 4) {
		out[i] = in[i + 3];
		out[i + 1] = in[i + 2];
		out[i + 2] = in[i + 1];
		out[i + 3] = in[i];
	}
}

static void knc_prepare_core_command(uint8_t *request, int command, int die, int core)
{
	request[0] = command;
	request[1] = die;
	request[2] = core >> 8;
	request[3] = core & 0xff;
}

int knc_prepare_report(uint8_t *request, int die, int core)
{
	knc_prepare_core_command(request, KNC_ASIC_CMD_REPORT, die, core);
	return 4;
}

int knc_prepare_info(uint8_t *request, int die, struct knc_die_info *die_info, int *response_size)
{
	request[0] = KNC_ASIC_CMD_GETINFO;
	request[1] = die;
	request[2] = 0;
	request[3] = 0;
	switch (die_info->version) {
	case KNC_VERSION_JUPITER:
		*response_size = 4;
		break;
	default:
		*response_size = 12 + (KNC_MAX_CORES_PER_DIE*2 + 7) / 8;
		break;
	case KNC_VERSION_NEPTUNE:
	case KNC_VERSION_TITAN:
		*response_size = 12 + (die_info->cores*2 + 7) / 8;
		break;
	}
	return 4;
}

int knc_prepare_titan_setwork(uint8_t *request, int die, int core, int slot, struct work *work, int clean)
{
	if (!clean)
		knc_prepare_core_command(request, KNC_ASIC_CMD_SETWORK, die, core);
	else
		knc_prepare_core_command(request, KNC_ASIC_CMD_SETWORK_CLEAN, die, core);
	request[4] = slot | 0xf0;
	if (work)
		knc_prepare_titan_work(request + 4 + 1, work);
	else
		memset(request + 4 + 1, 0, BLOCK_HEADER_BYTES_WITHOUT_NONCE);
	return 4 + 1 + BLOCK_HEADER_BYTES_WITHOUT_NONCE;
}

int knc_prepare_neptune_setwork(uint8_t *request, int die, int core, int slot, struct work *work, int clean)
{
	if (!clean)
		knc_prepare_core_command(request, KNC_ASIC_CMD_SETWORK, die, core);
	else
		knc_prepare_core_command(request, KNC_ASIC_CMD_SETWORK_CLEAN, die, core);
	request[4] = slot | 0xf0;
	if (work)
		knc_prepare_neptune_work(request + 4 + 1, work);
	else
		memset(request + 4 + 1, 0, 6*4 + 3*4 + 8*4);
	return 4 + 1 + 6*4 + 3*4 + 8*4;
}

int knc_prepare_jupiter_setwork(uint8_t *request, int die, int core, int slot, struct work *work)
{
	knc_prepare_core_command(request, KNC_ASIC_CMD_SETWORK, die, core);
	request[4] = slot | 0xf0;
	if (work)
		knc_prepare_jupiter_work(request + 4 + 1, work);
	else
		memset(request + 4 + 1, 0, 8*4 + 3*4);
	return 4 + 1 + 8*4 + 3*4;
}

int knc_prepare_jupiter_halt(uint8_t *request, int die, int core)
{
	knc_prepare_core_command(request, KNC_ASIC_CMD_HALT, die, core);
	return 4;
}

int knc_prepare_neptune_halt(uint8_t *request, int die, int core)
{
	knc_prepare_core_command(request, KNC_ASIC_CMD_HALT, die, core);
	request[4] = 0 | 0xf0;
	memset(request + 4 + 1, 0, 6*4 + 3*4 + 8*4);
	return 4 + 1 + 6*4 + 3*4 + 8*4;
}

int knc_prepare_titan_halt(uint8_t *request, int die, int core)
{
	knc_prepare_core_command(request, KNC_ASIC_CMD_HALT, die, core);
	request[4] = 0 | 0xf0;
	memset(request + 4 + 1, 0, BLOCK_HEADER_BYTES_WITHOUT_NONCE);
	return 4 + 1 + BLOCK_HEADER_BYTES_WITHOUT_NONCE;
}

int knc_prepare_titan_setup(uint8_t *request, int asic, int divider, int preclk, int declk, int sslowmin)
{
	request[0] = (KNC_FPGA_CMD_SETUP >> 4);
	request[1] = ((KNC_FPGA_CMD_SETUP & 0xF) << 4) | asic;
	request[2] = divider >> 8;
	request[3] = divider & 0xFF;
	request[4] = preclk;
	request[5] = declk;
	request[6] = sslowmin;
	return 7;
}

int knc_prepare_titan_work_request(uint8_t *request, int asic, int die, int slot, int core_range_start, int core_range_stop, int resend, struct work *work)
{
	request[0] = (KNC_FPGA_CMD_WORK_REQUEST >> 4);
	request[1] = ((KNC_FPGA_CMD_WORK_REQUEST & 0xF) << 4) | asic;
	request[2] = (die << 4) | slot;
	request[3] = core_range_start >> 8;
	request[4] = core_range_start & 0xFF;
	request[5] = core_range_stop >> 8;
	request[6] = core_range_stop & 0xFF;
	request[7] = resend;
	if (work)
		knc_prepare_titan_work(request + 8, work);
	else
		memset(request + 8, 0, BLOCK_HEADER_BYTES_WITHOUT_NONCE);
	request[8 + BLOCK_HEADER_BYTES_WITHOUT_NONCE] = 0;
	return 9 + BLOCK_HEADER_BYTES_WITHOUT_NONCE;
}

int knc_prepare_titan_work_status(uint8_t *request, int asic)
{
	request[0] = (KNC_FPGA_CMD_WORK_STATUS >> 4);
	request[1] = ((KNC_FPGA_CMD_WORK_STATUS & 0xF) << 4) | asic;
	return 2;
}


/* crc32.c -- compute the CRC-32 of a data stream, stolen from zlib
 */

#include <stdint.h>

static const uint32_t crc_table[256] = {
    0x00000000UL, 0x77073096UL, 0xee0e612cUL, 0x990951baUL, 0x076dc419UL,
    0x706af48fUL, 0xe963a535UL, 0x9e6495a3UL, 0x0edb8832UL, 0x79dcb8a4UL,
    0xe0d5e91eUL, 0x97d2d988UL, 0x09b64c2bUL, 0x7eb17cbdUL, 0xe7b82d07UL,
    0x90bf1d91UL, 0x1db71064UL, 0x6ab020f2UL, 0xf3b97148UL, 0x84be41deUL,
    0x1adad47dUL, 0x6ddde4ebUL, 0xf4d4b551UL, 0x83d385c7UL, 0x136c9856UL,
    0x646ba8c0UL, 0xfd62f97aUL, 0x8a65c9ecUL, 0x14015c4fUL, 0x63066cd9UL,
    0xfa0f3d63UL, 0x8d080df5UL, 0x3b6e20c8UL, 0x4c69105eUL, 0xd56041e4UL,
    0xa2677172UL, 0x3c03e4d1UL, 0x4b04d447UL, 0xd20d85fdUL, 0xa50ab56bUL,
    0x35b5a8faUL, 0x42b2986cUL, 0xdbbbc9d6UL, 0xacbcf940UL, 0x32d86ce3UL,
    0x45df5c75UL, 0xdcd60dcfUL, 0xabd13d59UL, 0x26d930acUL, 0x51de003aUL,
    0xc8d75180UL, 0xbfd06116UL, 0x21b4f4b5UL, 0x56b3c423UL, 0xcfba9599UL,
    0xb8bda50fUL, 0x2802b89eUL, 0x5f058808UL, 0xc60cd9b2UL, 0xb10be924UL,
    0x2f6f7c87UL, 0x58684c11UL, 0xc1611dabUL, 0xb6662d3dUL, 0x76dc4190UL,
    0x01db7106UL, 0x98d220bcUL, 0xefd5102aUL, 0x71b18589UL, 0x06b6b51fUL,
    0x9fbfe4a5UL, 0xe8b8d433UL, 0x7807c9a2UL, 0x0f00f934UL, 0x9609a88eUL,
    0xe10e9818UL, 0x7f6a0dbbUL, 0x086d3d2dUL, 0x91646c97UL, 0xe6635c01UL,
    0x6b6b51f4UL, 0x1c6c6162UL, 0x856530d8UL, 0xf262004eUL, 0x6c0695edUL,
    0x1b01a57bUL, 0x8208f4c1UL, 0xf50fc457UL, 0x65b0d9c6UL, 0x12b7e950UL,
    0x8bbeb8eaUL, 0xfcb9887cUL, 0x62dd1ddfUL, 0x15da2d49UL, 0x8cd37cf3UL,
    0xfbd44c65UL, 0x4db26158UL, 0x3ab551ceUL, 0xa3bc0074UL, 0xd4bb30e2UL,
    0x4adfa541UL, 0x3dd895d7UL, 0xa4d1c46dUL, 0xd3d6f4fbUL, 0x4369e96aUL,
    0x346ed9fcUL, 0xad678846UL, 0xda60b8d0UL, 0x44042d73UL, 0x33031de5UL,
    0xaa0a4c5fUL, 0xdd0d7cc9UL, 0x5005713cUL, 0x270241aaUL, 0xbe0b1010UL,
    0xc90c2086UL, 0x5768b525UL, 0x206f85b3UL, 0xb966d409UL, 0xce61e49fUL,
    0x5edef90eUL, 0x29d9c998UL, 0xb0d09822UL, 0xc7d7a8b4UL, 0x59b33d17UL,
    0x2eb40d81UL, 0xb7bd5c3bUL, 0xc0ba6cadUL, 0xedb88320UL, 0x9abfb3b6UL,
    0x03b6e20cUL, 0x74b1d29aUL, 0xead54739UL, 0x9dd277afUL, 0x04db2615UL,
    0x73dc1683UL, 0xe3630b12UL, 0x94643b84UL, 0x0d6d6a3eUL, 0x7a6a5aa8UL,
    0xe40ecf0bUL, 0x9309ff9dUL, 0x0a00ae27UL, 0x7d079eb1UL, 0xf00f9344UL,
    0x8708a3d2UL, 0x1e01f268UL, 0x6906c2feUL, 0xf762575dUL, 0x806567cbUL,
    0x196c3671UL, 0x6e6b06e7UL, 0xfed41b76UL, 0x89d32be0UL, 0x10da7a5aUL,
    0x67dd4accUL, 0xf9b9df6fUL, 0x8ebeeff9UL, 0x17b7be43UL, 0x60b08ed5UL,
    0xd6d6a3e8UL, 0xa1d1937eUL, 0x38d8c2c4UL, 0x4fdff252UL, 0xd1bb67f1UL,
    0xa6bc5767UL, 0x3fb506ddUL, 0x48b2364bUL, 0xd80d2bdaUL, 0xaf0a1b4cUL,
    0x36034af6UL, 0x41047a60UL, 0xdf60efc3UL, 0xa867df55UL, 0x316e8eefUL,
    0x4669be79UL, 0xcb61b38cUL, 0xbc66831aUL, 0x256fd2a0UL, 0x5268e236UL,
    0xcc0c7795UL, 0xbb0b4703UL, 0x220216b9UL, 0x5505262fUL, 0xc5ba3bbeUL,
    0xb2bd0b28UL, 0x2bb45a92UL, 0x5cb36a04UL, 0xc2d7ffa7UL, 0xb5d0cf31UL,
    0x2cd99e8bUL, 0x5bdeae1dUL, 0x9b64c2b0UL, 0xec63f226UL, 0x756aa39cUL,
    0x026d930aUL, 0x9c0906a9UL, 0xeb0e363fUL, 0x72076785UL, 0x05005713UL,
    0x95bf4a82UL, 0xe2b87a14UL, 0x7bb12baeUL, 0x0cb61b38UL, 0x92d28e9bUL,
    0xe5d5be0dUL, 0x7cdcefb7UL, 0x0bdbdf21UL, 0x86d3d2d4UL, 0xf1d4e242UL,
    0x68ddb3f8UL, 0x1fda836eUL, 0x81be16cdUL, 0xf6b9265bUL, 0x6fb077e1UL,
    0x18b74777UL, 0x88085ae6UL, 0xff0f6a70UL, 0x66063bcaUL, 0x11010b5cUL,
    0x8f659effUL, 0xf862ae69UL, 0x616bffd3UL, 0x166ccf45UL, 0xa00ae278UL,
    0xd70dd2eeUL, 0x4e048354UL, 0x3903b3c2UL, 0xa7672661UL, 0xd06016f7UL,
    0x4969474dUL, 0x3e6e77dbUL, 0xaed16a4aUL, 0xd9d65adcUL, 0x40df0b66UL,
    0x37d83bf0UL, 0xa9bcae53UL, 0xdebb9ec5UL, 0x47b2cf7fUL, 0x30b5ffe9UL,
    0xbdbdf21cUL, 0xcabac28aUL, 0x53b39330UL, 0x24b4a3a6UL, 0xbad03605UL,
    0xcdd70693UL, 0x54de5729UL, 0x23d967bfUL, 0xb3667a2eUL, 0xc4614ab8UL,
    0x5d681b02UL, 0x2a6f2b94UL, 0xb40bbe37UL, 0xc30c8ea1UL, 0x5a05df1bUL,
    0x2d02ef8dUL
};

#define DO1 crc = crc_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8)
#define DO8 DO1; DO1; DO1; DO1; DO1; DO1; DO1; DO1

static uint32_t crc32(const unsigned char *buf, unsigned len)
{
	uint32_t crc = 0xffffffffUL;
	while (len >= 8) {
		DO8;
		len -= 8;
	}
	if (len) do {
		DO1;
	} while (--len);
	return crc ^ 0xffffffffUL;
}

void knc_prepare_neptune_titan_message(int request_length, const uint8_t *request, uint8_t *buffer)
{
	uint32_t crc;
	memcpy(buffer, request, request_length);
	buffer += request_length;
	crc = crc32(request, request_length);
	PUT_ULONG_BE(crc, buffer, 0);
}

int knc_check_response(uint8_t *response, int response_length, uint8_t ack)
{
	int ret = 0;
	if (response_length > 0) {
		uint32_t crc, recv_crc;
		crc = crc32(response, response_length);
		recv_crc = GET_ULONG_BE(response, response_length);
		if (crc != recv_crc)
			ret |= KNC_ERR_CRC;
	}

	if ((ack & KNC_ASIC_ACK_MASK) != KNC_ASIC_ACK_MATCH)
		ret |= KNC_ERR_ACK;
	if ((ack & KNC_ASIC_ACK_CRC))
		ret |= KNC_ERR_CRCACK;
	if ((ack & KNC_ASIC_ACK_ACCEPT))
		ret |= KNC_ACCEPTED;
	return ret;
}

int knc_decode_info(uint8_t *response, struct knc_die_info *die_info)
{
	int cores_in_die = response[0]<<8 | response[1];
	int version = response[2]<<8 | response[3];
	if (version == KNC_ASIC_VERSION_JUPITER && cores_in_die <= KNC_CORES_PER_DIE_JUPITER) {
		die_info->version = KNC_VERSION_JUPITER;
		die_info->cores = cores_in_die;
		memset(die_info->want_work, -1, cores_in_die);
		memset(die_info->has_report, -1, cores_in_die);
		die_info->pll_power_down = -1;
		die_info->pll_reset_n = -1;
		die_info->hash_reset_n = -1;
		die_info->pll_locked = -1;
		return 0;
	} else if (version == KNC_ASIC_VERSION_NEPTUNE && cores_in_die <= KNC_CORES_PER_DIE_NEPTUNE) {
		die_info->version = KNC_VERSION_NEPTUNE;
		die_info->cores = cores_in_die;
		int core;
		for (core = 0; core < cores_in_die; core++)
			die_info->want_work[core] = ((response[12 + core/4] >> ((3-(core % 4)) * 2)) >> 1) & 1;
		memset(die_info->has_report, -1, cores_in_die);
		int die_status = response[11] & 0xf;
		die_info->pll_power_down = (die_status >> 0) & 1;
		die_info->pll_reset_n = (die_status >> 1) & 1;
		die_info->hash_reset_n = (die_status >> 2) & 1;
		die_info->pll_locked = (die_status >> 3) & 1;
		return 0;
	} else if (version == KNC_ASIC_VERSION_TITAN && cores_in_die <= KNC_CORES_PER_DIE_TITAN) {
		die_info->version = KNC_VERSION_TITAN;
		die_info->cores = cores_in_die;
		int core;
		for (core = 0; core < cores_in_die; core++) {
			die_info->has_report[core] = (response[12 + core/4] >> ((3-(core % 4)) * 2)) & 1;
			die_info->want_work[core] = ((response[12 + core/4] >> ((3-(core % 4)) * 2)) >> 1) & 1;
		}
		die_info->pll_power_down = -1;
		die_info->pll_reset_n = -1;
		die_info->hash_reset_n = -1;
		die_info->pll_locked = -1;
		return 0;
	} else {
		return -1;
	}
}

int knc_decode_report(uint8_t *response, struct knc_report *report, int version)
{
/*
 * reserved     2 bits
 * next_state   1 bit   next work state loaded
 * state        1 bit   hashing  (0 on Jupiter)
 * next_slot    4 bit   slot id of next work state (0 on Jupiter)
 * progress     8 bits  upper 8 bits of nonce counter
 * active_slot  4 bits  slot id of current work state
 * nonce_slot   4 bits  slot id of found nonce
 * nonce        32 bits
 * 
 * reserved     4 bits
 * nonce_slot   4 bits
 * nonce        32 bits
 */
	report->next_state = (response[0] >> 5) & 1;
	if (version != KNC_VERSION_JUPITER) {
		report->state = (response[0] >> 4) & 1;
		report->next_slot = response[0] & ((1<<4)-1);
	} else {
		report->state = -1;
		report->next_slot = -1;
	}
	report->progress = (uint32_t)response[1] << 24;
	report->active_slot = (response[2] >> 4) & ((1<<4)-1);
	int n;
	int n_nonces = version == KNC_VERSION_JUPITER ? 1 : 5;
	for (n = 0; n < n_nonces; n++) {
		report->nonce[n].slot = response[2+n*5] & ((1<<4)-1);
		report->nonce[n].nonce =
				(uint32_t)response[3+n*5] << 24 |
				(uint32_t)response[4+n*5] << 16 |
				(uint32_t)response[5+n*5] << 8 |
				(uint32_t)response[6+n*5] << 0 |
				0;
	}
	for (; n < KNC_NONCES_PER_REPORT; n++) {
		report->nonce[n].slot = -1;
		report->nonce[n].nonce = 0;
	}
	return 0;
}

int knc_decode_work_status(uint8_t *response, uint8_t *num_request_busy, uint16_t *num_status_byte_error)
{
	if (response[0] != 0xFF)
		return -1;
	*num_request_busy = response[1];
	int i;
	for (i = 0 ; i < KNC_STATUS_BYTE_ERROR_COUNTERS ; i++)
		num_status_byte_error[i] = 256 * response[2 + 2*i] + response[2 + 2*i + 1];
	return 0;
}

char * get_asicname_from_version(enum asic_version version)
{
	switch(version) {
	case KNC_VERSION_JUPITER:
		return "JUPITER";
	case KNC_VERSION_NEPTUNE:
		return "NEPTUNE";
	case KNC_VERSION_TITAN:
		return "TITAN";
	default:
		return "UNKNOWN";
	}
}

int knc_detect_die_(int log_level, void *ctx, int channel, int die, struct knc_die_info *die_info)
{
	uint8_t request[4];
	int response_len = 2 + 2 + 4 + 4 + (KNC_MAX_CORES_PER_DIE*2 + 7) / 8;
	uint8_t response[response_len];

	int request_len = knc_prepare_info(request, die, die_info, &response_len);
	int status = knc_syncronous_transfer(ctx, channel, request_len, request, response_len, response);

	/* Find the right request size based on ASIC generation */
	int cores_in_die = response[0]<<8 | response[1];
	int version = response[2]<<8 | response[3];
	if (status != 0) {
		if (version == KNC_ASIC_VERSION_NEPTUNE && cores_in_die <= KNC_CORES_PER_DIE_NEPTUNE) {
			applog(log_level, "KnC %d-%d: Looks like a NEPTUNE die with %d cores", channel, die, cores_in_die);
			/* Try again with right response size */
			response_len = 2 + 2 + 4 + 4 + (cores_in_die*2 + 7) / 8;
			status = knc_syncronous_transfer(ctx, channel, request_len, request, response_len, response);
		} else if (version == KNC_ASIC_VERSION_TITAN && cores_in_die <= KNC_CORES_PER_DIE_TITAN) {
			applog(log_level, "KnC %d-%d: Looks like a TITAN die with %d cores", channel, die, cores_in_die);
			/* Try again with right response size */
			response_len = 2 + 2 + 4 + 4 + (cores_in_die*2 + 7) / 8;
			status = knc_syncronous_transfer(ctx, channel, request_len, request, response_len, response);
		}
	}
	int rc = -1;
	if (version == KNC_ASIC_VERSION_JUPITER || status == 0)
		rc = knc_decode_info(response, die_info);
	if (rc == 0)
		applog(log_level, "KnC %d-%d: Found %s die with %d cores", channel, die,
		       get_asicname_from_version(die_info->version),
		       die_info->cores);
	else
		applog(log_level, "KnC %d-%d: No KnC chip found", channel, die);
	return rc;
}

int knc_detect_die(void *ctx, int channel, int die, struct knc_die_info *die_info)
{
	return knc_detect_die_(LOG_DEBUG, ctx, channel, die, die_info);
}

bool fill_in_thread_params(int num_threads, struct titan_setup_core_params *params)
{
	struct thread_params_t {
		uint8_t thread_enable;
		uint16_t thread_base_address[KNC_TITAN_THREADS_PER_CORE];
		uint16_t lookup_gap_mask[KNC_TITAN_THREADS_PER_CORE];
	};
	/* Parameters for different number of threads */
	const struct thread_params_t thread_params[KNC_TITAN_THREADS_PER_CORE + 1] = {
		[2] = {
			.thread_enable = 0x11,
			.thread_base_address = {0, 0, 0, 0, 1, 1, 1, 1},
			.lookup_gap_mask = {0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1},
		},
		[3] = {
			.thread_enable = 0x15,
			.thread_base_address = {0, 0, 0, 0, 1, 1, 3, 3},
			.lookup_gap_mask = {0x1, 0x1, 0x1, 0x1, 0x3, 0x3, 0x3, 0x3},
		},
		[4] = {
			.thread_enable = 0x55,
			.thread_base_address = {0, 0, 1, 1, 2, 2, 3, 3},
			.lookup_gap_mask = {0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3},
		},
		[5] = {
			.thread_enable = 0x57,
			.thread_base_address = {0, 0, 1, 1, 2, 2, 3, 7},
			.lookup_gap_mask = {0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x7, 0x7},
		},
		[6] = {
			.thread_enable = 0x77,
			.thread_base_address = {0, 0, 1, 5, 2, 2, 3, 7},
			.lookup_gap_mask = {0x3, 0x3, 0x7, 0x7, 0x3, 0x3, 0x7, 0x7},
		},
		[7] = {
			.thread_enable = 0x7F,
			.thread_base_address = {0, 0, 1, 5, 2, 6, 3, 7},
			.lookup_gap_mask = {0x3, 0x3, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7},
		},
		[8] = {
			.thread_enable = 0xFF,
			.thread_base_address = {0, 1, 2, 3, 4, 5, 6, 7},
			.lookup_gap_mask = {0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7},
		},
	};

	if ((2 > num_threads) || (KNC_TITAN_THREADS_PER_CORE < num_threads)) {
		applog(LOG_ERR, "Number of threads must be between %d and %d!", 2, KNC_TITAN_THREADS_PER_CORE);
		return false;
	}

	params->thread_enable = thread_params[num_threads].thread_enable;
	int i;
	for (i = 0; i < KNC_TITAN_THREADS_PER_CORE; ++i) {
		params->thread_base_address[i] = thread_params[num_threads].thread_base_address[i];
		params->lookup_gap_mask[i] = thread_params[num_threads].lookup_gap_mask[i];
	}

	return true;
}

bool knc_titan_setup_core_(int log_level, void * const ctx, int channel, int die, int core, struct titan_setup_core_params *params)
{
#define	SETWORK_CMD_SIZE	(5 + BLOCK_HEADER_BYTES_WITHOUT_NONCE)
	/* The size of command is the same as for set_work */
	uint8_t setup_core_cmd[SETWORK_CMD_SIZE] = {
		KNC_ASIC_CMD_SETUP_CORE,
		die,
		(core >> 8) & 0xFF,
		core & 0xFF,
		/* next follows padding and data */
	};
	const int send_size = sizeof(setup_core_cmd);
	int response_length = send_size - 4;
	uint8_t response[response_length];
	int status;
	uint32_t *src, *dst;
	int i;
	struct titan_packed_core_params {
		/* WORD [0] */
#if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
		uint32_t padding			:26;
		uint32_t bad_address_mask_0_6msb	:6;
#else
		uint32_t bad_address_mask_0_6msb	:6;
		uint32_t padding			:26;
#endif
		/* WORD [1] */
#if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
		uint32_t bad_address_mask_0_4lsb	:4;
		uint32_t bad_address_mask_1		:10;
		uint32_t bad_address_match_0		:10;
		uint32_t bad_address_match_1_8msb	:8;
#else
		uint32_t bad_address_match_1_8msb	:8;
		uint32_t bad_address_match_0		:10;
		uint32_t bad_address_mask_1		:10;
		uint32_t bad_address_mask_0_4lsb	:4;
#endif
		/* WORD [2] */
#if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
		uint32_t bad_address_match_1_2lsb	:2;
		uint32_t difficulty			:6;
		uint32_t thread_enable			:8;
		uint32_t thread_base_address_0		:10;
		uint32_t thread_base_address_1_6msb	:6;
#else
		uint32_t thread_base_address_1_6msb	:6;
		uint32_t thread_base_address_0		:10;
		uint32_t thread_enable			:8;
		uint32_t difficulty			:6;
		uint32_t bad_address_match_1_2lsb	:2;
#endif
		/* WORD [3] */
#if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
		uint32_t thread_base_address_1_4lsb	:4;
		uint32_t thread_base_address_2		:10;
		uint32_t thread_base_address_3		:10;
		uint32_t thread_base_address_4_8msb	:8;
#else
		uint32_t thread_base_address_4_8msb	:8;
		uint32_t thread_base_address_3		:10;
		uint32_t thread_base_address_2		:10;
		uint32_t thread_base_address_1_4lsb	:4;
#endif
		/* WORD [4] */
#if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
		uint32_t thread_base_address_4_2lsb	:2;
		uint32_t thread_base_address_5		:10;
		uint32_t thread_base_address_6		:10;
		uint32_t thread_base_address_7		:10;
#else
		uint32_t thread_base_address_7		:10;
		uint32_t thread_base_address_6		:10;
		uint32_t thread_base_address_5		:10;
		uint32_t thread_base_address_4_2lsb	:2;
#endif
		/* WORD [5] */
#if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
		uint32_t lookup_gap_mask_0		:10;
		uint32_t lookup_gap_mask_1		:10;
		uint32_t lookup_gap_mask_2		:10;
		uint32_t lookup_gap_mask_3_2msb		:2;
#else
		uint32_t lookup_gap_mask_3_2msb		:2;
		uint32_t lookup_gap_mask_2		:10;
		uint32_t lookup_gap_mask_1		:10;
		uint32_t lookup_gap_mask_0		:10;
#endif
		/* WORD [6] */
#if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
		uint32_t lookup_gap_mask_3_8lsb		:8;
		uint32_t lookup_gap_mask_4		:10;
		uint32_t lookup_gap_mask_5		:10;
		uint32_t lookup_gap_mask_6_4msb		:4;
#else
		uint32_t lookup_gap_mask_6_4msb		:4;
		uint32_t lookup_gap_mask_5		:10;
		uint32_t lookup_gap_mask_4		:10;
		uint32_t lookup_gap_mask_3_8lsb		:8;
#endif
		/* WORD [7] */
#if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
		uint32_t lookup_gap_mask_6_6lsb		:6;
		uint32_t lookup_gap_mask_7		:10;
		uint32_t N_mask_0			:10;
		uint32_t N_mask_1_6msb			:6;
#else
		uint32_t N_mask_1_6msb			:6;
		uint32_t N_mask_0			:10;
		uint32_t lookup_gap_mask_7		:10;
		uint32_t lookup_gap_mask_6_6lsb		:6;
#endif
		/* WORD [8] */
#if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
		uint32_t N_mask_1_4lsb			:4;
		uint32_t N_mask_2			:10;
		uint32_t N_mask_3			:10;
		uint32_t N_mask_4_8msb			:8;
#else
		uint32_t N_mask_4_8msb			:8;
		uint32_t N_mask_3			:10;
		uint32_t N_mask_2			:10;
		uint32_t N_mask_1_4lsb			:4;
#endif
		/* WORD [9] */
#if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
		uint32_t N_mask_4_2lsb			:2;
		uint32_t N_mask_5			:10;
		uint32_t N_mask_6			:10;
		uint32_t N_mask_7			:10;
#else
		uint32_t N_mask_7			:10;
		uint32_t N_mask_6			:10;
		uint32_t N_mask_5			:10;
		uint32_t N_mask_4_2lsb			:2;
#endif
		/* WORD [10] */
#if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
		uint32_t N_shift_0			:4;
		uint32_t N_shift_1			:4;
		uint32_t N_shift_2			:4;
		uint32_t N_shift_3			:4;
		uint32_t N_shift_4			:4;
		uint32_t N_shift_5			:4;
		uint32_t N_shift_6			:4;
		uint32_t N_shift_7			:4;
#else
		uint32_t N_shift_7			:4;
		uint32_t N_shift_6			:4;
		uint32_t N_shift_5			:4;
		uint32_t N_shift_4			:4;
		uint32_t N_shift_3			:4;
		uint32_t N_shift_2			:4;
		uint32_t N_shift_1			:4;
		uint32_t N_shift_0			:4;
#endif
		/* WORD [11] */
		uint32_t nonce_top;
		/* WORD [12] */
		uint32_t nonce_bottom;
	} __attribute__((packed)) packed_params;

	packed_params.padding = 0;
	packed_params.bad_address_mask_0_6msb = (params->bad_address_mask[0] >> 4) & 0x03F;
	packed_params.bad_address_mask_0_4lsb = params->bad_address_mask[0] & 0x00F;
	packed_params.bad_address_mask_1 = params->bad_address_mask[1];
	packed_params.bad_address_match_0 = params->bad_address_match[0];
	packed_params.bad_address_match_1_8msb = (params->bad_address_match[1] >> 2) & 0x0FF;
	packed_params.bad_address_match_1_2lsb = params->bad_address_match[1] & 0x003;
	packed_params.difficulty = params->difficulty;
	packed_params.thread_enable = params->thread_enable;
	packed_params.thread_base_address_0 = params->thread_base_address[0];
	packed_params.thread_base_address_1_6msb = (params->thread_base_address[1] >> 4) & 0x03F;
	packed_params.thread_base_address_1_4lsb = params->thread_base_address[1] & 0x00F;
	packed_params.thread_base_address_2 = params->thread_base_address[2];
	packed_params.thread_base_address_3 = params->thread_base_address[3];
	packed_params.thread_base_address_4_8msb = (params->thread_base_address[4] >> 2) & 0x0FF;
	packed_params.thread_base_address_4_2lsb = params->thread_base_address[4] & 0x003;
	packed_params.thread_base_address_5 = params->thread_base_address[5];
	packed_params.thread_base_address_6 = params->thread_base_address[6];
	packed_params.thread_base_address_7 = params->thread_base_address[7];
	packed_params.lookup_gap_mask_0 = params->lookup_gap_mask[0];
	packed_params.lookup_gap_mask_1 = params->lookup_gap_mask[1];
	packed_params.lookup_gap_mask_2 = params->lookup_gap_mask[2];
	packed_params.lookup_gap_mask_3_2msb = (params->lookup_gap_mask[3] >> 8) & 0x003;
	packed_params.lookup_gap_mask_3_8lsb = params->lookup_gap_mask[3] & 0x0FF;
	packed_params.lookup_gap_mask_4 = params->lookup_gap_mask[4];
	packed_params.lookup_gap_mask_5 = params->lookup_gap_mask[5];
	packed_params.lookup_gap_mask_6_4msb = (params->lookup_gap_mask[6] >> 6) & 0x00F;
	packed_params.lookup_gap_mask_6_6lsb = params->lookup_gap_mask[6] & 0x03F;
	packed_params.lookup_gap_mask_7 = params->lookup_gap_mask[7];
	packed_params.N_mask_0 = params->N_mask[0];
	packed_params.N_mask_1_6msb = (params->N_mask[1] >> 4) & 0x03F;
	packed_params.N_mask_1_4lsb = params->N_mask[1] & 0x00F;
	packed_params.N_mask_2 = params->N_mask[2];
	packed_params.N_mask_3 = params->N_mask[3];
	packed_params.N_mask_4_8msb = (params->N_mask[4] >> 2) & 0x0FF;
	packed_params.N_mask_4_2lsb = params->N_mask[4] & 0x003;
	packed_params.N_mask_5 = params->N_mask[5];
	packed_params.N_mask_6 = params->N_mask[6];
	packed_params.N_mask_7 = params->N_mask[7];
	packed_params.N_shift_0 = params->N_shift[0];
	packed_params.N_shift_1 = params->N_shift[1];
	packed_params.N_shift_2 = params->N_shift[2];
	packed_params.N_shift_3 = params->N_shift[3];
	packed_params.N_shift_4 = params->N_shift[4];
	packed_params.N_shift_5 = params->N_shift[5];
	packed_params.N_shift_6 = params->N_shift[6];
	packed_params.N_shift_7 = params->N_shift[7];
	packed_params.nonce_top = params->nonce_top;
	packed_params.nonce_bottom = params->nonce_bottom;

	src = (uint32_t *)&packed_params;
	dst = (uint32_t *)(&setup_core_cmd[send_size - sizeof(packed_params)]);
	for (i = 0; i < (int)(sizeof(packed_params) / 4); ++i)
		dst[i] = htobe32(src[i]);

	knc_syncronous_transfer(ctx, channel, send_size, setup_core_cmd, response_length, response);
	/* For this command ASIC does not send CRC and status byte in response, so ignore it */

	/* Check the success of the command by reading core report */
	uint8_t request[4];
	response_length = 1 + 1 + (1 + 4) * 5;
	int request_length = knc_prepare_report(request, die, core);
	status = knc_syncronous_transfer(ctx, channel, request_length, request, response_length, response);
	if (status) {
		applog(log_level, "KnC %d-%d: Failed (%x)", channel, die, status);
		return false;
	}
	struct knc_report report;
	knc_decode_report(response, &report, KNC_VERSION_TITAN);
	if (report.progress != (params->nonce_bottom & 0xFF000000)) {
		applog(log_level, "KnC %d-%d: Failed to set nonce range (wanted 0x%02X, get 0x%02X)", channel, die, params->nonce_bottom >> 24, report.progress >> 24);
		return false;
	}

	return true;
}

/* Use nonce range to detect if core accepted configuration */
bool knc_titan_setup_core(void * const ctx, int channel, int die, int core, struct titan_setup_core_params *params)
{
	struct titan_setup_core_params tmp = *params;
	if (params->nonce_bottom < 0x01000000) {
		tmp.nonce_bottom = 0xFF000000;
		tmp.nonce_top = 0xFFFFFFFF;
	} else {
		tmp.nonce_bottom = 0x00000000;
		tmp.nonce_top = 0x00FFFFFFFF;
	}
	if (!knc_titan_setup_core_(LOG_ERR, ctx, channel, die, core, &tmp))
		return false;
	return knc_titan_setup_core_(LOG_ERR, ctx, channel, die, core, params);
}
