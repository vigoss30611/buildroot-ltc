/*
 * hal_audio_clock.c
 *
 *  Created on: Jun 28, 2010
 *      Author: klabadi & dlopo
 */

#include "halAudioClock.h"
#include "access.h"
#include "log.h"

/* register offsets */
static const u8 AUD_N1 = 0x00;
static const u8 AUD_N2 = 0x01;
static const u8 AUD_N3 = 0x02;
static const u8 AUD_CTS1 = 0x03;
static const u8 AUD_CTS2 = 0x04;
static const u8 AUD_CTS3 = 0x05;
static const u8 AUD_INPUTCLKFS = 0x06;

void halAudioClock_N(u16 baseAddress, u32 value)
{
	LOG_TRACE();
	/* 19-bit width */
	access_CoreWriteByte((u8) (value >> 0), baseAddress + AUD_N1);
	access_CoreWriteByte((u8) (value >> 8), baseAddress + AUD_N2);
	access_CoreWrite((u8) (value >> 16), baseAddress + AUD_N3, 0, 3);
	/* no shift */
	access_CoreWrite(0, baseAddress + AUD_CTS3, 5, 3);
}

/*
 * @param value: new CTS value or set to zero for automatic mode
 */
void halAudioClock_Cts(u16 baseAddress, u32 value)
{
	LOG_TRACE1(value);
	/* manual or automatic? must be programmed first */
	access_CoreWrite((value != 0) ? 1 : 0, baseAddress + AUD_CTS3, 4, 1);
	/* 19-bit width */
	access_CoreWriteByte((u8) (value >> 0), baseAddress + AUD_CTS1);
	access_CoreWriteByte((u8) (value >> 8), baseAddress + AUD_CTS2);
	access_CoreWrite((u8) (value >> 16), baseAddress + AUD_CTS3, 0, 3);
}

void halAudioClock_F(u16 baseAddress, u8 value)
{
	LOG_TRACE();
	access_CoreWrite(value, baseAddress + AUD_INPUTCLKFS, 0, 3);
}
