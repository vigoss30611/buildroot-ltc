/*
 * @file:audio.c
 *
 *  Created on: Jul 2, 2010
 *  Modified: Oct 2010: GPA interface added
 *  Synopsys Inc.
 */
#include "audio.h"
#include "halFrameComposerAudio.h"
#include "halAudioI2s.h"
#include "halAudioSpdif.h"
#include "halAudioHbr.h"
#include "halAudioGpa.h"
#include "halAudioClock.h"
#include "halAudioGenerator.h"
#include "log.h"

/* block offsets */
static const u16 FC_BASE_ADDR = 0x1000;
static const u16 AUD_BASE_ADDR = 0x3100;
static const u16 AG_BASE_ADDR = 0x7100;
/* interface offset*/
static const u16 AUDIO_I2S   = 0x0000;
static const u16 AUDIO_CLOCK = 0x0100;
static const u16 AUDIO_SPDIF = 0x0200;
static const u16 AUDIO_HBR   = 0x0300;
static const u16 AUDIO_GPA   = 0x0400;
/* bit shifts */
static const u8 VALIDITY_BIT = 1;
static const u8 USER_BIT = 0;

int audio_Initialize(u16 baseAddr)
{
	LOG_TRACE();
	return audio_Mute(baseAddr, 1);
}

int audio_Configure(u16 baseAddr, audioParams_t *params, u16 pixelClk, unsigned int ratioClk)
{
	u16 i = 0;
	
	LOG_TRACE();
	/* more than 2 channels => layout 1 else layout 0 */
	halFrameComposerAudio_PacketLayout(baseAddr + FC_BASE_ADDR,
			((audioParams_ChannelCount(params) + 1) > 2) ? 1 : 0);
	/* iec validity and user bits (IEC 60958-1 */
	for (i = 0; i < 4; i++)
	{
		/* audioParams_IsChannelEn considers left as 1 channel and
		 * right as another (+1), hence the x2 factor in the following */
		/* validity bit is 0 when reliable, which is !IsChannelEn */
		halFrameComposerAudio_ValidityRight(baseAddr + FC_BASE_ADDR,
				!(audioParams_IsChannelEn(params, (2 * i))), i);
		halFrameComposerAudio_ValidityLeft(baseAddr + FC_BASE_ADDR,
				!(audioParams_IsChannelEn(params, (2 * i) + 1)), i);
		halFrameComposerAudio_UserRight(baseAddr + FC_BASE_ADDR,
				USER_BIT, i);
		halFrameComposerAudio_UserLeft(baseAddr + FC_BASE_ADDR,
				USER_BIT, i);
	}

	/* IEC - not needed if non-linear PCM */
	halFrameComposerAudio_IecCgmsA(baseAddr + FC_BASE_ADDR,
			audioParams_GetIecCgmsA(params));
	halFrameComposerAudio_IecCopyright(baseAddr + FC_BASE_ADDR,
			audioParams_GetIecCopyright(params) ? 0 : 1);
	halFrameComposerAudio_IecCategoryCode(baseAddr + FC_BASE_ADDR,
			audioParams_GetIecCategoryCode(params));
	halFrameComposerAudio_IecPcmMode(baseAddr + FC_BASE_ADDR,
			audioParams_GetIecPcmMode(params));
	halFrameComposerAudio_IecSource(baseAddr + FC_BASE_ADDR,
			audioParams_GetIecSourceNumber(params));
	for (i = 0; i < 4; i++)
	{ /* 0, 1, 2, 3 */
		halFrameComposerAudio_IecChannelLeft(baseAddr + FC_BASE_ADDR,
				2* i + 1, i); /* 1, 3, 5, 7 */
		halFrameComposerAudio_IecChannelRight(baseAddr + FC_BASE_ADDR,
				2* (i + 1), i); /* 2, 4, 6, 8 */
	}
	halFrameComposerAudio_IecClockAccuracy(baseAddr + FC_BASE_ADDR, audioParams_GetIecClockAccuracy(params));
    halFrameComposerAudio_IecSamplingFreq(baseAddr + FC_BASE_ADDR, audioParams_IecSamplingFrequency(params));
    halFrameComposerAudio_IecOriginalSamplingFreq(baseAddr + FC_BASE_ADDR, audioParams_IecOriginalSamplingFrequency(params));
    halFrameComposerAudio_IecWordLength(baseAddr + FC_BASE_ADDR, audioParams_IecWordLength(params));

	halAudioI2s_Select(baseAddr + AUD_BASE_ADDR + AUDIO_I2S, (audioParams_GetInterfaceType(params) == I2S)? 1 : 0);
 	/*
 	 * ATTENTION: fixed I2S data enable configuration
 	 * is equivalent to 0x1 for 1 or 2 channels
 	 * is equivalent to 0x3 for 3 or 4 channels
 	 * is equivalent to 0x7 for 5 or 6 channels
 	 * is equivalent to 0xF for 7 or 8 channels
 	 */
 	halAudioI2s_DataEnable(baseAddr + AUD_BASE_ADDR + AUDIO_I2S, 0xF);
 	/* ATTENTION: fixed I2S data mode (standard) */
 	halAudioI2s_DataMode(baseAddr + AUD_BASE_ADDR + AUDIO_I2S, 0);
 	halAudioI2s_DataWidth(baseAddr + AUD_BASE_ADDR + AUDIO_I2S, audioParams_GetSampleSize(params));
 	halAudioI2s_InterruptMask(baseAddr + AUD_BASE_ADDR + AUDIO_I2S, 3);
 	halAudioI2s_InterruptPolarity(baseAddr + AUD_BASE_ADDR + AUDIO_I2S, 3);
 	halAudioI2s_ResetFifo(baseAddr + AUD_BASE_ADDR + AUDIO_I2S);

 	halAudioSpdif_NonLinearPcm(baseAddr + AUD_BASE_ADDR + AUDIO_SPDIF, audioParams_IsLinearPCM(params)? 0 : 1);
 	halAudioSpdif_DataWidth(baseAddr + AUD_BASE_ADDR + AUDIO_SPDIF, audioParams_GetSampleSize(params));
 	halAudioSpdif_InterruptMask(baseAddr + AUD_BASE_ADDR + AUDIO_SPDIF, 3);
 	halAudioSpdif_InterruptPolarity(baseAddr + AUD_BASE_ADDR + AUDIO_SPDIF, 3);
 	halAudioSpdif_ResetFifo(baseAddr + AUD_BASE_ADDR + AUDIO_SPDIF);

 	halAudioHbr_Select(baseAddr + AUD_BASE_ADDR + AUDIO_HBR, (audioParams_GetInterfaceType(params) == HBR)? 1 : 0);
 	halAudioHbr_InterruptMask(baseAddr + AUD_BASE_ADDR + AUDIO_HBR, 7);
 	halAudioHbr_InterruptPolarity(baseAddr + AUD_BASE_ADDR + AUDIO_HBR, 7);
 	halAudioHbr_ResetFifo(baseAddr + AUD_BASE_ADDR + AUDIO_HBR);

	if (audioParams_GetInterfaceType(params) == GPA)
	{
		for (i = 0; i < 8; i++)
		{
			halAudioGpa_ChannelEnable(baseAddr + AUD_BASE_ADDR + AUDIO_GPA, audioParams_IsChannelEn(params, i), i);
		}
		halAudioGpa_HbrEnable(baseAddr + AUD_BASE_ADDR + AUDIO_GPA, audioParams_GetPacketType(params) == HBR_STREAM? 1: 0);
		halAudioGpa_InterruptMask(baseAddr + AUD_BASE_ADDR + AUDIO_GPA, 3);
		halAudioGpa_InterruptPolarity(baseAddr + AUD_BASE_ADDR + AUDIO_GPA, 3);
		halAudioGpa_ResetFifo(baseAddr + AUD_BASE_ADDR + AUDIO_GPA);
	}
 	halAudioClock_N(baseAddr + AUD_BASE_ADDR + AUDIO_CLOCK, audio_ComputeN(baseAddr, audioParams_GetSamplingFrequency(params), pixelClk, ratioClk));
	/* if NOT GPA interface, use automatic mode, else, compute CTS */
 	halAudioClock_Cts(baseAddr + AUD_BASE_ADDR + AUDIO_CLOCK, (audioParams_GetInterfaceType(params) != GPA)? 0: audio_ComputeCts(baseAddr, audioParams_GetSamplingFrequency(params), pixelClk, ratioClk));
	if (audioParams_GetInterfaceType(params) != GPA)
	{
		switch (audioParams_GetClockFsFactor(params))
		{
			/* This version does not support DRU Bypass - found in controller 1v30a
			 *	0 128Fs		I2S- (SPDIF when DRU in bypass)
			 *	1 256Fs		I2S-
			 *	2 512Fs		I2S-HBR-(SPDIF - when NOT DRU bypass)
			 *	3 1024Fs	I2S-(SPDIF - when NOT DRU bypass)
			 *	4 64Fs		I2S
			 */
			case 64:
				/* William
				if (audioParams_GetInterfaceType(params) != I2S)
				{
					return FALSE;
				}
				*/
				halAudioClock_F(baseAddr + AUD_BASE_ADDR + AUDIO_CLOCK, 4);
				break;
			case 128:
				/* William
				if (audioParams_GetInterfaceType(params) != I2S)
				{
					return FALSE;
				}
				*/
				halAudioClock_F(baseAddr + AUD_BASE_ADDR + AUDIO_CLOCK, 0);
				break;
			case 256:
				/* William
				if (audioParams_GetInterfaceType(params) != I2S)
				{
					return FALSE;
				}
				*/
				halAudioClock_F(baseAddr + AUD_BASE_ADDR + AUDIO_CLOCK, 1);
				break;
			case 512:
				halAudioClock_F(baseAddr + AUD_BASE_ADDR + AUDIO_CLOCK, 2);
				break;
			case 1024:
				if (audioParams_GetInterfaceType(params) == HBR)
				{
					return FALSE;
				}
				halAudioClock_F(baseAddr + AUD_BASE_ADDR + AUDIO_CLOCK, 3);
				break;
			default:
				/* Fs clock factor not supported */
				return FALSE;
		}
	}
#if 0
 	if (audio_AudioGenerator(baseAddr, params) != TRUE)
 	{
 		return FALSE;
 	}
#endif
 	return audio_Mute(baseAddr, 0);
 }

int audio_Mute(u16 baseAddr, u8 state)
{
	/* audio mute priority: AVMUTE, sample flat, validity */
	/* AVMUTE also mutes video */
	halFrameComposerAudio_PacketSampleFlat(baseAddr + FC_BASE_ADDR,
			state ? 0xF : 0);
	return TRUE;
}

int audio_AudioGenerator(u16 baseAddr, audioParams_t *params)
{
	/*
	 * audio generator block is not included in real application hardware,
	 * Then the external audio sources are used and this code has no effect
	 */
	u32 tmp;
	LOG_TRACE();
	halAudioGenerator_I2sMode(baseAddr + AG_BASE_ADDR, 1); /* should be coherent with I2S config? */
	tmp = (1500 * 65535) / audioParams_GetSamplingFrequency(params); /* 1500Hz */
	halAudioGenerator_FreqIncrementLeft(baseAddr + AG_BASE_ADDR, (u16) (tmp));
	tmp = (3000 * 65535) / audioParams_GetSamplingFrequency(params); /* 3000Hz */
	halAudioGenerator_FreqIncrementRight(baseAddr + AG_BASE_ADDR,
			(u16) (tmp));

	halAudioGenerator_IecCgmsA(baseAddr + AG_BASE_ADDR,
			audioParams_GetIecCgmsA(params));
	halAudioGenerator_IecCopyright(baseAddr + AG_BASE_ADDR,
			audioParams_GetIecCopyright(params) ? 0 : 1);
	halAudioGenerator_IecCategoryCode(baseAddr + AG_BASE_ADDR,
			audioParams_GetIecCategoryCode(params));
	halAudioGenerator_IecPcmMode(baseAddr + AG_BASE_ADDR,
			audioParams_GetIecPcmMode(params));
	halAudioGenerator_IecSource(baseAddr + AG_BASE_ADDR,
			audioParams_GetIecSourceNumber(params));
	for (tmp = 0; tmp < 4; tmp++)
	{
		/* 0 -> iec spec 60958-3 means "do not take into account" */
		halAudioGenerator_IecChannelLeft(baseAddr + AG_BASE_ADDR, 0, tmp);
		halAudioGenerator_IecChannelRight(baseAddr + AG_BASE_ADDR, 0, tmp);
		/* USER_BIT 0 default by spec */
		halAudioGenerator_UserLeft(baseAddr + AG_BASE_ADDR, USER_BIT, tmp);
		halAudioGenerator_UserRight(baseAddr + AG_BASE_ADDR, USER_BIT, tmp);
	}
	halAudioGenerator_IecClockAccuracy(baseAddr + AG_BASE_ADDR,
			audioParams_GetIecClockAccuracy(params));
	halAudioGenerator_IecSamplingFreq(baseAddr + AG_BASE_ADDR,
			audioParams_IecSamplingFrequency(params));
	halAudioGenerator_IecOriginalSamplingFreq(baseAddr + AG_BASE_ADDR,
			audioParams_IecOriginalSamplingFrequency(params));
	halAudioGenerator_IecWordLength(baseAddr + AG_BASE_ADDR,
			audioParams_IecWordLength(params));

	halAudioGenerator_SpdifValidity(baseAddr + AG_BASE_ADDR,
			VALIDITY_BIT);
	halAudioGenerator_SpdifEnable(baseAddr + AG_BASE_ADDR, 1); /* enabled by default but usage depends on core selection */

	/* HBR is synthesizable but not audible */
	halAudioGenerator_HbrDdrEnable(baseAddr + AG_BASE_ADDR, 0);
	halAudioGenerator_HbrDdrChannel(baseAddr + AG_BASE_ADDR, 0);
	halAudioGenerator_HbrBurstLength(baseAddr + AG_BASE_ADDR, 0);
	halAudioGenerator_HbrClockDivider(baseAddr + AG_BASE_ADDR, 128); /* 128 * fs */
	halAudioGenerator_HbrEnable(baseAddr + AG_BASE_ADDR, 1);
	/* GPA */
	for (tmp = 0; tmp < 8; tmp++)
	{
		halAudioGenerator_GpaSampleValid(baseAddr + AG_BASE_ADDR, !(audioParams_IsChannelEn(params, tmp)), tmp);
		halAudioGenerator_ChannelSelect(baseAddr + AG_BASE_ADDR, audioParams_IsChannelEn(params, tmp), tmp);
	}
	halAudioGenerator_GpaReplyLatency(baseAddr + AG_BASE_ADDR, 0x03);
	halAudioGenerator_SwReset(baseAddr + AG_BASE_ADDR, 1);
	return TRUE;
}

u16 audio_ComputeN(u16 baseAddr, u32 freq, u16 pixelClk, u16 ratioClk)
{
	u32 n = (128 * freq) / 1000;

	switch (freq)
	{
	case 32000:
		if (pixelClk == 2517)
			n = (ratioClk == 150) ? 9152 : 4576;
		else if (pixelClk == 2702)
			n = (ratioClk == 150) ? 8192 : 4096;
		else if (pixelClk == 7417 || pixelClk == 14835)
			n = 11648;
		else
			n = 4096;
		break;
	case 44100:
		if (pixelClk == 2517)
			n = 7007;
		else if (pixelClk == 7417)
			n = 17836;
		else if (pixelClk == 14835)
			n = (ratioClk == 150) ? 17836 : 8918;
		else
			n = 6272;
		break;
	case 48000:
		if (pixelClk == 2517)
			n = (ratioClk == 150) ? 9152 : 6864;
		else if (pixelClk == 2702)
			n = (ratioClk == 150) ? 8192 : 6144;
		else if (pixelClk == 7417)
			n = 11648;
		else if (pixelClk == 14835)
			n = (ratioClk == 150) ? 11648 : 5824;
		else
			n = 6144;
		break;
	case 88200:
		n = audio_ComputeN(baseAddr, 44100, pixelClk, ratioClk) * 2;
		break;
	case 96000:
		n = audio_ComputeN(baseAddr, 48000, pixelClk, ratioClk) * 2;
		break;
	case 176400:
		n = audio_ComputeN(baseAddr, 44100, pixelClk, ratioClk) * 4;
		break;
	case 192000:
		n = audio_ComputeN(baseAddr, 48000, pixelClk, ratioClk) * 4;
		break;
	default:
		break;
	}
	return n;
}

u32 audio_ComputeCts(u16 baseAddr, u32 freq, u16 pixelClk, u16 ratioClk)
{
	u32 cts = 0;
	switch(freq)
	{
		case 32000:
			if (pixelClk == 29700)
			{
				cts = 222750;
				break;
			}
		case 48000:
		case 96000:
		case 192000:
			switch (pixelClk)
			{
				case 2520:
				case 2700:
				case 5400:
				case 7425:
				case 14850:
					cts = pixelClk;
					break;
				case 29700:
					cts = 247500;
					break;
				default:
					/* All other TMDS clocks are not supported by DWC_hdmi_tx
					 * the TMDS clocks divided or multiplied by 1,001
					 * coefficients are not supported.
					 */
					break;
			}
			break;
		case 44100:
		case 88200:
		case 176400:
			switch (pixelClk)
			{
				case 2520:
					cts = 28000;
					break;
				case 2700:
					cts = 30000;
					break;
				case 5400:
					cts = 60000;
					break;
				case 7425:
					cts = 82500;
					break;				
				case 14850:
					cts = 165000;
					break;					
				case 29700:
					cts = 247500;
					break;					
				default:
					break;
			}
			break;
		default:
			break;
	}
	return (cts * ratioClk) / 100;
}
