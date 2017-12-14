/*
 * @file:halEdid.c
 *
 *  Created on: Jul 5, 2010
 *      Author: klabadi & dlopo
 */

#include "halEdid.h"
#include "log.h"
#include "access.h"

static const u8 SLAVE_ADDR = 0x00;
static const u8 REQUEST_ADDR = 0x01;
static const u8 DATA_OUT = 0x02;
static const u8 DATA_IN = 0x03;
static const u8 OPERATION = 0x04;
static const u8 DONE_INT = 0x05;
static const u8 ERROR_INT = 0x06;
static const u8 CLOCK_DIV = 0x07;
static const u8 SEG_ADDR = 0x08;
static const u8 SEG_POINTER = 0x0A;
static const u8 SS_SCL_CNT = 0x0B;
static const u8 FS_SCL_CNT = 0x0F;

void halEdid_SlaveAddress(u16 baseAddr, u8 addr)
{
	LOG_TRACE1(addr);
	access_CoreWrite(addr, (baseAddr + SLAVE_ADDR), 0, 7);
}

void halEdid_RequestAddr(u16 baseAddr, u8 addr)
{
	LOG_TRACE1(addr);
	access_CoreWriteByte(addr, (baseAddr + REQUEST_ADDR));
}

void halEdid_WriteData(u16 baseAddr, u8 data)
{
	LOG_TRACE1(data);
	access_CoreWriteByte(data, (baseAddr + DATA_OUT));
}

u8 halEdid_ReadData(u16 baseAddr)
{
	LOG_TRACE();
	return access_CoreReadByte((baseAddr + DATA_IN));
}

void halEdid_RequestRead(u16 baseAddr)
{
	LOG_TRACE();
	access_CoreWrite(1, (baseAddr + OPERATION), 0, 1);
}

void halEdid_RequestExtRead(u16 baseAddr)
{
	LOG_TRACE();
	access_CoreWrite(1, (baseAddr + OPERATION), 1, 1);
}

void halEdid_RequestWrite(u16 baseAddr)
{
	LOG_TRACE();
	access_CoreWrite(1, (baseAddr + OPERATION), 4, 1);
}

void halEdid_MasterClockDivision(u16 baseAddr, u8 value)
{
	LOG_TRACE1(value);
	/* bit 4 selects between high and standard speed operation */
	access_CoreWrite(value, (baseAddr + CLOCK_DIV), 0, 4);
}

void halEdid_SegmentAddr(u16 baseAddr, u8 addr)
{
	LOG_TRACE1(addr);
	access_CoreWriteByte(addr, (baseAddr + SEG_ADDR));
}

void halEdid_SegmentPointer(u16 baseAddr, u8 pointer)
{
	LOG_TRACE1(pointer);
	access_CoreWriteByte(pointer, (baseAddr + SEG_POINTER));
}

void halEdid_MaskInterrupts(u16 baseAddr, u8 mask)
{
	LOG_TRACE1(mask);
	access_CoreWrite(mask ? 1 : 0, (baseAddr + DONE_INT), 2, 1);
	access_CoreWrite(mask ? 1 : 0, (baseAddr + ERROR_INT), 2, 1);
	access_CoreWrite(mask ? 1 : 0, (baseAddr + ERROR_INT), 6, 1);
}

void halEdid_FastSpeedCounter(u16 baseAddr, u32 value)
{
	LOG_TRACE2((baseAddr + FS_SCL_CNT), value);
	access_CoreWriteByte((u8) (value >> 24), (baseAddr + FS_SCL_CNT + 0));
	access_CoreWriteByte((u8) (value >> 16), (baseAddr + FS_SCL_CNT + 1));
	access_CoreWriteByte((u8) (value >> 8), (baseAddr + FS_SCL_CNT + 2));
	access_CoreWriteByte((u8) (value >> 0), (baseAddr + FS_SCL_CNT + 3));
}

void halEdid_StandardSpeedCounter(u16 baseAddr, u32 value)
{
	LOG_TRACE2((baseAddr + SS_SCL_CNT), value);
	access_CoreWriteByte((u8) (value >> 24), (baseAddr + SS_SCL_CNT + 0));
	access_CoreWriteByte((u8) (value >> 16), (baseAddr + SS_SCL_CNT + 1));
	access_CoreWriteByte((u8) (value >> 8), (baseAddr + SS_SCL_CNT + 2));
	access_CoreWriteByte((u8) (value >> 0), (baseAddr + SS_SCL_CNT + 3));
}
