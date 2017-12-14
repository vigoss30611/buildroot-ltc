/*
 * halAudioGpa.c
 *
 * Created on Oct. 30th 2010
 * Synopsys Inc.
 */
#include "halAudioGpa.h"
#include "access.h"
#include "log.h"

static const u16 GP_CONF0 = 0x00;
static const u16 GP_CONF1 = 0x01;
static const u16 GP_CONF2 = 0x02;
static const u16 GP_STAT  = 0x03;

static const u16 GP_MASK  = 0x06;
static const u16 GP_POL   = 0x05;

void halAudioGpa_ResetFifo(u16 baseAddress)
{
 	LOG_TRACE();
	access_CoreWrite(1, baseAddress + GP_CONF0, 0, 1);
}

void halAudioGpa_ChannelEnable(u16 baseAddress, u8 enable, u8 channel)
{
	LOG_TRACE();
	access_CoreWrite(enable, baseAddress + GP_CONF1, channel, 1);
}

void halAudioGpa_HbrEnable(u16 baseAddress, u8 enable)
{
	int i = 0;
 	LOG_TRACE();
	access_CoreWrite(enable, baseAddress + GP_CONF2, 0, 1);
	/* 8 channels must be enabled */
	if (enable == 1)
	{
		for (i = 0; i < 8; i++)
			halAudioGpa_ChannelEnable(baseAddress, i, 1);
	}
}

u8 halAudioGpa_InterruptStatus(u16 baseAddress)
{
	LOG_TRACE();
	return access_CoreRead(baseAddress + GP_STAT, 0, 2);
}

void halAudioGpa_InterruptMask(u16 baseAddress, u8 value)
{
	LOG_TRACE();
	access_CoreWrite(value, baseAddress + GP_MASK, 0, 2);
}

void halAudioGpa_InterruptPolarity(u16 baseAddress, u8 value)
{
	LOG_TRACE();
	access_CoreWrite(value, baseAddress + GP_POL, 0, 2);
}
