#ifndef _CGMINER_KNC_TRANSPORT_H
#define _CGMINER_KNC_TRANSPORT_H
/*
 * Transport layer interface for KnCminer devices
 *
 * Copyright 2014 KnCminer
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 3 of the License, or (at your option)
 * any later version.  See COPYING for more details.
 */

#include <stdint.h>

#define	MAX_BYTES_IN_SPI_XSFER	4096

void *knc_trnsp_new(const char *devname);
void knc_trnsp_free(void *opaque_ctx);
int knc_trnsp_transfer(void *opaque_ctx, const uint8_t *txbuf, uint8_t *rxbuf, int len);
int knc_trnsp_transfer_multi(void *opaque_ctx, uint8_t **txbuf, uint8_t **rxbuf, int *len, int num);
bool knc_trnsp_asic_detect(void *opaque_ctx, int chip_id);
void knc_trnsp_periodic_check(void *opaque_ctx);

/* API */
#ifdef USE_KNC_TRANSPORT_FTDI
struct api_data *knc_i2c_response(char *param, char *err);
struct api_data *knc_asics_response(char *err);
#endif

#endif
