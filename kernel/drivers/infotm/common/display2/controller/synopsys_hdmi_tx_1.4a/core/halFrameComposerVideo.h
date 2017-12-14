/*
 * @file:halFrameComposerVideo.h
 *
 *  Created on: Jul 1, 2010
 *      Author: klabadi & dlopo
 */

#ifndef HALFRAMECOMPOSERVIDEO_H_
#define HALFRAMECOMPOSERVIDEO_H_

#include "types.h"

void halFrameComposerVideo_HdcpKeepout(u16 baseAddr, u8 bit);

void halFrameComposerVideo_VSyncPolarity(u16 baseAddr, u8 bit);

void halFrameComposerVideo_HSyncPolarity(u16 baseAddr, u8 bit);

void halFrameComposerVideo_DataEnablePolarity(u16 baseAddr, u8 bit);

void halFrameComposerVideo_DviOrHdmi(u16 baseAddr, u8 bit);

void halFrameComposerVideo_VBlankOsc(u16 baseAddr, u8 bit);

void halFrameComposerVideo_Interlaced(u16 baseAddr, u8 bit);

void halFrameComposerVideo_HActive(u16 baseAddr, u16 value);

void halFrameComposerVideo_HBlank(u16 baseAddr, u16 value);

void halFrameComposerVideo_VActive(u16 baseAddr, u16 value);

void halFrameComposerVideo_VBlank(u16 baseAddr, u16 value);

void halFrameComposerVideo_HSyncEdgeDelay(u16 baseAddr, u16 value);

void halFrameComposerVideo_HSyncPulseWidth(u16 baseAddr, u16 value);

void halFrameComposerVideo_VSyncEdgeDelay(u16 baseAddr, u16 value);

void halFrameComposerVideo_VSyncPulseWidth(u16 baseAddr, u16 value);

void halFrameComposerVideo_RefreshRate(u16 baseAddr, u32 value);

void halFrameComposerVideo_ControlPeriodMinDuration(u16 baseAddr, u8 value);

void halFrameComposerVideo_ExtendedControlPeriodMinDuration(u16 baseAddr,
		u8 value);

void halFrameComposerVideo_ExtendedControlPeriodMaxSpacing(u16 baseAddr,
		u8 value);

void halFrameComposerVideo_PreambleFilter(u16 baseAddr, u8 value,
		unsigned channel);

void halFrameComposerVideo_PixelRepetitionInput(u16 baseAddr, u8 value);

#endif /* HALFRAMECOMPOSERVIDEO_H_ */
