/*
 * halAudioSpdif.c
 *
 *  Created on: Jun 30, 2010
 *      Author: klabadi & dlopo
 */

#include "halAudioSpdif.h"
#include "access.h"
#include "log.h"
/* register offsets */
static const u8 AUD_SPDIF0 = 0x00;
static const u8 AUD_SPDIF1 = 0x01;
static const u8 AUD_SPDIFINT = 0x02;

void halAudioSpdif_ResetFifo(u16 baseAddr)
{
	LOG_TRACE();
	access_CoreWrite(1, baseAddr + AUD_SPDIF0, 7, 1);
}

void halAudioSpdif_NonLinearPcm(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, baseAddr + AUD_SPDIF1, 7, 1);
}

void halAudioSpdif_DataWidth(u16 baseAddr, u8 value)
{
	LOG_TRACE1(value);
	access_CoreWrite(value, baseAddr + AUD_SPDIF1, 0, 5);
}

void halAudioSpdif_InterruptMask(u16 baseAddr, u8 value)
{
	LOG_TRACE1(value);
	access_CoreWrite(value, baseAddr + AUD_SPDIFINT, 2, 2);
}

void halAudioSpdif_InterruptPolarity(u16 baseAddr, u8 value)
{
	LOG_TRACE1(value);
	access_CoreWrite(value, baseAddr + AUD_SPDIFINT, 0, 2);
}
