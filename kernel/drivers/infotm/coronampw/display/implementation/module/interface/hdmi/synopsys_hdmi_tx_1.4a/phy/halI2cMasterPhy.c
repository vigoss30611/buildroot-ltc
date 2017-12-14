/*
 * @file:halI2cMasterPhy.c
 *
 *  Created on: Jul 1, 2010
 *      Author: klabadi & dlopo
 */
#include "halI2cMasterPhy.h"
#include "access.h"
#include "log.h"

/* register offsets */
static const u8 PHY_I2CM_SLAVE = 0x00;
static const u8 PHY_I2CM_ADDRESS = 0x01;
static const u8 PHY_I2CM_DATAO = 0x02;
static const u8 PHY_I2CM_DATAI = 0x04;
static const u8 PHY_I2CM_OPERATION = 0x06;
static const u8 PHY_I2CM_DIV = 0x09;
static const u8 PHY_I2CM_SOFTRSTZ = 0x0A;

void halI2cMasterPhy_SlaveAddress(u16 baseAddr, u8 value)
{
	LOG_TRACE1(value);
	access_CoreWrite(value, (baseAddr + PHY_I2CM_SLAVE), 0, 7);
}

void halI2cMasterPhy_RegisterAddress(u16 baseAddr, u8 value)
{
	LOG_TRACE1(value);
	access_CoreWriteByte(value, (baseAddr + PHY_I2CM_ADDRESS));
}

void halI2cMasterPhy_WriteData(u16 baseAddr, u16 value)
{
	LOG_TRACE1(value);
	access_CoreWriteByte((u8) (value >> 8), (baseAddr + PHY_I2CM_DATAO + 0));
	access_CoreWriteByte((u8) (value >> 0), (baseAddr + PHY_I2CM_DATAO + 1));
}

void halI2cMasterPhy_WriteRequest(u16 baseAddr)
{
	LOG_TRACE();
	access_CoreWrite(1, (baseAddr + PHY_I2CM_OPERATION), 4, 1);
}

void FhalI2cMasterPhy_FastMode(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + PHY_I2CM_DIV), 0, 1);
}

void halI2cMasterPhy_Reset(u16 baseAddr)
{
	LOG_TRACE();
	access_CoreWrite(0, (baseAddr + PHY_I2CM_SOFTRSTZ), 0, 1);
}

void halI2cMasterPhy_ReadRequest(u16 baseAddr)
{
	access_CoreWrite(1, (baseAddr + PHY_I2CM_OPERATION), 0, 1);
}

u16 halI2cMasterPhy_ReadData(u16 baseAddr)
{
	u16 value;
	value = 0;
	value |= access_CoreReadByte(baseAddr + PHY_I2CM_DATAI + 0)  << 8;
	value |= access_CoreReadByte(baseAddr + PHY_I2CM_DATAI + 1);
	return value;
}
