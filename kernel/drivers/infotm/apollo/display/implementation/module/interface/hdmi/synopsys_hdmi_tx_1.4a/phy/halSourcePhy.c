/*
 * @file:halSourcePhy.c
 *
 *  Created on: Jul 1, 2010
 *      Author: klabadi & dlopo
 */
#include "halSourcePhy.h"
#include "access.h"
#include "log.h"

/* register offsets */
static const u8 PHY_CONF0 = 0x00;
static const u8 PHY_TST0 = 0x01;
static const u8 PHY_TST1 = 0x02;
static const u8 PHY_TST2 = 0x03;
static const u8 PHY_STAT0 = 0x04;
static const u8 PHY_MASK0 = 0x06;
static const u8 PHY_POL0 = 0x07;

void halSourcePhy_PowerDown(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + PHY_CONF0), 7, 1);
}

void halSourcePhy_EnableTMDS(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + PHY_CONF0), 6, 1);
}

void halSourcePhy_Gen2PDDQ(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + PHY_CONF0), 4, 1);
}

void halSourcePhy_Gen2TxPowerOn(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + PHY_CONF0), 3, 1);
}

void halSourcePhy_Gen2EnHpdRxSense(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + PHY_CONF0), 2, 1);
}

void halSourcePhy_DataEnablePolarity(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + PHY_CONF0), 1, 1);
}

void halSourcePhy_InterfaceControl(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + PHY_CONF0), 0, 1);
}

void halSourcePhy_TestClear(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + PHY_TST0), 5, 1);
}

void halSourcePhy_TestEnable(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + PHY_TST0), 4, 1);
}

void halSourcePhy_TestClock(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + PHY_TST0), 0, 1);
}

void halSourcePhy_TestDataIn(u16 baseAddr, u8 data)
{
	LOG_TRACE1(data);
	access_CoreWriteByte(data, (baseAddr + PHY_TST1));
}

u8 halSourcePhy_TestDataOut(u16 baseAddr)
{
	LOG_TRACE();
	return access_CoreReadByte(baseAddr + PHY_TST2);
}

u8 halSourcePhy_PhaseLockLoopState(u16 baseAddr)
{
	LOG_TRACE();
	return access_CoreRead((baseAddr + PHY_STAT0), 0, 1);
}

u8 halSourcePhy_HotPlugState(u16 baseAddr)
{
	LOG_TRACE();
	return access_CoreRead((baseAddr + PHY_STAT0), 1, 1);
}

void halSourcePhy_InterruptMask(u16 baseAddr, u8 mask)
{
	LOG_TRACE1(mask);
	access_CoreWriteByte(mask, (baseAddr + PHY_MASK0));
}

u8 halSourcePhy_InterruptMaskStatus(u16 baseAddr, u8 mask)
{
	LOG_TRACE1(mask);
	return access_CoreReadByte(baseAddr + PHY_MASK0) & mask;
}

void halSourcePhy_InterruptPolarity(u16 baseAddr, u8 bitShift, u8 value)
{
	LOG_TRACE2(bitShift, value);
	access_CoreWrite(value, (baseAddr + PHY_POL0), bitShift, 1);
}

u8 halSourcePhy_InterruptPolarityStatus(u16 baseAddr, u8 mask)
{
	LOG_TRACE1(mask);
	return access_CoreReadByte(baseAddr + PHY_POL0) & mask;
}
