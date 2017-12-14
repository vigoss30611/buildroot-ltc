/*
 * halAudioSpdif.h
 *
 *  Created on: Jun 30, 2010
 *      Author: klabadi & dlopo
 */

#ifndef HALAUDIOSPDIF_H_
#define HALAUDIOSPDIF_H_

#include "types.h"

void halAudioSpdif_ResetFifo(u16 baseAddr);

void halAudioSpdif_NonLinearPcm(u16 baseAddr, u8 bit);

void halAudioSpdif_DataWidth(u16 baseAddr, u8 value);

void halAudioSpdif_InterruptMask(u16 baseAddr, u8 value);

void halAudioSpdif_InterruptPolarity(u16 baseAddr, u8 value);

#endif /* HALAUDIOSPDIF_H_ */
