/*
 * @file:halMainController.c
 *
 *  Created on: Jul 1, 2010
 *      Author: klabadi & dlopo
 */
#include "halMainController.h"
#include "access.h"
#include "log.h"

/* register offsets */
static const u8 MC_SFRDIV = 0x00;
static const u8 MC_CLKDIS = 0x01;
static const u8 MC_SWRSTZREQ = 0x02;
static const u8 MC_FLOWCTRL = 0x04;
static const u8 MC_PHYRSTZ = 0x05;
static const u8 MC_LOCKONCLOCK = 0x06;
static const u8 MC_HEACPHY_RST = 0x07;

void halMainController_SfrClockDivision(u16 baseAddr, u8 value)
{
	LOG_TRACE1(value);
	access_CoreWrite(value, (baseAddr + MC_SFRDIV), 0, 4);
}

void halMainController_HdcpClockGate(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + MC_CLKDIS), 6, 1);
}

void halMainController_CecClockGate(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + MC_CLKDIS), 5, 1);
}

void halMainController_ColorSpaceConverterClockGate(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + MC_CLKDIS), 4, 1);
}

void halMainController_AudioSamplerClockGate(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + MC_CLKDIS), 3, 1);
}

void halMainController_PixelRepetitionClockGate(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + MC_CLKDIS), 2, 1);
}

void halMainController_TmdsClockGate(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + MC_CLKDIS), 1, 1);
}

void halMainController_PixelClockGate(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + MC_CLKDIS), 0, 1);
}

void halMainController_CecClockReset(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + MC_SWRSTZREQ), 6, 1);
}

void halMainController_AudioGpaReset(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + MC_SWRSTZREQ), 7, 1);
}

void halMainController_AudioHbrReset(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + MC_SWRSTZREQ), 5, 1);
}

void halMainController_AudioSpdifReset(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + MC_SWRSTZREQ), 4, 1);
}

void halMainController_AudioI2sReset(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + MC_SWRSTZREQ), 3, 1);
}

void halMainController_PixelRepetitionClockReset(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + MC_SWRSTZREQ), 2, 1);
}

void halMainController_TmdsClockReset(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + MC_SWRSTZREQ), 1, 1);
}

void halMainController_PixelClockReset(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + MC_SWRSTZREQ), 0, 1);
}

void halMainController_VideoFeedThroughOff(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + MC_FLOWCTRL), 0, 1);
}

void halMainController_PhyReset(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	/* active low */
	access_CoreWrite(bit ? 0 : 1, (baseAddr + MC_PHYRSTZ), 0, 1);
}

void halMainController_HeacPhyReset(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	/* active high */
	access_CoreWrite(bit ? 1 : 0, (baseAddr + MC_HEACPHY_RST), 0, 1);
}

u8 halMainController_LockOnClockStatus(u16 baseAddr, u8 clockDomain)
{
	LOG_TRACE1(clockDomain);
	return access_CoreRead((baseAddr + MC_LOCKONCLOCK), clockDomain, 1);
}

void halMainController_LockOnClockClear(u16 baseAddr, u8 clockDomain)
{
	LOG_TRACE1(clockDomain);
	access_CoreWrite(1, (baseAddr + MC_LOCKONCLOCK), clockDomain, 1);
}
