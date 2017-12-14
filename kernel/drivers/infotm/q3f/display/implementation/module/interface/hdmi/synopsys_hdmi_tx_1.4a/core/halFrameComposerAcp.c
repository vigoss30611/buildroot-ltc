/*
 * @file:halFrameComposerAcp.c
 *
 *  Created on: Jun 30, 2010
 *      Author: klabadi & dlopo
 */

#include "halFrameComposerAcp.h"
#include "access.h"
#include "log.h"
/* Frame composer ACP register offsets*/
static const u8 FC_ACP0 = 0x75;
static const u8 FC_ACP1 = 0x91;
static const u8 FC_ACP16 = 0x82;

void halFrameComposerAcp_Type(u16 baseAddr, u8 type)
{
	LOG_TRACE1(type);
	access_CoreWriteByte(type, baseAddr + FC_ACP0);
}

void halFrameComposerAcp_TypeDependentFields(u16 baseAddr, u8 * fields,
		u8 fieldsLength)
{
	u8 c = 0;
	LOG_TRACE1(fields[0]);
	if (fieldsLength > (FC_ACP1 - FC_ACP16 + 1))
	{
		fieldsLength = (FC_ACP1 - FC_ACP16 + 1);
		LOG_WARNING("ACP Fields Truncated");
	}

	for (c = 0; c < fieldsLength; c++)
		access_CoreWriteByte(fields[c], baseAddr + FC_ACP1 - c);
}
