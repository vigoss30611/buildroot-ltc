/*
 * @file:halVideoGenerator.c
 *
 *  Created on: Jul 1, 2010
 *      Author: klabadi & dlopo
 */
#include "halVideoGenerator.h"
#include "access.h"
#include "log.h"

/* register offsets */
static const u8 VG_SWRSTZ = 0x00;
static const u8 VG_CONF = 0x01;
static const u8 VG_PREP = 0x02;
static const u8 VG_HACTIVE0 = 0x03;
static const u8 VG_HACTIVE1 = 0x04;
static const u8 VG_HBLANK0 = 0x05;
static const u8 VG_HBLANK1 = 0x06;
static const u8 VG_HDELAY0 = 0x07;
static const u8 VG_HDELAY1 = 0x08;
static const u8 VG_HWIDTH0 = 0x09;
static const u8 VG_HWIDTH1 = 0x0A;
static const u8 VG_VACTIVE0 = 0x0B;
static const u8 VG_VACTIVE1 = 0x0C;
static const u8 VG_VBLANK0 = 0x0D;
static const u8 VG_VDELAY0 = 0x0E;
static const u8 VG_VWIDTH0 = 0x0F;
static const u8 VG_VIDSOURCE = 0x10;
static const u8 VG_3DSTRUCT = 0x11;
static const u8 VG_RAM_ADDR0 = 0x20;
static const u8 VG_RAM_ADDR1 = 0x21;
static const u8 VG_WRT_RAM_CTRL = 0x22;
static const u8 VG_WRT_RAM_DATA = 0x23;
static const u8 VG_WRT_RAM_STOP_ADDR0 = 0x24;
static const u8 VG_WRT_RAM_STOP_ADDR1 = 0x25;
static const u8 VG_WRT_RAM_BYTE_STOP = 0x26;

void halVideoGenerator_SwReset(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	/* active low */
	access_CoreWrite(bit ? 0 : 1, (baseAddr + VG_SWRSTZ), 0, 1);
}

void halVideoGenerator_Ycc(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + VG_CONF), 7, 1);
}

void halVideoGenerator_Ycc422(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + VG_CONF), 6, 1);
}

void halVideoGenerator_VBlankOsc(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + VG_CONF), 5, 1);
}

void halVideoGenerator_ColorIncrement(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + VG_CONF), 4, 1);
}

void halVideoGenerator_Interlaced(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + VG_CONF), 3, 1);
}

void halVideoGenerator_VSyncPolarity(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + VG_CONF), 2, 1);
}

void halVideoGenerator_HSyncPolarity(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + VG_CONF), 1, 1);
}

void halVideoGenerator_DataEnablePolarity(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + VG_CONF), 0, 1);
}

void halVideoGenerator_ColorResolution(u16 baseAddr, u8 value)
{
	LOG_TRACE1(value);
	access_CoreWrite(value, (baseAddr + VG_PREP), 4, 2);
}

void halVideoGenerator_PixelRepetitionInput(u16 baseAddr, u8 value)
{
	LOG_TRACE1(value);
	access_CoreWrite(value, (baseAddr + VG_PREP), 0, 4);
}

void halVideoGenerator_HActive(u16 baseAddr, u16 value)
{
	LOG_TRACE1(value);
	/* 12-bit width */
	access_CoreWriteByte((u8) (value), (baseAddr + VG_HACTIVE0));
	access_CoreWrite((u8) (value >> 8), (baseAddr + VG_HACTIVE1), 0, 4);
}

void halVideoGenerator_HBlank(u16 baseAddr, u16 value)
{
	LOG_TRACE1(value);
	/* 10-bit width */
	access_CoreWriteByte((u8) (value), (baseAddr + VG_HBLANK0));
	access_CoreWrite((u8) (value >> 8), (baseAddr + VG_HBLANK1), 0, 2);
}

void halVideoGenerator_HSyncEdgeDelay(u16 baseAddr, u16 value)
{
	LOG_TRACE1(value);
	/* 11-bit width */
	access_CoreWriteByte((u8) (value), (baseAddr + VG_HDELAY0));
	access_CoreWrite(value >> 8, (baseAddr + VG_HDELAY1), 0, 3);
}

void halVideoGenerator_HSyncPulseWidth(u16 baseAddr, u16 value)
{
	LOG_TRACE1(value);
	/* 9-bit width */
	access_CoreWriteByte((u8) (value), (baseAddr + VG_HWIDTH0));
	access_CoreWrite((u8) (value >> 8), (baseAddr + VG_HWIDTH1), 0, 1);
}

void halVideoGenerator_VActive(u16 baseAddr, u16 value)
{
	LOG_TRACE1(value);
	/* 11-bit width */
	access_CoreWriteByte((u8) (value), (baseAddr + VG_VACTIVE0));
	access_CoreWrite((u8) (value >> 8), (baseAddr + VG_VACTIVE1), 0, 3);
}

void halVideoGenerator_VBlank(u16 baseAddr, u16 value)
{
	LOG_TRACE1(value);
	/* 8-bit width */
	access_CoreWriteByte((u8) (value), (baseAddr + VG_VBLANK0));
}

void halVideoGenerator_VSyncEdgeDelay(u16 baseAddr, u16 value)
{
	LOG_TRACE1(value);
	/* 8-bit width */
	access_CoreWriteByte((u8) (value), (baseAddr + VG_VDELAY0));
}

void halVideoGenerator_VSyncPulseWidth(u16 baseAddr, u16 value)
{
	LOG_TRACE1(value);
	/* 6-bit width */
	access_CoreWrite((u8) (value), (baseAddr + VG_VWIDTH0), 0, 6);
}

void halVideoGenerator_Enable3D(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + VG_3DSTRUCT), 4, 1);
}

void halVideoGenerator_Structure3D(u16 baseAddr, u8 value)
{
	LOG_TRACE1(value);
	access_CoreWrite(value, (baseAddr + VG_3DSTRUCT), 0, 4);
}

void halVideoGenerator_WriteStart(u16 baseAddr, u16 value)
{
	LOG_TRACE1(value);
	/* 13-bit width */
	access_CoreWriteByte((u8) (value), (baseAddr + VG_RAM_ADDR0));
	access_CoreWrite((u8) (value >> 8), (baseAddr + VG_RAM_ADDR1), 0, 5);
	/* start RAM write */
	access_CoreWrite(1, (baseAddr + VG_WRT_RAM_CTRL), 0, 1);
}

void halVideoGenerator_WriteData(u16 baseAddr, u8 value)
{
	LOG_TRACE1(value);
	access_CoreWriteByte((u8) (value), (baseAddr + VG_WRT_RAM_DATA));
}

u16 halVideoGenerator_WriteCurrentAddr(u16 baseAddr)
{
	u16 value = 0;
	LOG_TRACE();
	value = access_CoreRead((baseAddr + VG_WRT_RAM_STOP_ADDR1), 0, 5) << 8;
	return (value | access_CoreReadByte(baseAddr + VG_WRT_RAM_STOP_ADDR0));
}

u8 halVideoGenerator_WriteCurrentByte(u16 baseAddr)
{
	LOG_TRACE();
	return access_CoreRead((baseAddr + VG_WRT_RAM_BYTE_STOP), 0, 3);
}
