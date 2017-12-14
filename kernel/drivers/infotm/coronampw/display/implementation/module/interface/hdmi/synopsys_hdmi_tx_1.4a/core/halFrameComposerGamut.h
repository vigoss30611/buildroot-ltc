/*
 * @file:halFrameComposerGamut.h
 *
 *  Created on: Jun 30, 2010
 *      Author: klabadi & dlopo
 */

#ifndef HALFRAMECOMPOSERGAMUT_H_
#define HALFRAMECOMPOSERGAMUT_H_

#include "types.h"

void halFrameComposerGamut_Profile(u16 baseAddr, u8 profile);

void halFrameComposerGamut_AffectedSeqNo(u16 baseAddr, u8 no);

void halFrameComposerGamut_PacketsPerFrame(u16 baseAddr, u8 packets);

void halFrameComposerGamut_PacketLineSpacing(u16 baseAddr, u8 lineSpacing);

void halFrameComposerGamut_Content(u16 baseAddr, const u8 * content, u8 length);

void halFrameComposerGamut_EnableTx(u16 baseAddr, u8 enable);

/* Only when this function is called is the packet updated with
 * the new information
 * @param baseAddr of the frame composer-gamut block
 * @return nothing
 */
void halFrameComposerGamut_UpdatePacket(u16 baseAddr);

u8 halFrameComposerGamut_CurrentSeqNo(u16 baseAddr);

u8 halFrameComposerGamut_PacketSeq(u16 baseAddr);

u8 halFrameComposerGamut_NoCurrentGbd(u16 baseAddr);

#endif /* HALFRAMECOMPOSERGAMUT_H_ */
