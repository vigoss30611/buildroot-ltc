/*
 * @file:halInterrupt.h
 *
 *  Created on: Jul 1, 2010
 *      Author: klabadi & dlopo
 */

#ifndef HALINTERRUPT_H_
#define HALINTERRUPT_H_
#include "types.h"

u8 halInterrupt_AudioPacketsState(u16 baseAddr);

void halInterrupt_AudioPacketsClear(u16 baseAddr, u8 value);

u8 halInterrupt_OtherPacketsState(u16 baseAddr);

void halInterrupt_OtherPacketsClear(u16 baseAddr, u8 value);

u8 halInterrupt_PacketsOverflowState(u16 baseAddr);

void halInterrupt_PacketsOverflowClear(u16 baseAddr, u8 value);

u8 halInterrupt_AudioSamplerState(u16 baseAddr);

void halInterrupt_AudioSamplerClear(u16 baseAddr, u8 value);

u8 halInterrupt_PhyState(u16 baseAddr);

void halInterrupt_PhyClear(u16 baseAddr, u8 value);

u8 halInterrupt_I2cDdcState(u16 baseAddr);

void halInterrupt_I2cDdcClear(u16 baseAddr, u8 value);

u8 halInterrupt_CecState(u16 baseAddr);

void halInterrupt_CecClear(u16 baseAddr, u8 value);

u8 halInterrupt_VideoPacketizerState(u16 baseAddr);

void halInterrupt_VideoPacketizerClear(u16 baseAddr, u8 value);

u8 halInterrupt_I2cPhyState(u16 baseAddr);

void halInterrupt_I2cPhyClear(u16 baseAddr, u8 value);

void halInterrupt_Mute(u16 baseAddr, u8 value);

#endif /* HALINTERRUPT_H_ */
