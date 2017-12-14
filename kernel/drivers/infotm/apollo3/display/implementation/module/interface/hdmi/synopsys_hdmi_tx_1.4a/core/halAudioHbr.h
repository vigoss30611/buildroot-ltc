/*
 * halAudioHbr.h
 *
 *  Created on: Jun 29, 2010
 *      Author: klabadi & dlopo
 */

#ifndef HALAUDIOHBR_H_
#define HALAUDIOHBR_H_

#include "types.h"

void halAudioHbr_ResetFifo(u16 baseAddress);

void halAudioHbr_Select(u16 baseAddress, u8 bit);

void halAudioHbr_InterruptMask(u16 baseAddress, u8 value);

void halAudioHbr_InterruptPolarity(u16 baseAddress, u8 value);

#endif /* HALAUDIOHBR_H_ */
