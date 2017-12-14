/*
 * @file:halFrameComposerDebug.c
 *
 *  Created on: Jun 30, 2010
 *      Author: klabadi & dlopo
 */
#include "halFrameComposerDebug.h"
#include "access.h"
#include "log.h"

/* register offsets */
static const u8 FC_DBGFORCE = 0x00;
static const u8 FC_DBGTMDS0 = 0x19;
static const u8 FC_DBGTMDS1 = 0x1A;
static const u8 FC_DBGTMDS2 = 0x1B;

void halFrameComposerDebug_ForceAudio(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, baseAddr + FC_DBGFORCE, 4, 1);
}

void halFrameComposerDebug_ForceVideo(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	/* avoid glitches */
	if (bit != 0)
	{
		access_CoreWriteByte(bit ? 0x00 : 0x00, baseAddr + FC_DBGTMDS2); /* R */
		access_CoreWriteByte(bit ? 0x00 : 0x00, baseAddr + FC_DBGTMDS1); /* G */
		access_CoreWriteByte(bit ? 0xFF : 0x00, baseAddr + FC_DBGTMDS0); /* B */
		access_CoreWrite(bit, baseAddr + FC_DBGFORCE, 0, 1);
	}
	else
	{
		access_CoreWrite(bit, baseAddr + FC_DBGFORCE, 0, 1);
		access_CoreWriteByte(bit ? 0x00 : 0x00, baseAddr + FC_DBGTMDS2); /* R */
		access_CoreWriteByte(bit ? 0x00 : 0x00, baseAddr + FC_DBGTMDS1); /* G */
		access_CoreWriteByte(bit ? 0xFF : 0x00, baseAddr + FC_DBGTMDS0); /* B */
	}
}
