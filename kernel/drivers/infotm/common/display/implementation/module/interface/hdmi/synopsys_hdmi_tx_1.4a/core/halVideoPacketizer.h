/*
 * @file:halVideoPacketizer.h
 *
 *  Created on: Jul 1, 2010
 *      Author: klabadi & dlopo
 */

#ifndef HALVIDEOPACKETIZER_H_
#define HALVIDEOPACKETIZER_H_

#include "types.h"

u8 halVideoPacketizer_PixelPackingPhase(u16 baseAddr);

void halVideoPacketizer_ColorDepth(u16 baseAddr, u8 value);

void halVideoPacketizer_PixelPackingDefaultPhase(u16 baseAddr, u8 bit);

void halVideoPacketizer_PixelRepetitionFactor(u16 baseAddr, u8 value);

void halVideoPacketizer_Ycc422RemapSize(u16 baseAddr, u8 value);

void halVideoPacketizer_OutputSelector(u16 baseAddr, u8 value);

#endif /* HALVIDEOPACKETIZER_H_ */
