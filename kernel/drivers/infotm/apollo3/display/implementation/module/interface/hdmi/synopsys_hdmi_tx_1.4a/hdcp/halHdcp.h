/*
 * halHdcp.h
 *
 *  Created on: Jul 19, 2010
 *
 *  Synopsys Inc.
 *  SG DWC PT02 
 */
 /** @file
 *   HAL (hardware abstraction layer) of HDCP engine register block
 */

#ifndef HALHDCP_H_
#define HALHDCP_H_

#include "types.h"

/**
 * @param baseAddr Base address of the HDCP module registers
 * @param bit
 */
void halHdcp_DeviceMode(u16 baseAddr, u8 bit);

/**
 * @param baseAddr Base address of the HDCP module registers
 * @param bit
 */
void halHdcp_EnableFeature11(u16 baseAddr, u8 bit);

/**
 * @param baseAddr Base address of the HDCP module registers
 * @param bit
 */
void halHdcp_RxDetected(u16 baseAddr, u8 bit);

/**
 * @param baseAddr Base address of the HDCP module registers
 * @param bit
 */
void halHdcp_EnableAvmute(u16 baseAddr, u8 bit);

/**
 * @param baseAddr Base address of the HDCP module registers
 * @param bit
 */
void halHdcp_RiCheck(u16 baseAddr, u8 bit);

/**
 * @param baseAddr Base address of the HDCP module registers
 */
int halHdcp_BypassEncryptionState(u16 baseAddr);

/**
 * @param baseAddr Base address of the HDCP module registers
 * @param bit
 */
void halHdcp_BypassEncryption(u16 baseAddr, u8 bit);

/**
 * @param baseAddr Base address of the HDCP module registers
 * @param bit
 */
void halHdcp_EnableI2cFastMode(u16 baseAddr, u8 bit);

/**
 * @param baseAddr Base address of the HDCP module registers
 * @param bit
 */
void halHdcp_EnhancedLinkVerification(u16 baseAddr, u8 bit);

/**
 * @param baseAddr Base address of the HDCP module registers
 */
void halHdcp_SwReset(u16 baseAddr);

/**
 * @param baseAddr Base address of the HDCP module registers
 * @param bit
 */
void halHdcp_DisableEncryption(u16 baseAddr, u8 bit);

/**
 * @param baseAddr Base address of the HDCP module registers
 * @param bit
 */
void halHdcp_EncodingPacketHeader(u16 baseAddr, u8 bit);

/**
 * @param baseAddr Base address of the HDCP module registers
 * @param bit
 */
void halHdcp_DisableKsvListCheck(u16 baseAddr, u8 bit);

/**
 * @param baseAddr Base address of the HDCP module registers
 */
u8 halHdcp_HdcpEngaged(u16 baseAddr);

/**
 * @param baseAddr Base address of the HDCP module registers
 */
u8 halHdcp_AuthenticationState(u16 baseAddr);

/**
 * @param baseAddr Base address of the HDCP module registers
 */
u8 halHdcp_CipherState(u16 baseAddr);

/**
 * @param baseAddr Base address of the HDCP module registers
 */
u8 halHdcp_RevocationState(u16 baseAddr);

/**
 * @param baseAddr Base address of the HDCP module registers
 */
u8 halHdcp_OessState(u16 baseAddr);

/**
 * @param baseAddr Base address of the HDCP module registers
 */
u8 halHdcp_EessState(u16 baseAddr);

/**
 * @param baseAddr Base address of the HDCP module registers
 */
u8 halHdcp_DebugInfo(u16 baseAddr);

/**
 * @param baseAddr Base address of the HDCP module registers
 * @param value
 */
void halHdcp_InterruptClear(u16 baseAddr, u8 value);

/**
 * @param baseAddr Base address of the HDCP module registers
 */
u8 halHdcp_InterruptStatus(u16 baseAddr);

/**
 * @param baseAddr Base address of the HDCP module registers
 * @param value
 */
void halHdcp_InterruptMask(u16 baseAddr, u8 value);

/**
 * @param baseAddr Base address of the HDCP module registers
 * @param bit
 */
void halHdcp_HSyncPolarity(u16 baseAddr, u8 bit);

/**
 * @param baseAddr Base address of the HDCP module registers
 * @param bit
 */
void halHdcp_VSyncPolarity(u16 baseAddr, u8 bit);

/**
 * @param baseAddr Base address of the HDCP module registers
 * @param bit
 */
void halHdcp_DataEnablePolarity(u16 baseAddr, u8 bit);

/**
 * @param baseAddr Base address of the HDCP module registers
 * @param value
 */
void halHdcp_UnencryptedVideoColor(u16 baseAddr, u8 value);

/**
 * @param baseAddr Base address of the HDCP module registers
 * @param value
 */
void halHdcp_OessWindowSize(u16 baseAddr, u8 value);

/**
 * @param baseAddr Base address of the HDCP module registers
 */
u16 halHdcp_CoreVersion(u16 baseAddr);

/**
 * @param baseAddr Base address of the HDCP module registers
 * @param bit
 */
void halHdcp_MemoryAccessRequest(u16 baseAddr, u8 bit);

/**
 * @param baseAddr Base address of the HDCP module registers
 */
u8 halHdcp_MemoryAccessGranted(u16 baseAddr);

/**
 * @param baseAddr Base address of the HDCP module registers
 * @param bit
 */
void halHdcp_UpdateKsvListState(u16 baseAddr, u8 bit);

/**
 * @param baseAddr Base address of the HDCP module registers
 * @param addr in list
 */
u8 halHdcp_KsvListRead(u16 baseAddr, u16 addr);

/**
 * @param baseAddr Base address of the HDCP module registers
 * @param addr in list
 * @param data
 */
void halHdcp_RevocListWrite(u16 baseAddr, u16 addr, u8 data);

#endif /* HALHDCP_H_ */
