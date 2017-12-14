/*
 * halHdcp.c
 *
 *  Created on: Jul 19, 2010
 *      Author: klabadi & dlopo
 */
#include "halHdcp.h"
#include "access.h"
#include "log.h"

const u8 A_HDCPCFG0 = 0x00;
const u8 A_HDCPCFG1 = 0x01;
const u8 A_HDCPOBS0 = 0x02;
const u8 A_HDCPOBS1 = 0x03;
const u8 A_HDCPOBS2 = 0x04;
const u8 A_HDCPOBS3 = 0x05;
const u8 A_APIINTCLR = 0x06;
const u8 A_APIINTSTA = 0x07;
const u8 A_APIINTMSK = 0x08;
const u8 A_VIDPOLCFG = 0x09;
const u8 A_OESSWCFG = 0x0A;
const u8 A_COREVERLSB = 0x14;
const u8 A_COREVERMSB = 0x15;
const u8 A_KSVMEMCTRL = 0x16;
const u8 A_MEMORY = 0x20;
const u16 REVOCATION_OFFSET = 0x299;

void halHdcp_DeviceMode(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + A_HDCPCFG0), 0, 1);
}

void halHdcp_EnableFeature11(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + A_HDCPCFG0), 1, 1);
}

void halHdcp_RxDetected(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + A_HDCPCFG0), 2, 1);
}

void halHdcp_EnableAvmute(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + A_HDCPCFG0), 3, 1);
}

void halHdcp_RiCheck(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + A_HDCPCFG0), 4, 1);
}

/* William Smith add */
int halHdcp_BypassEncryptionState(u16 baseAddr)
{
	LOG_TRACE();
	return access_CoreRead((baseAddr + A_HDCPCFG0), 5, 1);
}

void halHdcp_BypassEncryption(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + A_HDCPCFG0), 5, 1);
}

void halHdcp_EnableI2cFastMode(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + A_HDCPCFG0), 6, 1);
}

void halHdcp_EnhancedLinkVerification(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + A_HDCPCFG0), 7, 1);
}

void halHdcp_SwReset(u16 baseAddr)
{
	LOG_TRACE();
	/* active low */
	access_CoreWrite(0, (baseAddr + A_HDCPCFG1), 0, 1);
}

void halHdcp_DisableEncryption(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + A_HDCPCFG1), 1, 1);
}

void halHdcp_EncodingPacketHeader(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + A_HDCPCFG1), 2, 1);
}

void halHdcp_DisableKsvListCheck(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + A_HDCPCFG1), 3, 1);
}

u8 halHdcp_HdcpEngaged(u16 baseAddr)
{
	LOG_TRACE();
	return access_CoreRead((baseAddr + A_HDCPOBS0), 0, 1);
}

u8 halHdcp_AuthenticationState(u16 baseAddr)
{
	LOG_TRACE();
	return access_CoreRead((baseAddr + A_HDCPOBS0), 1, 7);
}

u8 halHdcp_CipherState(u16 baseAddr)
{
	LOG_TRACE();
	return access_CoreRead((baseAddr + A_HDCPOBS2), 3, 3);
}

u8 halHdcp_RevocationState(u16 baseAddr)
{
	LOG_TRACE();
	return access_CoreRead((baseAddr + A_HDCPOBS1), 0, 3);
}

u8 halHdcp_OessState(u16 baseAddr)
{
	LOG_TRACE();
	return access_CoreRead((baseAddr + A_HDCPOBS1), 3, 3);
}

u8 halHdcp_EessState(u16 baseAddr)
{
	LOG_TRACE();
	/* TODO width = 3 or 4? */
	return access_CoreRead((baseAddr + A_HDCPOBS2), 0, 3);
}

u8 halHdcp_DebugInfo(u16 baseAddr)
{
	LOG_TRACE();
	return access_CoreReadByte(baseAddr + A_HDCPOBS3);
}

void halHdcp_InterruptClear(u16 baseAddr, u8 value)
{
	LOG_TRACE1(value);
	access_CoreWriteByte(value, (baseAddr + A_APIINTCLR));
}

u8 halHdcp_InterruptStatus(u16 baseAddr)
{
	LOG_TRACE();
	return access_CoreReadByte(baseAddr + A_APIINTSTA);
}

void halHdcp_InterruptMask(u16 baseAddr, u8 value)
{
	LOG_TRACE1(value);
	access_CoreWriteByte(value, (baseAddr + A_APIINTMSK));
}

void halHdcp_HSyncPolarity(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + A_VIDPOLCFG), 1, 1);
}

void halHdcp_VSyncPolarity(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + A_VIDPOLCFG), 3, 1);
}

void halHdcp_DataEnablePolarity(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + A_VIDPOLCFG), 4, 1);
}

void halHdcp_UnencryptedVideoColor(u16 baseAddr, u8 value)
{
	LOG_TRACE1(value);
	access_CoreWrite(value, (baseAddr + A_VIDPOLCFG), 5, 2);
}

void halHdcp_OessWindowSize(u16 baseAddr, u8 value)
{
	LOG_TRACE1(value);
	access_CoreWriteByte(value, (baseAddr + A_OESSWCFG));
}

u16 halHdcp_CoreVersion(u16 baseAddr)
{
	u16 version = 0;
	LOG_TRACE();
	version = access_CoreReadByte(baseAddr + A_COREVERLSB);
	version |= access_CoreReadByte(baseAddr + A_COREVERMSB) << 8;
	return version;
}

void halHdcp_MemoryAccessRequest(u16 baseAddr, u8 bit)
{
	LOG_TRACE();
	access_CoreWrite(bit, (baseAddr + A_KSVMEMCTRL), 0, 1);
}

u8 halHdcp_MemoryAccessGranted(u16 baseAddr)
{
	LOG_TRACE();
	return access_CoreRead((baseAddr + A_KSVMEMCTRL), 1, 1);
}

void halHdcp_UpdateKsvListState(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + A_KSVMEMCTRL), 3, 1);
	access_CoreWrite(1, (baseAddr + A_KSVMEMCTRL), 2, 1);
	access_CoreWrite(0, (baseAddr + A_KSVMEMCTRL), 2, 1);
}

u8 halHdcp_KsvListRead(u16 baseAddr, u16 addr)
{
	LOG_TRACE1(addr);
	if (addr >= REVOCATION_OFFSET)
	{
		LOG_WARNING("Invalid address");
	}
	return access_CoreReadByte((baseAddr + A_MEMORY) + addr);
}

void halHdcp_RevocListWrite(u16 baseAddr, u16 addr, u8 data)
{
	LOG_TRACE2(addr, data);
	access_CoreWriteByte(data, (baseAddr + A_MEMORY) + REVOCATION_OFFSET + addr);
}
