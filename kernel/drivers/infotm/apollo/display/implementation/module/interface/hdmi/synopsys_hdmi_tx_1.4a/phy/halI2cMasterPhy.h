/*
 * @file:halI2cMasterPhy.h
 *
 *  Created on: Jul 1, 2010
 *      Author: klabadi & dlopo
 *      NOTE: only write operations are implemented in this module
 *   	This module is used only from PHY GEN2 on
 */

#ifndef HALI2CMASTERPHY_H_
#define HALI2CMASTERPHY_H_

#include "types.h"

void halI2cMasterPhy_SlaveAddress(u16 baseAddr, u8 value);

void halI2cMasterPhy_RegisterAddress(u16 baseAddr, u8 value);

void halI2cMasterPhy_WriteData(u16 baseAddr, u16 value);

void halI2cMasterPhy_WriteRequest(u16 baseAddr);

void halI2cMasterPhy_FastMode(u16 baseAddr, u8 bit);

void halI2cMasterPhy_Reset(u16 baseAddr);

void halI2cMasterPhy_ReadRequest(u16 baseAddr);

u16 halI2cMasterPhy_ReadData(u16 baseAddr);

#endif /* HALI2CMASTERPHY_H_ */
