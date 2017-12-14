/*
 * @file:halVideoGenerator.h
 *
 *  Created on: Jul 1, 2010
 *      Author: klabadi & dlopo
 */

#ifndef HALVIDEOGENERATOR_H_
#define HALVIDEOGENERATOR_H_

#include "types.h"

void halVideoGenerator_SwReset(u16 baseAddr, u8 bit);

void halVideoGenerator_Ycc(u16 baseAddr, u8 bit);

void halVideoGenerator_Ycc422(u16 baseAddr, u8 bit);

void halVideoGenerator_VBlankOsc(u16 baseAddr, u8 bit);

void halVideoGenerator_ColorIncrement(u16 baseAddr, u8 bit);

void halVideoGenerator_Interlaced(u16 baseAddr, u8 bit);

void halVideoGenerator_VSyncPolarity(u16 baseAddr, u8 bit);

void halVideoGenerator_HSyncPolarity(u16 baseAddr, u8 bit);

void halVideoGenerator_DataEnablePolarity(u16 baseAddr, u8 bit);

void halVideoGenerator_ColorResolution(u16 baseAddr, u8 value);

void halVideoGenerator_PixelRepetitionInput(u16 baseAddr, u8 value);

void halVideoGenerator_HActive(u16 baseAddr, u16 value);

void halVideoGenerator_HBlank(u16 baseAddr, u16 value);

void halVideoGenerator_HSyncEdgeDelay(u16 baseAddr, u16 value);

void halVideoGenerator_HSyncPulseWidth(u16 baseAddr, u16 value);

void halVideoGenerator_VActive(u16 baseAddr, u16 value);

void halVideoGenerator_VBlank(u16 baseAddr, u16 value);

void halVideoGenerator_VSyncEdgeDelay(u16 baseAddr, u16 value);

void halVideoGenerator_VSyncPulseWidth(u16 baseAddr, u16 value);

void halVideoGenerator_Enable3D(u16 baseAddr, u8 bit);

void halVideoGenerator_Structure3D(u16 baseAddr, u8 value);

void halVideoGenerator_WriteStart(u16 baseAddr, u16 value);

void halVideoGenerator_WriteData(u16 baseAddr, u8 value);

u16 halVideoGenerator_WriteCurrentAddr(u16 baseAddr);

u8 halVideoGenerator_WriteCurrentByte(u16 baseAddr);

#endif /* HALVIDEOGENERATOR_H_ */
