/*
 * @file:halFrameComposerSpd.h
 *
 *  Created on: Jul 1, 2010
 *      Author: klabadi & dlopo
 */

#ifndef HALFRAMECOMPOSERSPD_H_
#define HALFRAMECOMPOSERSPD_H_

#include "types.h"
/*
 * Configure Vendor Name to be transmitted in the InfoFrame
 * @param baseAddr Block base address
 * @param vendorName: character array holding the vendor name
 * @param length: of the array
 */
void halFrameComposerSpd_VendorName(u16 baseAddr, const u8 * data,
		unsigned short length);
/*
 * Configure Product Name to be transmitted in the InfoFrame
 * @param baseAddr Block base address
 * @param productName: character array holding the product name
 * @param length: of the array
 */
void halFrameComposerSpd_ProductName(u16 baseAddr, const u8 * data,
		unsigned short length);
/*
 * Code  Source Device Information.
 * 0x00  unknown .
 * 0x01  Digital STB.
 * 0x02  DVD player .
 * 0x03  D-VHS.
 * 0x04  HDD Videorecorder .
 * 0x05  DVC .
 * 0x06  DSC .
 * 0x07  Video CD .
 * 0x08  Game .
 * 0x09  PC general .
 * 0x0A  Blu-Ray Disc (BD) .
 * 0x0B  Super Audio CD .
 * @param baseAddr Block base address
 * @param code
 */
void halFrameComposerSpd_SourceDeviceInfo(u16 baseAddr, u8 code);

#endif /* HALFRAMECOMPOSERSPD_H_ */
