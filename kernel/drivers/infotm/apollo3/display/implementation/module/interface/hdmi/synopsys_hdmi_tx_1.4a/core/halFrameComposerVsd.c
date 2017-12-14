/*
 * @file:halFrameComposerVsd.c
 *
 *  Created on: Jul 1, 2010
 *      Author: klabadi & dlopo
 */
#include "halFrameComposerVsd.h"
#include "access.h"
#include "log.h"

/* register offsets */
static const u8 FC_VSDIEEEID0 = 0x29;
static const u8 FC_VSDIEEEID1 = 0x30;
static const u8 FC_VSDIEEEID2 = 0x31;
static const u8 FC_VSDPAYLOAD0 = 0x32;

void halFrameComposerVsd_VendorOUI(u16 baseAddr, u32 id)
{
	LOG_TRACE1(id);
	access_CoreWriteByte(id, (baseAddr + FC_VSDIEEEID0));
	access_CoreWriteByte(id >> 8, (baseAddr + FC_VSDIEEEID1));
	access_CoreWriteByte(id >> 16, (baseAddr + FC_VSDIEEEID2));
}

u8 halFrameComposerVsd_VendorPayload(u16 baseAddr, const u8 * data,
		unsigned short length)
{
	const unsigned short size = 24;
	unsigned i = 0;
	LOG_TRACE();
	if (data == 0)
	{
		LOG_WARNING("invalid parameter");
		return 1;
	}
	if (length > size)
	{
		length = size;
		LOG_WARNING("vendor payload truncated");
	}
	for (i = 0; i < length; i++)
	{
		access_CoreWriteByte(data[i], (baseAddr + FC_VSDPAYLOAD0 + i));
	}
	return 0;
}
