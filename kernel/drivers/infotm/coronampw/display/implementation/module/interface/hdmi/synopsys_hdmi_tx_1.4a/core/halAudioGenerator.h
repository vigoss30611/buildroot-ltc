/*
 * hal_audio_generator.h
 *
 *  Created on: Jun 29, 2010
 *      Author: klabadi & dlopo
 */

#ifndef HAL_AUDIO_GENERATOR_H_
#define HAL_AUDIO_GENERATOR_H_

#include "types.h"

void halAudioGenerator_SwReset(u16 baseAddress, u8 bit);

void halAudioGenerator_I2sMode(u16 baseAddress, u8 value);

void halAudioGenerator_FreqIncrementLeft(u16 baseAddress, u16 value);

void halAudioGenerator_FreqIncrementRight(u16 baseAddress, u16 value);

void halAudioGenerator_IecCgmsA(u16 baseAddress, u8 value);

void halAudioGenerator_IecCopyright(u16 baseAddress, u8 bit);

void halAudioGenerator_IecCategoryCode(u16 baseAddress, u8 value);

void halAudioGenerator_IecPcmMode(u16 baseAddress, u8 value);

void halAudioGenerator_IecSource(u16 baseAddress, u8 value);

void halAudioGenerator_IecChannelRight(u16 baseAddress, u8 value, u8 channelNo);

void halAudioGenerator_IecChannelLeft(u16 baseAddress, u8 value, u8 channelNo);

void halAudioGenerator_IecClockAccuracy(u16 baseAddress, u8 value);

void halAudioGenerator_IecSamplingFreq(u16 baseAddress, u8 value);

void halAudioGenerator_IecOriginalSamplingFreq(u16 baseAddress, u8 value);

void halAudioGenerator_IecWordLength(u16 baseAddress, u8 value);

void halAudioGenerator_UserRight(u16 baseAddress, u8 bit, u8 channelNo);

void halAudioGenerator_UserLeft(u16 baseAddress, u8 bit, u8 channelNo);

void halAudioGenerator_SpdifValidity(u16 baseAddress, u8 bit);

void halAudioGenerator_SpdifEnable(u16 baseAddress, u8 bit);

void halAudioGenerator_HbrEnable(u16 baseAddress, u8 bit);

void halAudioGenerator_HbrDdrEnable(u16 baseAddress, u8 bit);

/*
 * @param bit: right(1) or left(0)
 */
void halAudioGenerator_HbrDdrChannel(u16 baseAddress, u8 bit);

void halAudioGenerator_HbrBurstLength(u16 baseAddress, u8 value);

void halAudioGenerator_HbrClockDivider(u16 baseAddress, u16 value);
/* 
 * (HW simulation only)
 */
void halAudioGenerator_GpaReplyLatency(u16 baseAddress, u8 value);
/* 
 * (HW simulation only)
 */
void halAudioGenerator_UseLookUpTable(u16 baseAddress, u8 value);

void halAudioGenerator_GpaSampleValid(u16 baseAddress, u8 value, u8 channel);

void halAudioGenerator_ChannelSelect(u16 baseAddress, u8 enable, u8 channel); 

#endif /* HAL_AUDIO_GENERATOR_H_ */
