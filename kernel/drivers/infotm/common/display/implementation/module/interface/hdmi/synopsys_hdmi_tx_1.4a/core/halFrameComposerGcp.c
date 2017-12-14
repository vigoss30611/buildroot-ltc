/*
 * @file:halFrameComposerGcp.c
 *
 *  Created on: Jun 30, 2010
 *      Author: klabadi & dlopo
 */
#include "halFrameComposerGcp.h"
#include "access.h"
#include "log.h"

/* register offset */
static const u8 FC_GCP = 0x18;
/* bit shifts */
static const u8 DEFAULT_PHASE = 2;
static const u8 CLEAR_AVMUTE = 0;
static const u8 SET_AVMUTE = 1;

void halFrameComposerGcp_DefaultPhase(u16 baseAddr, u8 uniform)
{
	LOG_TRACE1(uniform);
	access_CoreWrite((uniform ? 1 : 0), (baseAddr + FC_GCP), DEFAULT_PHASE, 1);
}

void halFrameComposerGcp_AvMute(u16 baseAddr, u8 enable)
{
	LOG_TRACE1(enable);
	access_CoreWrite((enable ? 1 : 0), (baseAddr + FC_GCP), SET_AVMUTE, 1);
	access_CoreWrite((enable ? 0 : 1), (baseAddr + FC_GCP), CLEAR_AVMUTE, 1);
}
