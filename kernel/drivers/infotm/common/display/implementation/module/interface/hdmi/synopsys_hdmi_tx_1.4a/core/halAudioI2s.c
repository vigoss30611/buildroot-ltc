/*
 * hal_audio_i2s.c
 *
 *  Created on: Jun 29, 2010
 *      Author: klabadi & dlopo
 */

#include "halAudioI2s.h"
#include "access.h"
#include "log.h"

/* register offsets */
static const u8 AUD_CONF0 = 0x00;
static const u8 AUD_CONF1 = 0x01;
static const u8 AUD_INT = 0x02;

void halAudioI2s_ResetFifo(u16 baseAddress)
{
	LOG_TRACE();
	access_CoreWrite(1, baseAddress + AUD_CONF0, 7, 1);
}

void halAudioI2s_Select(u16 baseAddress, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, baseAddress + AUD_CONF0, 5, 1);
}

void halAudioI2s_DataEnable(u16 baseAddress, u8 value)
{
	LOG_TRACE1(value);
	access_CoreWrite(value, baseAddress + AUD_CONF0, 0, 4);
}

void halAudioI2s_DataMode(u16 baseAddress, u8 value)
{
	LOG_TRACE1(value);
	access_CoreWrite(value, baseAddress + AUD_CONF1, 5, 3);
}

void halAudioI2s_DataWidth(u16 baseAddress, u8 value)
{
	LOG_TRACE1(value);
	access_CoreWrite(value, baseAddress + AUD_CONF1, 0, 5);
}

void halAudioI2s_InterruptMask(u16 baseAddress, u8 value)
{
	LOG_TRACE1(value);
	access_CoreWrite(value, baseAddress + AUD_INT, 2, 2);
}

void halAudioI2s_InterruptPolarity(u16 baseAddress, u8 value)
{
	LOG_TRACE1(value);
	access_CoreWrite(value, baseAddress + AUD_INT, 0, 2);
}

