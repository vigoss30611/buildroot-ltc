/******************************************************************************
 *
 * Copyright(c) 2013 Realtek Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 *
 ******************************************************************************/
#include <drv_types.h>

//drivers/infotm/switch/switch_wifi.c
extern void wifi_power(int power);
extern void bt_power(int power);
extern void sdio_scan(int flag);

int platform_wifi_power_on(void)
{
	int ret = 0;
	DBG_871X_LEVEL(_drv_always_, "wifi_power 1\n");
	wifi_power(1);
	msleep(50);
	DBG_871X_LEVEL(_drv_always_, "sdio_scan 1\n");
	sdio_scan(1);
	msleep(50);
	return ret;
}

void platform_wifi_power_off(void)
{
	DBG_871X_LEVEL(_drv_always_, "sdio_scan 0\n");
	sdio_scan(0);
	DBG_871X_LEVEL(_drv_always_, "wifi_power 0\n");
	wifi_power(0);
}

