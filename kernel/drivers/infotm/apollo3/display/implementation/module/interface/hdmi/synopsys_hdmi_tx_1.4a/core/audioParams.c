/*
 * @file:audioParams.c
 *
 *  Created on: Jul 2, 2010
 *      Author: klabadi & dlopo
 */

#include "audioParams.h"

void audioParams_Reset(audioParams_t *params)
{
	params->mInterfaceType = SPDIF;
	params->mCodingType = PCM; 
	params->mChannelAllocation = 0;
	params->mSampleSize = 16;
	params->mSamplingFrequency = 32000; 
	params->mLevelShiftValue = 0;
	params->mDownMixInhibitFlag = 0;
	params->mOriginalSamplingFrequency = 0;
	params->mIecCopyright = 1;
	params->mIecCgmsA = 3;
	params->mIecPcmMode = 0;
	params->mIecCategoryCode = 0;
	params->mIecSourceNumber = 1;
	params->mIecClockAccuracy = 0;
	params->mPacketType = AUDIO_SAMPLE;
	params->mClockFsFactor = 512;
}

 u8 audioParams_GetChannelAllocation(audioParams_t *params)
{
	return params->mChannelAllocation;
}

void audioParams_SetChannelAllocation(audioParams_t *params, u8 value)
{
	params->mChannelAllocation = value;
}

 u8 audioParams_GetCodingType(audioParams_t *params)
{
	return params->mCodingType;
}

void audioParams_SetCodingType(audioParams_t *params, codingType_t value)
{
	params->mCodingType = value;
}

 u8 audioParams_GetDownMixInhibitFlag(audioParams_t *params)
{
	return params->mDownMixInhibitFlag;
}

void audioParams_SetDownMixInhibitFlag(audioParams_t *params, u8 value)
{
	params->mDownMixInhibitFlag = value;
}

 u8 audioParams_GetIecCategoryCode(audioParams_t *params)
{
	return params->mIecCategoryCode;
}

void audioParams_SetIecCategoryCode(audioParams_t *params, u8 value)
{
	params->mIecCategoryCode = value;
}

 u8 audioParams_GetIecCgmsA(audioParams_t *params)
{
	return params->mIecCgmsA;
}

void audioParams_SetIecCgmsA(audioParams_t *params, u8 value)
{
	params->mIecCgmsA = value;
}

 u8 audioParams_GetIecClockAccuracy(audioParams_t *params)
{
	return params->mIecClockAccuracy;
}

void audioParams_SetIecClockAccuracy(audioParams_t *params, u8 value)
{
	params->mIecClockAccuracy = value;
}

 u8 audioParams_GetIecCopyright(audioParams_t *params)
{
	return params->mIecCopyright;
}

void audioParams_SetIecCopyright(audioParams_t *params, u8 value)
{
	params->mIecCopyright = value;
}

 u8 audioParams_GetIecPcmMode(audioParams_t *params)
{
	return params->mIecPcmMode;
}

void audioParams_SetIecPcmMode(audioParams_t *params, u8 value)
{
	params->mIecPcmMode = value;
}

 u8 audioParams_GetIecSourceNumber(audioParams_t *params)
{
	return params->mIecSourceNumber;
}

void audioParams_SetIecSourceNumber(audioParams_t *params, u8 value)
{
	params->mIecSourceNumber = value;
}

 interfaceType_t audioParams_GetInterfaceType(audioParams_t *params)
{
	return params->mInterfaceType;
}

void audioParams_SetInterfaceType(audioParams_t *params, interfaceType_t value)
{
	params->mInterfaceType = value;
}

 u8 audioParams_GetLevelShiftValue(audioParams_t *params)
{
	return params->mLevelShiftValue;
}

void audioParams_SetLevelShiftValue(audioParams_t *params, u8 value)
{
	params->mLevelShiftValue = value;
}

 u32 audioParams_GetOriginalSamplingFrequency(audioParams_t *params)
{
	return params->mOriginalSamplingFrequency;
}

void audioParams_SetOriginalSamplingFrequency(audioParams_t *params, u32 value)
{
	params->mOriginalSamplingFrequency = value;
}

 u8 audioParams_GetSampleSize(audioParams_t *params)
{
	return params->mSampleSize;
}

void audioParams_SetSampleSize(audioParams_t *params, u8 value)
{
	params->mSampleSize = value;
}

 u32 audioParams_GetSamplingFrequency(audioParams_t *params)
{
	return params->mSamplingFrequency;
}

void audioParams_SetSamplingFrequency(audioParams_t *params, u32 value)
{
	params->mSamplingFrequency = value;
}

 u32 audioParams_AudioClock(audioParams_t *params)
{
	return (params->mClockFsFactor * params->mSamplingFrequency);
}

 u8 audioParams_ChannelCount(audioParams_t *params)
{
	switch (params->mChannelAllocation)
	{
		case 0x00:
			return 1;
		case 0x01:
		case 0x02:
		case 0x04:
			return 2;
		case 0x03:
		case 0x05:
		case 0x06:
		case 0x08:
		case 0x14:
			return 3;
		case 0x07:
		case 0x09:
		case 0x0A:
		case 0x0C:
		case 0x15:
		case 0x16:
		case 0x18:
			return 4;
		case 0x0B:
		case 0x0D:
		case 0x0E:
		case 0x10:
		case 0x17:
		case 0x19:
		case 0x1A:
		case 0x1C:
			return 5;
		case 0x0F:
		case 0x11:
		case 0x12:
		case 0x1B:
		case 0x1D:
		case 0x1E:
			return 6;
		case 0x13:
		case 0x1F:
			return 7;
		default:
			return 0;
	}
}

 u8 audioParams_IecOriginalSamplingFrequency(audioParams_t *params)
{
	/* SPEC values are LSB to MSB */
	switch (params->mOriginalSamplingFrequency)
	{
		case 44100:
			return 0xF;
		case 88200:
			return 0x7;
		case 22050:
			return 0xB;
		case 176400:
			return 0x3;
		case 48000:
			return 0xD;
		case 96000:
			return 0x5;
		case 24000:
			return 0x9;
		case 192000:
			return 0x1;
		case 8000:
			return 0x6;
		case 11025:
			return 0xA;
		case 12000:
			return 0x2;
		case 32000:
			return 0xC;
		case 16000:
			return 0x8;
		default:
			return 0x0; /* not indicated */
	}
}

 u8 audioParams_IecSamplingFrequency(audioParams_t *params)
{
	/* SPEC values are LSB to MSB */
	switch (params->mSamplingFrequency)
	{
		case 22050:
			return 0x4;
		case 44100:
			return 0x0;
		case 88200:
			return 0x8;
		case 176400:
			return 0xC;
		case 24000:
			return 0x6;
		case 48000:
			return 0x2;
		case 96000:
			return 0xA;
		case 192000:
			return 0xE;
		case 32000:
			return 0x3;
		case 768000:
			return 0x9;
		default:
			return 0x1; /* not indicated */
	}
}

 u8 audioParams_IecWordLength(audioParams_t *params)
{
	/* SPEC values are LSB to MSB */
	switch (params->mSampleSize)
	{
		case 20:
			return 0x3; /* or 0xA; */
		case 22:
			return 0x5;
		case 23:
			return 0x9;
		case 24:
			return 0xB;
		case 21:
			return 0xD;
		case 16:
			return 0x2;
		case 18:
			return 0x4;
		case 19:
			return 0x8;
		case 17:
			return 0xC;
		default:
			return 0x0; /* not indicated */
	}
}

 u8 audioParams_IsLinearPCM(audioParams_t *params)
{
	return params->mCodingType == PCM;
}

packet_t audioParams_GetPacketType(audioParams_t *params)
{
	return params->mPacketType;
}

void audioParams_SetPacketType(audioParams_t *params, packet_t packetType)
{
	params->mPacketType = packetType;
}

u8 audioParams_IsChannelEn(audioParams_t *params, u8 channel)
{
	switch (channel)
	{
		case 0:
		case 1:
			return 1;
		case 2:
			return params->mChannelAllocation & BIT(0);
		case 3:
			return params->mChannelAllocation & BIT(1);
		case 4:	
			if ((params->mChannelAllocation > 0x03) && 
				(params->mChannelAllocation < 0x14) && 
				(params->mChannelAllocation > 0x17) && 
				(params->mChannelAllocation < 0x20))
				return 1;
			else 
				return 0;
		case 5:
			if ((params->mChannelAllocation > 0x07) && 
				(params->mChannelAllocation < 0x14) && 
				(params->mChannelAllocation > 0x1C) && 
				(params->mChannelAllocation < 0x20))
				return 1;
			else 
				return 0;
		case 6:
			if ((params->mChannelAllocation > 0x0B) && 
				(params->mChannelAllocation < 0x20))
				return 1;
			else 
				return 0;
		case 7:
			return params->mChannelAllocation & BIT(4);
		default:
			return 0;
	}
}

u16 audioParams_GetClockFsFactor(audioParams_t *params)
{
	return params->mClockFsFactor;
}

void audioParams_SetClockFsFactor(audioParams_t *params, u16 clockFsFactor)
{
	params->mClockFsFactor = clockFsFactor;
}
