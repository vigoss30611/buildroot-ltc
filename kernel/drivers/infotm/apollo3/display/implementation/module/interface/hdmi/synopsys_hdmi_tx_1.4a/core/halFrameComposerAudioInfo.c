/*
 * @file:halFrameComposerAudioInfo.c
 *
 *  Created on: Jun 30, 2010
 *      Author: klabadi & dlopo
 */
#include "halFrameComposerAudioInfo.h"
#include "access.h"
#include "log.h"

static const u8 FC_AUDICONF0 = 0X25;
static const u8 FC_AUDICONF1 = 0X26;
static const u8 FC_AUDICONF2 = 0X27;
static const u8 FC_AUDICONF3 = 0X28;
static const u8 CHANNEL_COUNT = 4;
static const u8 CODING_TYPE = 0;
static const u8 SAMPLING_SIZE = 4;
static const u8 SAMPLING_FREQ = 0;
static const u8 CHANNEL_ALLOCATION = 0;
static const u8 LEVEL_SHIFT_VALUE = 0;
static const u8 DOWN_MIX_INHIBIT = 4;

void halFrameComposerAudioInfo_ChannelCount(u16 baseAddr, u8 noOfChannels)
{
	LOG_TRACE1(noOfChannels);
	access_CoreWrite(noOfChannels, baseAddr + FC_AUDICONF0, CHANNEL_COUNT, 3);
}

void halFrameComposerAudioInfo_SampleFreq(u16 baseAddr, u8 sf)
{
	LOG_TRACE1(sf);
	access_CoreWrite(sf, baseAddr + FC_AUDICONF1, SAMPLING_FREQ, 3);
}

void halFrameComposerAudioInfo_AllocateChannels(u16 baseAddr, u8 ca)
{
	LOG_TRACE1(ca);
	access_CoreWriteByte(ca, baseAddr + FC_AUDICONF2);
}

void halFrameComposerAudioInfo_LevelShiftValue(u16 baseAddr, u8 lsv)
{
	LOG_TRACE1(lsv);
	access_CoreWrite(lsv, baseAddr + FC_AUDICONF3, LEVEL_SHIFT_VALUE, 4);
}

void halFrameComposerAudioInfo_DownMixInhibit(u16 baseAddr, u8 prohibited)
{
	LOG_TRACE1(prohibited);
	access_CoreWrite((prohibited ? 1 : 0), baseAddr + FC_AUDICONF3,
			DOWN_MIX_INHIBIT, 1);
}

void halFrameComposerAudioInfo_CodingType(u16 baseAddr, u8 codingType)
{
	LOG_TRACE1(codingType);
	access_CoreWrite(codingType, baseAddr + FC_AUDICONF0, CODING_TYPE, 4);
}

void halFrameComposerAudioInfo_SamplingSize(u16 baseAddr, u8 ss)
{
	LOG_TRACE1(ss);
	access_CoreWrite(ss, baseAddr + FC_AUDICONF1, SAMPLING_SIZE, 2);
}
