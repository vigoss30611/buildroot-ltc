/*
 * @file:halInterrupt.c
 *
 *  Created on: Jul 1, 2010
 *      Author: klabadi & dlopo
 */
#include "halInterrupt.h"
#include "access.h"
#include "log.h"

/* register offsets */
static const u8 IH_FC_STAT0 = 0x00;
static const u8 IH_FC_STAT1 = 0x01;
static const u8 IH_FC_STAT2 = 0x02;
static const u8 IH_AS_STAT0 = 0x03;
static const u8 IH_PHY_STAT0 = 0x04;
static const u8 IH_I2CM_STAT0 = 0x05;
static const u8 IH_CEC_STAT0 = 0x06;
static const u8 IH_VP_STAT0 = 0x07;
static const u8 IH_I2CMPHY_STAT0 = 0x08;
static const u8 IH_MUTE = 0xFF;

u8 halInterrupt_AudioPacketsState(u16 baseAddr)
{
	LOG_TRACE();
	return access_CoreReadByte(baseAddr + IH_FC_STAT0);
}

void halInterrupt_AudioPacketsClear(u16 baseAddr, u8 value)
{
	LOG_TRACE1(value);
	access_CoreWriteByte(value, (baseAddr + IH_FC_STAT0));
}

u8 halInterrupt_OtherPacketsState(u16 baseAddr)
{
	LOG_TRACE();
	return access_CoreReadByte(baseAddr + IH_FC_STAT1);
}

void halInterrupt_OtherPacketsClear(u16 baseAddr, u8 value)
{
	LOG_TRACE1(value);
	access_CoreWriteByte(value, (baseAddr + IH_FC_STAT1));
}

u8 halInterrupt_PacketsOverflowState(u16 baseAddr)
{
	LOG_TRACE();
	return access_CoreReadByte(baseAddr + IH_FC_STAT2);
}

void halInterrupt_PacketsOverflowClear(u16 baseAddr, u8 value)
{
	LOG_TRACE1(value);
	access_CoreWriteByte(value, (baseAddr + IH_FC_STAT2));
}

u8 halInterrupt_AudioSamplerState(u16 baseAddr)
{
	LOG_TRACE();
	return access_CoreReadByte(baseAddr + IH_AS_STAT0);
}

void halInterrupt_AudioSamplerClear(u16 baseAddr, u8 value)
{
	LOG_TRACE1(value);
	access_CoreWriteByte(value, (baseAddr + IH_AS_STAT0));
}

u8 halInterrupt_PhyState(u16 baseAddr)
{
	LOG_TRACE();
	return access_CoreReadByte(baseAddr + IH_PHY_STAT0);
}

void halInterrupt_PhyClear(u16 baseAddr, u8 value)
{
	LOG_TRACE1(value);
	access_CoreWriteByte(value, (baseAddr + IH_PHY_STAT0));
}

u8 halInterrupt_I2cDdcState(u16 baseAddr)

{
	LOG_TRACE();
	return access_CoreReadByte(baseAddr + IH_I2CM_STAT0);
}
void halInterrupt_I2cDdcClear(u16 baseAddr, u8 value)
{
	LOG_TRACE1(value);
	access_CoreWriteByte(value, (baseAddr + IH_I2CM_STAT0));
}

u8 halInterrupt_CecState(u16 baseAddr)
{
	LOG_TRACE();
	return access_CoreReadByte(baseAddr + IH_CEC_STAT0);
}

void halInterrupt_CecClear(u16 baseAddr, u8 value)
{
	LOG_TRACE1(value);
	access_CoreWriteByte(value, (baseAddr + IH_CEC_STAT0));
}

u8 halInterrupt_VideoPacketizerState(u16 baseAddr)
{
	LOG_TRACE();
	return access_CoreReadByte(baseAddr + IH_VP_STAT0);
}

void halInterrupt_VideoPacketizerClear(u16 baseAddr, u8 value)
{
	LOG_TRACE1(value);
	access_CoreWriteByte(value, (baseAddr + IH_VP_STAT0));
}

u8 halInterrupt_I2cPhyState(u16 baseAddr)
{
	LOG_TRACE();
	return access_CoreReadByte(baseAddr + IH_I2CMPHY_STAT0);
}

void halInterrupt_I2cPhyClear(u16 baseAddr, u8 value)
{
	LOG_TRACE1(value);
	access_CoreWriteByte(value, (baseAddr + IH_I2CMPHY_STAT0));
}

void halInterrupt_Mute(u16 baseAddr, u8 value)
{
	LOG_TRACE1(value);
	access_CoreWriteByte(value, (baseAddr + IH_MUTE));
}
