/*
 * @file:halFrameComposerPackets.h
 *
 *  Created on: Jul 1, 2010
 *      Author: klabadi & dlopo
 */

#ifndef HALFRAMECOMPOSERPACKETS_H_
#define HALFRAMECOMPOSERPACKETS_H_

#include "types.h"

#define ACP_TX  	0
#define ISRC1_TX 	1
#define ISRC2_TX 	2
#define SPD_TX 		4
#define VSD_TX 		3

void halFrameComposerPackets_QueuePriorityHigh(u16 baseAddr, u8 value);

void halFrameComposerPackets_QueuePriorityLow(u16 baseAddr, u8 value);

void halFrameComposerPackets_MetadataFrameInterpolation(u16 baseAddr, u8 value);

void halFrameComposerPackets_MetadataFramesPerPacket(u16 baseAddr, u8 value);

void halFrameComposerPackets_MetadataLineSpacing(u16 baseAddr, u8 value);

void halFrameComposerPackets_AutoSend(u16 baseAddr, u8 enable, u8 mask);

void halFrameComposerPackets_ManualSend(u16 baseAddr, u8 mask);

void halFrameComposerPackets_DisableAll(u16 baseAddr);

#endif /* HALFRAMECOMPOSERPACKETS_H_ */
