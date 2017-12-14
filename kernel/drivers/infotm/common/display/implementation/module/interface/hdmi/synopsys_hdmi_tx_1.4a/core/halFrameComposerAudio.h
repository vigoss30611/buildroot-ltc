/*
 * @file:halFrameComposerAudio.h
 *
 *  Created on: Jun 30, 2010
 *      Author: klabadi & dlopo
 */

#ifndef HALFRAMECOMPOSERAUDIO_H_
#define HALFRAMECOMPOSERAUDIO_H_

#include "types.h"

void halFrameComposerAudio_PacketSampleFlat(u16 baseAddr, u8 value);

void halFrameComposerAudio_PacketLayout(u16 baseAddr, u8 bit);

void halFrameComposerAudio_ValidityRight(u16 baseAddr, u8 bit,
				unsigned channel);

void halFrameComposerAudio_ValidityLeft(u16 baseAddr, u8 bit, unsigned channel);

void halFrameComposerAudio_UserRight(u16 baseAddr, u8 bit, unsigned channel);

void halFrameComposerAudio_UserLeft(u16 baseAddr, u8 bit, unsigned channel);

void halFrameComposerAudio_IecCgmsA(u16 baseAddr, u8 value);

void halFrameComposerAudio_IecCopyright(u16 baseAddr, u8 bit);

void halFrameComposerAudio_IecCategoryCode(u16 baseAddr, u8 value);

void halFrameComposerAudio_IecPcmMode(u16 baseAddr, u8 value);

void halFrameComposerAudio_IecSource(u16 baseAddr, u8 value);

void halFrameComposerAudio_IecChannelRight(u16 baseAddr, u8 value,
		unsigned channel);

void halFrameComposerAudio_IecChannelLeft(u16 baseAddr, u8 value,
		unsigned channel);

void halFrameComposerAudio_IecClockAccuracy(u16 baseAddr, u8 value);

void halFrameComposerAudio_IecSamplingFreq(u16 baseAddr, u8 value);

void halFrameComposerAudio_IecOriginalSamplingFreq(u16 baseAddr, u8 value);

void halFrameComposerAudio_IecWordLength(u16 baseAddr, u8 value);

#endif /* HALFRAMECOMPOSERAUDIO_H_ */
