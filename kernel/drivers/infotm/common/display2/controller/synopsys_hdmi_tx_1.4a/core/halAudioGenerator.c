/*
 * hal_audio_generator.c
 *
 *  Created on: Jun 29, 2010
 *      Author: klabadi & dlopo
 */

#include "halAudioGenerator.h"
#include "access.h"
#include "log.h"

/* register offsets */
static const u8 AG_SWRSTZ = 0x00;
static const u8 AG_MODE = 0x01;
static const u8 AG_INCLEFT0 = 0x02;
static const u8 AG_INCLEFT1 = 0x03;
static const u8 AG_INCRIGHT0 = 0x04;
static const u8 AG_SPDIF_AUDSCHNLS2 = 0x12;
static const u8 AG_INCRIGHT1 = 0x05;
static const u8 AG_SPDIF_CONF = 0x16;
static const u8 AG_SPDIF_AUDSCHNLS1 = 0x11;
static const u8 AG_SPDIF_AUDSCHNLS0 = 0x10;
static const u8 AG_SPDIF_AUDSCHNLS3 = 0x13;
static const u8 AG_SPDIF_AUDSCHNLS5 = 0x15;
static const u8 AG_SPDIF_AUDSCHNLS4 = 0x14;
static const u8 AG_HBR_CONF = 0x17;
static const u8 AG_HBR_CLKDIV0 = 0x18;
static const u8 AG_HBR_CLKDIV1 = 0x19;
static const u8 AG_GPA_CONF0 = 0x1A;
static const u8 AG_GPA_CONF1 = 0x1B;
static const u8 AG_GPA_SAMPLEVALID = 0x1C;
static const u8 AG_GPA_CHNUM1 = 0x1D;
static const u8 AG_GPA_CHNUM2 = 0x1E;
static const u8 AG_GPA_CHNUM3 = 0x1F;
static const u8 AG_GPA_USERBIT = 0x20;
static const u8 AG_GPA_SAMPLE_LSB = 0x21;
static const u8 AG_GPA_SAMPLE_MSB = 0x22;
static const u8 AG_GPA_SAMPLE_DIFF = 0x23;
static const u8 AG_GPA_INT = 0x24;

void halAudioGenerator_SwReset(u16 baseAddress, u8 bit)
{
	LOG_TRACE1(bit);
	/* active low */
	access_CoreWrite(bit ? 0 : 1, baseAddress + AG_SWRSTZ, 0, 1);
}

void halAudioGenerator_I2sMode(u16 baseAddress, u8 value)
{
	LOG_TRACE1(value);
	access_CoreWrite(value, baseAddress + AG_MODE, 0, 3);
}

void halAudioGenerator_FreqIncrementLeft(u16 baseAddress, u16 value)
{
	LOG_TRACE1(value);
	access_CoreWriteByte((u8) (value), baseAddress + AG_INCLEFT0);
	access_CoreWriteByte(value >> 8, baseAddress + AG_INCLEFT1);
}

void halAudioGenerator_FreqIncrementRight(u16 baseAddress, u16 value)
{
	LOG_TRACE1(value);
	access_CoreWriteByte((u8) (value), baseAddress + AG_INCRIGHT0);
	access_CoreWriteByte(value >> 8, baseAddress + AG_INCRIGHT1);
}

void halAudioGenerator_IecCgmsA(u16 baseAddress, u8 value)
{
	LOG_TRACE1(value);
	access_CoreWrite(value, baseAddress + AG_SPDIF_AUDSCHNLS0, 1, 2);
}

void halAudioGenerator_IecCopyright(u16 baseAddress, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, baseAddress + AG_SPDIF_AUDSCHNLS0, 0, 1);
}

void halAudioGenerator_IecCategoryCode(u16 baseAddress, u8 value)
{
	LOG_TRACE1(value);
	access_CoreWriteByte(value, baseAddress + AG_SPDIF_AUDSCHNLS1);
}

void halAudioGenerator_IecPcmMode(u16 baseAddress, u8 value)
{
	LOG_TRACE1(value);
	access_CoreWrite(value, baseAddress + AG_SPDIF_AUDSCHNLS2, 4, 3);
}

void halAudioGenerator_IecSource(u16 baseAddress, u8 value)
{
	LOG_TRACE1(value);
	access_CoreWrite(value, baseAddress + AG_SPDIF_AUDSCHNLS2, 0, 4);
}

void halAudioGenerator_IecChannelRight(u16 baseAddress, u8 value, u8 channelNo)
{
	LOG_TRACE1(value);
	switch (channelNo)
	{
		case 0:
			access_CoreWrite(value, baseAddress + AG_SPDIF_AUDSCHNLS3, 4, 4);
			break;
		case 1:
			access_CoreWrite(value, baseAddress + AG_GPA_CHNUM1, 4, 4);
			break;
		case 2:
			access_CoreWrite(value, baseAddress + AG_GPA_CHNUM2, 4, 4);
			break;
		case 3:
			access_CoreWrite(value, baseAddress + AG_GPA_CHNUM3, 4, 4);
			break;
		default:
			LOG_ERROR("wrong channel number");
			break;
	}
	
}

void halAudioGenerator_IecChannelLeft(u16 baseAddress, u8 value, u8 channelNo)
{
	LOG_TRACE1(value);
	switch (channelNo)
	{
		case 0:
			access_CoreWrite(value, baseAddress + AG_SPDIF_AUDSCHNLS3, 0, 4);
			break;
		case 1:
			access_CoreWrite(value, baseAddress + AG_GPA_CHNUM1, 0, 4);
			break;
		case 2:
			access_CoreWrite(value, baseAddress + AG_GPA_CHNUM2, 0, 4);
			break;
		case 3:
			access_CoreWrite(value, baseAddress + AG_GPA_CHNUM3, 0, 4);
			break;
		default:
			LOG_ERROR("wrong channel number");
			break;
	}	
}

void halAudioGenerator_IecClockAccuracy(u16 baseAddress, u8 value)
{
	LOG_TRACE1(value);
	access_CoreWrite(value, baseAddress + AG_SPDIF_AUDSCHNLS4, 4, 2);
}

void halAudioGenerator_IecSamplingFreq(u16 baseAddress, u8 value)
{
	LOG_TRACE1(value);
	access_CoreWrite(value, baseAddress + AG_SPDIF_AUDSCHNLS4, 0, 4);
}

void halAudioGenerator_IecOriginalSamplingFreq(u16 baseAddress, u8 value)
{
	LOG_TRACE1(value);
	access_CoreWrite(value, baseAddress + AG_SPDIF_AUDSCHNLS5, 4, 4);
}

void halAudioGenerator_IecWordLength(u16 baseAddress, u8 value)
{
	LOG_TRACE1(value);
	access_CoreWrite(value, baseAddress + AG_SPDIF_AUDSCHNLS5, 0, 4);
}

void halAudioGenerator_UserRight(u16 baseAddress, u8 bit, u8 channelNo)
{
	LOG_TRACE1(bit);
	switch (channelNo)
	{
		case 0:
			access_CoreWrite(bit, baseAddress + AG_SPDIF_CONF, 3, 1);
			break;
		case 1:
			access_CoreWrite(bit, baseAddress + AG_GPA_USERBIT, 1, 1);
			break;
		case 2:
			access_CoreWrite(bit, baseAddress + AG_GPA_USERBIT, 2, 1);
			break;
		case 3:
			access_CoreWrite(bit, baseAddress + AG_GPA_USERBIT, 5, 1);
			break;
		default:
			LOG_ERROR("wrong channel number");
			break;
	}
}

void halAudioGenerator_UserLeft(u16 baseAddress, u8 bit, u8 channelNo)
{
	LOG_TRACE1(bit);
	switch (channelNo)
	{
		case 0:
			access_CoreWrite(bit, baseAddress + AG_SPDIF_CONF, 2, 1);
			break;
		case 1:
			access_CoreWrite(bit, baseAddress + AG_GPA_USERBIT, 0, 1);
			break;
		case 2:
			access_CoreWrite(bit, baseAddress + AG_GPA_USERBIT, 2, 1);
			break;
		case 3:
			access_CoreWrite(bit, baseAddress + AG_GPA_USERBIT, 4, 1);
			break;
		default:
			LOG_ERROR("wrong channel number");
			break;
	}
}

void halAudioGenerator_SpdifValidity(u16 baseAddress, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, baseAddress + AG_SPDIF_CONF, 1, 1);
}

void halAudioGenerator_SpdifEnable(u16 baseAddress, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, baseAddress + AG_SPDIF_CONF, 0, 1);
}

void halAudioGenerator_HbrEnable(u16 baseAddress, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, baseAddress + AG_HBR_CONF, 0, 1);
}

void halAudioGenerator_HbrDdrEnable(u16 baseAddress, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, baseAddress + AG_HBR_CONF, 1, 1);
}

void halAudioGenerator_HbrDdrChannel(u16 baseAddress, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, baseAddress + AG_HBR_CONF, 2, 1);
}

void halAudioGenerator_HbrBurstLength(u16 baseAddress, u8 value)
{
	LOG_TRACE1(value);
	access_CoreWrite(value, baseAddress + AG_HBR_CONF, 3, 4);
}

void halAudioGenerator_HbrClockDivider(u16 baseAddress, u16 value)
{
	LOG_TRACE1(value);
	/* 9-bit width */
	access_CoreWriteByte((u8) (value), baseAddress + AG_HBR_CLKDIV0);
	access_CoreWrite(value >> 8, baseAddress + AG_HBR_CLKDIV1, 0, 1);
}

void halAudioGenerator_UseLookUpTable(u16 baseAddress, u8 value)
{
	access_CoreWrite(value, baseAddress + AG_GPA_CONF0, 5, 1);
}

void halAudioGenerator_GpaReplyLatency(u16 baseAddress, u8 value)
{
	access_CoreWrite(value, baseAddress + AG_GPA_CONF0, 0, 2);
}

void halAudioGenerator_ChannelSelect(u16 baseAddress, u8 enable, u8 channel)
{
	LOG_TRACE2(channel, enable);
	access_CoreWrite(enable, baseAddress + AG_GPA_CONF1, channel, 1);
}

void halAudioGenerator_GpaSampleValid(u16 baseAddress, u8 value, u8 channelNo)
{
		access_CoreWrite(value, baseAddress + AG_GPA_SAMPLEVALID, channelNo, 1);	
}
