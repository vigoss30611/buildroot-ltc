/*
 * @file:halMainController.h
 *
 *  Created on: Jul 1, 2010
 *      Author: klabadi & dlopo
 */

#ifndef HALMAINCONTROLLER_H_
#define HALMAINCONTROLLER_H_

#include "types.h"

void halMainController_SfrClockDivision(u16 baseAddr, u8 value);

void halMainController_HdcpClockGate(u16 baseAddr, u8 bit);

void halMainController_CecClockGate(u16 baseAddr, u8 bit);

void halMainController_ColorSpaceConverterClockGate(u16 baseAddr, u8 bit);

void halMainController_AudioSamplerClockGate(u16 baseAddr, u8 bit);

void halMainController_PixelRepetitionClockGate(u16 baseAddr, u8 bit);

void halMainController_TmdsClockGate(u16 baseAddr, u8 bit);

void halMainController_PixelClockGate(u16 baseAddr, u8 bit);

void halMainController_CecClockReset(u16 baseAddr, u8 bit);

void halMainController_AudioGpaReset(u16 baseAddr, u8 bit);

void halMainController_AudioHbrReset(u16 baseAddr, u8 bit);

void halMainController_AudioSpdifReset(u16 baseAddr, u8 bit);

void halMainController_AudioI2sReset(u16 baseAddr, u8 bit);

void halMainController_PixelRepetitionClockReset(u16 baseAddr, u8 bit);

void halMainController_TmdsClockReset(u16 baseAddr, u8 bit);

void halMainController_PixelClockReset(u16 baseAddr, u8 bit);

void halMainController_VideoFeedThroughOff(u16 baseAddr, u8 bit);

void halMainController_PhyReset(u16 baseAddr, u8 bit);

void halMainController_PhyReset(u16 baseAddr, u8 bit);

void halMainController_HeacPhyReset(u16 baseAddr, u8 bit);

u8 halMainController_LockOnClockStatus(u16 baseAddr, u8 clockDomain);

void halMainController_LockOnClockClear(u16 baseAddr, u8 clockDomain);

#endif /* HALMAINCONTROLLER_H_ */
