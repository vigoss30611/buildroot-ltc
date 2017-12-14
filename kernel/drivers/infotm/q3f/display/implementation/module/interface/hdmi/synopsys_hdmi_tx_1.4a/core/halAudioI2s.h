/*
 * hal_audio_i2s.h
 *
 *  Created on: Jun 29, 2010
 *      Author: klabadi & dlopo
 */

#ifndef HALAUDIOI2S_H_
#define HALAUDIOI2S_H_

#include "types.h"

void halAudioI2s_ResetFifo(u16 baseAddress);

void halAudioI2s_Select(u16 baseAddress, u8 bit);

void halAudioI2s_DataEnable(u16 baseAddress, u8 value);

void halAudioI2s_DataMode(u16 baseAddress, u8 value);

void halAudioI2s_DataWidth(u16 baseAddress, u8 value);

void halAudioI2s_InterruptMask(u16 baseAddress, u8 value);

void halAudioI2s_InterruptPolarity(u16 baseAddress, u8 value);

#endif /* HALAUDIOI2S_H_ */
