/*
 * @file:halVideoPacketizer.c
 *
 *  Created on: Jul 1, 2010
 *      Author: klabadi & dlopo
 */
#include "halVideoPacketizer.h"
#include "access.h"
#include "log.h"

/* register offsets */
static const u8 VP_STATUS = 0x00;
static const u8 VP_PR_CD = 0x01;
static const u8 VP_STUFF = 0x02;
static const u8 VP_REMAP = 0x03;
static const u8 VP_CONF = 0x04;

u8 halVideoPacketizer_PixelPackingPhase(u16 baseAddr)
{
	LOG_TRACE();
	return access_CoreRead((baseAddr + VP_STATUS), 0, 4);
}

void halVideoPacketizer_ColorDepth(u16 baseAddr, u8 value)
{
	LOG_TRACE1(value);
	/* color depth */
	access_CoreWrite(value, (baseAddr + VP_PR_CD), 4, 4);
}

void halVideoPacketizer_PixelPackingDefaultPhase(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + VP_STUFF), 5, 1);
}

void halVideoPacketizer_PixelRepetitionFactor(u16 baseAddr, u8 value)
{
	LOG_TRACE1(value);
	/* desired factor */
	access_CoreWrite(value, (baseAddr + VP_PR_CD), 0, 4);
	/* enable stuffing */
	access_CoreWrite(1, (baseAddr + VP_STUFF), 0, 1);
	/* enable block */
	access_CoreWrite((value > 1) ? 1 : 0, (baseAddr + VP_CONF), 4, 1);
	/* bypass block */
	access_CoreWrite((value > 1) ? 0 : 1, (baseAddr + VP_CONF), 2, 1);
}

void halVideoPacketizer_Ycc422RemapSize(u16 baseAddr, u8 value)
{
	LOG_TRACE1(value);
	access_CoreWrite(value, (baseAddr + VP_REMAP), 0, 2);
}

void halVideoPacketizer_OutputSelector(u16 baseAddr, u8 value)
{
	LOG_TRACE1(value);
	if (value == 0)
	{ /* pixel packing */
		access_CoreWrite(0, (baseAddr + VP_CONF), 6, 1);
		/* enable pixel packing */
		access_CoreWrite(1, (baseAddr + VP_CONF), 5, 1);
		access_CoreWrite(0, (baseAddr + VP_CONF), 3, 1);
	}
	else if (value == 1)
	{ /* YCC422 */
		access_CoreWrite(0, (baseAddr + VP_CONF), 6, 1);
		access_CoreWrite(0, (baseAddr + VP_CONF), 5, 1);
		/* enable YCC422 */
		access_CoreWrite(1, (baseAddr + VP_CONF), 3, 1);
	}
	else if (value == 2 || value == 3)
	{ /* bypass */
		/* enable bypass */
		access_CoreWrite(1, (baseAddr + VP_CONF), 6, 1);
		access_CoreWrite(0, (baseAddr + VP_CONF), 5, 1);
		access_CoreWrite(0, (baseAddr + VP_CONF), 3, 1);
	}
	else
	{
		LOG_ERROR2("wrong output option: ", value);
		return;
	}

	/* YCC422 stuffing */
	access_CoreWrite(1, (baseAddr + VP_STUFF), 2, 1);
	/* pixel packing stuffing */
	access_CoreWrite(1, (baseAddr + VP_STUFF), 1, 1);

	/* ouput selector */
	access_CoreWrite(value, (baseAddr + VP_CONF), 0, 2);
}
