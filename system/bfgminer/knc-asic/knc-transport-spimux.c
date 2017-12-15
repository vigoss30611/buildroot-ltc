/*
 * Direct SPI transport layer for KnCminer Jupiters
 *
 * Copyright 2014 KnCminer
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 3 of the License, or (at your option)
 * any later version.  See COPYING for more details.
 */

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <logging.h>
#include <miner.h>
#include "hexdump.c"
#include "knc-transport.h"

#define UNUSED __attribute__((unused))

#ifdef CONTROLLER_BOARD_BBB
#define SPI_DEFAULT_DEVICE	"spidev1.0"
#endif
#ifdef CONTROLLER_BOARD_RPI
#define SPI_DEFAULT_DEVICE	"spidev0.0"
#endif
#ifdef CONTROLLER_BOARD_BACKPLANE
#define SPI_DEFAULT_DEVICE	"UNKNOWN"
#define SPI_MODE		(SPI_CS_HIGH)
#define SPI_MAX_SPEED		600000
#endif

#define SPI_DEVICE_TEMPLATE	"/dev/%s"
#ifndef SPI_MODE
#define SPI_MODE		(SPI_CPHA | SPI_CPOL | SPI_CS_HIGH)
#endif
#define SPI_BITS_PER_WORD	8
#ifndef SPI_MAX_SPEED
#define SPI_MAX_SPEED		3000000
#endif
#define SPI_DELAY_USECS		0

struct spidev_context {
	int fd;
	uint32_t speed;
	uint16_t delay;
	uint8_t mode;
	uint8_t bits;
};

/* Init SPI transport */
void *knc_trnsp_new(const char *devname)
{
	struct spidev_context *ctx;
	char dev_name[PATH_MAX];
	if (!devname)
		devname = SPI_DEFAULT_DEVICE;

	if (NULL == (ctx = malloc(sizeof(struct spidev_context)))) {
		applog(LOG_ERR, "KnC transport: Out of memory");
		goto l_exit_error;
	}
	ctx->mode = SPI_MODE;
	ctx->bits = SPI_BITS_PER_WORD;
	ctx->speed = SPI_MAX_SPEED;
	ctx->delay = SPI_DELAY_USECS;

	ctx->fd = -1;
	if (*devname == '/')
		strcpy(dev_name, devname);
	else
		sprintf(dev_name, SPI_DEVICE_TEMPLATE, devname);
	if (0 > (ctx->fd = open(dev_name, O_RDWR))) {
		applog(LOG_ERR, "KnC transport: Can not open SPI device %s: %m",
		       dev_name);
		goto l_free_exit_error;
	}

	/*
	 * spi mode
	 */
	if (0 > ioctl(ctx->fd, SPI_IOC_WR_MODE, &ctx->mode))
		goto l_ioctl_error;
	if (0 > ioctl(ctx->fd, SPI_IOC_RD_MODE, &ctx->mode))
		goto l_ioctl_error;

	/*
	 * bits per word
	 */
	if (0 > ioctl(ctx->fd, SPI_IOC_WR_BITS_PER_WORD, &ctx->bits))
		goto l_ioctl_error;
	if (0 > ioctl(ctx->fd, SPI_IOC_RD_BITS_PER_WORD, &ctx->bits))
		goto l_ioctl_error;

	/*
	 * max speed hz
	 */
	if (0 > ioctl(ctx->fd, SPI_IOC_WR_MAX_SPEED_HZ, &ctx->speed))
		goto l_ioctl_error;
	if (0 > ioctl(ctx->fd, SPI_IOC_RD_MAX_SPEED_HZ, &ctx->speed))
		goto l_ioctl_error;

	applog(LOG_INFO, "KnC transport: SPI device %s uses mode %hhu, bits %hhu, speed %u",
	       dev_name, ctx->mode, ctx->bits, ctx->speed);

	return ctx;

l_ioctl_error:
	applog(LOG_ERR, "KnC transport: ioctl error on SPI device %s: %m", dev_name);
	close(ctx->fd);
l_free_exit_error:
	free(ctx);
l_exit_error:
	return NULL;
}

void knc_trnsp_free(void *opaque_ctx)
{
	struct spidev_context *ctx = opaque_ctx;

	if (NULL == ctx)
		return;

	close(ctx->fd);
	free(ctx);
}

int knc_trnsp_transfer(void *opaque_ctx, const uint8_t *txbuf, uint8_t *rxbuf, int len)
{
	struct spidev_context *ctx = opaque_ctx;
	struct spi_ioc_transfer xfr;
	int ret;

	memset(rxbuf, 0xff, len);

	ret = len;

	xfr.tx_buf = (unsigned long)txbuf;
	xfr.rx_buf = (unsigned long)rxbuf;
	xfr.len = len;
	xfr.speed_hz = ctx->speed;
	xfr.delay_usecs = ctx->delay;
	xfr.bits_per_word = ctx->bits;
	xfr.cs_change = 0;
	xfr.pad = 0;

        applog(LOG_DEBUG, "KnC spi:");
        hexdump(txbuf, len);
	if (1 > (ret = ioctl(ctx->fd, SPI_IOC_MESSAGE(1), &xfr)))
		applog(LOG_ERR, "KnC spi xfer: ioctl error on SPI device: %m");
        hexdump(rxbuf, len);

	return ret;
}

int knc_trnsp_transfer_multi(void *opaque_ctx, uint8_t **txbuf, uint8_t **rxbuf, int *len, int num)
{
	struct spidev_context *ctx = opaque_ctx;
	struct spi_ioc_transfer *xfr;
	int i, ret = 0;

	if (NULL == (xfr = malloc(sizeof(struct spi_ioc_transfer) * num)))
		return -1;

	for (i = 0; i < num; ++i) {
		memset(rxbuf[i], 0xff, len[i]);
		ret += len[i];

		xfr[i].tx_buf = (unsigned long)txbuf[i];
		xfr[i].rx_buf = (unsigned long)rxbuf[i];
		xfr[i].len = len[i];
		xfr[i].speed_hz = ctx->speed;
		xfr[i].delay_usecs = ctx->delay;
		xfr[i].bits_per_word = ctx->bits;
		xfr[i].cs_change = 0;
		xfr[i].pad = 0;
	}
	applog(LOG_DEBUG, "KnC spi: multi-transfer with %d elements", num);
	if (num > (ret = ioctl(ctx->fd, SPI_IOC_MESSAGE(num), xfr)))
		applog(LOG_ERR, "KnC spi xfer: ioctl error (ret = %d) on SPI device: %m", ret);

	free(xfr);
	return ret;
}

bool knc_trnsp_asic_detect(UNUSED void *opaque_ctx, UNUSED int chip_id)
{
	return true;
}

void knc_trnsp_periodic_check(UNUSED void *opaque_ctx)
{
	return;
}
