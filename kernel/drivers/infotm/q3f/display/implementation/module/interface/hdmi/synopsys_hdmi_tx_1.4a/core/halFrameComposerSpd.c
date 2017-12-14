/*
 * @file:halFrameComposerSpd.c
 *
 *  Created on: Jul 1, 2010
 *      Author: klabadi & dlopo
 */
#include "halFrameComposerSpd.h"
#include "access.h"
#include "log.h"

/* register offsets */
static const u8 FC_SPDVENDORNAME0 = 0x4A;
static const u8 FC_SPDPRODUCTNAME0 = 0x52;
static const u8 FC_SPDDEVICEINF = 0x62;

void halFrameComposerSpd_VendorName(u16 baseAddr, const u8 * data,
		unsigned short length)
{
	unsigned short i = 0;
	LOG_TRACE();
	for (i = 0; i < length; i++)
	{
		access_CoreWriteByte(data[i], (baseAddr + FC_SPDVENDORNAME0 + i));
	}
}
void halFrameComposerSpd_ProductName(u16 baseAddr, const u8 * data,
		unsigned short length)
{
	unsigned short i = 0;
	LOG_TRACE();
	for (i = 0; i < length; i++)
	{
		access_CoreWriteByte(data[i], (baseAddr + FC_SPDPRODUCTNAME0 + i));
	}
}

void halFrameComposerSpd_SourceDeviceInfo(u16 baseAddr, u8 code)
{
	LOG_TRACE1(code);
	access_CoreWriteByte(code, (baseAddr + FC_SPDDEVICEINF));
}

