/*
 * halColorSpaceConverter.h
 *
 *  Created on: Jun 30, 2010
 *      Author: klabadi & dlopo
 */

#ifndef HALCOLORSPACECONVERTER_H_
#define HALCOLORSPACECONVERTER_H_

#include "types.h"

void halColorSpaceConverter_Interpolation(u16 baseAddr, u8 value);

void halColorSpaceConverter_Decimation(u16 baseAddr, u8 value);

void halColorSpaceConverter_ColorDepth(u16 baseAddr, u8 value);

void halColorSpaceConverter_ScaleFactor(u16 baseAddr, u8 value);

void halColorSpaceConverter_CoefficientA1(u16 baseAddr, u16 value);

void halColorSpaceConverter_CoefficientA2(u16 baseAddr, u16 value);

void halColorSpaceConverter_CoefficientA3(u16 baseAddr, u16 value);

void halColorSpaceConverter_CoefficientA4(u16 baseAddr, u16 value);

void halColorSpaceConverter_CoefficientB1(u16 baseAddr, u16 value);

void halColorSpaceConverter_CoefficientB2(u16 baseAddr, u16 value);

void halColorSpaceConverter_CoefficientB3(u16 baseAddr, u16 value);

void halColorSpaceConverter_CoefficientB4(u16 baseAddr, u16 value);

void halColorSpaceConverter_CoefficientC1(u16 baseAddr, u16 value);

void halColorSpaceConverter_CoefficientC2(u16 baseAddr, u16 value);

void halColorSpaceConverter_CoefficientC3(u16 baseAddr, u16 value);

void halColorSpaceConverter_CoefficientC4(u16 baseAddr, u16 value);

#endif /* HALCOLORSPACECONVERTER_H_ */
