/*
 * @file:halSourcePhy.h
 *
 *  Created on: Jul 1, 2010
 *      Author: klabadi & dlopo
 */

#ifndef HALSOURCEPHY_H_
#define HALSOURCEPHY_H_

#include "types.h"

void halSourcePhy_PowerDown(u16 baseAddr, u8 bit);

void halSourcePhy_EnableTMDS(u16 baseAddr, u8 bit);

void halSourcePhy_Gen2PDDQ(u16 baseAddr, u8 bit);

void halSourcePhy_Gen2TxPowerOn(u16 baseAddr, u8 bit);

void halSourcePhy_Gen2EnHpdRxSense(u16 baseAddr, u8 bit);

void halSourcePhy_DataEnablePolarity(u16 baseAddr, u8 bit);

void halSourcePhy_InterfaceControl(u16 baseAddr, u8 bit);

void halSourcePhy_TestClear(u16 baseAddr, u8 bit);

void halSourcePhy_TestEnable(u16 baseAddr, u8 bit);

void halSourcePhy_TestClock(u16 baseAddr, u8 bit);

void halSourcePhy_TestDataIn(u16 baseAddr, u8 data);

u8 halSourcePhy_TestDataOut(u16 baseAddr);

u8 halSourcePhy_PhaseLockLoopState(u16 baseAddr);

u8 halSourcePhy_HotPlugState(u16 baseAddr);

void halSourcePhy_InterruptMask(u16 baseAddr, u8 value);

u8 halSourcePhy_InterruptMaskStatus(u16 baseAddr, u8 mask);

void halSourcePhy_InterruptPolarity(u16 baseAddr, u8 bitShift, u8 value);

u8 halSourcePhy_InterruptPolarityStatus(u16 baseAddr, u8 mask);


#endif /* HALSOURCEPHY_H_ */
