/*
 * include/linux/power/ricoh61x_battery_init.h
 *
 * Battery initial parameter for RICOH R5T618/619 power management chip.
 *
 * Copyright (C) 2012-2013 RICOH COMPANY,LTD
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef __LINUX_POWER_RICOH61X_BATTERY_INIT_H
#define __LINUX_POWER_RICOH61X_BATTERY_INIT_H


uint8_t battery_init_para[][32] = {
	{
		0x0A, 0x21, 0x0B, 0xD3, 0x0B, 0xFB, 0x0C, 0x19,
		0x0C, 0x36, 0x0C, 0x54, 0x0C, 0x79, 0x0C, 0xB0,
		0x0C, 0xDB, 0x0C, 0xFF, 0x0D, 0x55, 0x0A, 0xF0,
		0x00, 0x4F, 0x0F, 0xC8, 0x05, 0x2C, 0x22, 0x56
	}
};

#endif
