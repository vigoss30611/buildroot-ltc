/*
 * hal_audio_hbr.c
 *
 *  Created on: Jun 29, 2010
 *      Author: klabadi & dlopo
 */

#include "halAudioHbr.h"
#include "access.h"
#include "log.h"

/* register offsets */
static const u8 AUD_CONF0_HBR = 0x00;
static const u8 AUD_HBR_STATUS = 0x01;
static const u8 AUD_HBRINT = 0x02;
static const u8 AUD_HBRPOL = 0x03;
static const u8 AUD_HBRMASK = 0x04;

void halAudioHbr_ResetFifo(u16 baseAddress)
{
	LOG_TRACE();
	access_CoreWrite(1, baseAddress + AUD_CONF0_HBR, 7, 1);
}

void halAudioHbr_Select(u16 baseAddress, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, baseAddress + AUD_CONF0_HBR, 2, 1);
}

void halAudioHbr_InterruptMask(u16 baseAddress, u8 value)
{
	LOG_TRACE1(value);
	access_CoreWrite(value, baseAddress + AUD_HBRMASK, 0, 3);
}

void halAudioHbr_InterruptPolarity(u16 baseAddress, u8 value)
{
	LOG_TRACE1(value);
	access_CoreWrite(value, baseAddress + AUD_HBRPOL, 0, 3);
}
