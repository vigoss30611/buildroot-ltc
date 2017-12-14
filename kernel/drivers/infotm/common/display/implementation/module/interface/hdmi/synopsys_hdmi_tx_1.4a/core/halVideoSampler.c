/*
 * @file:halVideoSampler.c
 *
 *  Created on: Jul 1, 2010
 *      Author: klabadi & dlopo
 */
#include "halVideoSampler.h"
#include "access.h"
#include "log.h"

/* register offsets */
static const u8 TX_INVID0 = 0x00;
static const u8 TX_INSTUFFING = 0x01;
static const u8 TX_GYDATA0 = 0x02;
static const u8 TX_GYDATA1 = 0x03;
static const u8 TX_RCRDATA0 = 0x04;
static const u8 TX_RCRDATA1 = 0x05;
static const u8 TX_BCBDATA0 = 0x06;
static const u8 TX_BCBDATA1 = 0x07;

void halVideoSampler_InternalDataEnableGenerator(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + TX_INVID0), 7, 1);
}

void halVideoSampler_VideoMapping(u16 baseAddr, u8 value)
{
	LOG_TRACE1(value);
	access_CoreWrite(value, (baseAddr + TX_INVID0), 0, 5);
}

void halVideoSampler_StuffingGy(u16 baseAddr, u16 value)
{
	LOG_TRACE1(value);
	access_CoreWriteByte((u8) (value >> 0), (baseAddr + TX_GYDATA0));
	access_CoreWriteByte((u8) (value >> 8), (baseAddr + TX_GYDATA1));
	access_CoreWrite(1, (baseAddr + TX_INSTUFFING), 0, 1);
}

void halVideoSampler_StuffingRcr(u16 baseAddr, u16 value)
{
	LOG_TRACE1(value);
	access_CoreWriteByte((u8) (value >> 0), (baseAddr + TX_RCRDATA0));
	access_CoreWriteByte((u8) (value >> 8), (baseAddr + TX_RCRDATA1));
	access_CoreWrite(1, (baseAddr + TX_INSTUFFING), 1, 1);
}

void halVideoSampler_StuffingBcb(u16 baseAddr, u16 value)
{
	LOG_TRACE1(value);
	access_CoreWriteByte((u8) (value >> 0), (baseAddr + TX_BCBDATA0));
	access_CoreWriteByte((u8) (value >> 8), (baseAddr + TX_BCBDATA1));
	access_CoreWrite(1, (baseAddr + TX_INSTUFFING), 2, 1);
}
