/*
 * (C) Copyright 2001
 * Denis Peter, MPL AG Switzerland
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 * Note: Part of this code has been derived from linux
 *
 */
#ifndef _USB_API_H_
#define _USB_API_H_

#define	udisk0_vs_read		udisk_vs_read
#define	udisk1_vs_read		udisk_vs_read
#define	udisk0_vs_write		udisk_vs_write
#define	udisk1_vs_write		udisk_vs_write
extern int udisk_vs_read(uint8_t *buf, loff_t start, int length, int extra);
extern int udisk_vs_write(uint8_t *buf, loff_t start, int length, int extra);
extern int udisk0_vs_align(void);
extern int udisk1_vs_align(void);
extern int udisk0_vs_reset(void);
extern int udisk1_vs_reset(void);
extern int udisk1_vs_capacity(void);
extern int udisk1_vs_capacity(void);
#endif /*_USB_API_H_ */
