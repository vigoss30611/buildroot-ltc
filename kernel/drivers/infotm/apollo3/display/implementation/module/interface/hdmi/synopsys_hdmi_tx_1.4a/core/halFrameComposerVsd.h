/*
 * @file:halFrameComposerVsd.h
 *
 *  Created on: Jul 1, 2010
 *      Author: klabadi & dlopo
 */

#ifndef HALFRAMECOMPOSERVSD_H_
#define HALFRAMECOMPOSERVSD_H_

#include "types.h"
/*
 * Configure the 24 bit IEEE Registration Identifier
 * @param baseAddr Block base address
 * @param id vendor unique identifier
 */
void halFrameComposerVsd_VendorOUI(u16 baseAddr, u32 id);
/*
 * Configure the Vendor Payload to be carried by the InfoFrame
 * @param info array
 * @param length of the array
 * @return 0 when successful and 1 on error
 */
u8 halFrameComposerVsd_VendorPayload(u16 baseAddr, const u8 * data,
		unsigned short length);

#endif /* HALFRAMECOMPOSERVSD_H_ */
