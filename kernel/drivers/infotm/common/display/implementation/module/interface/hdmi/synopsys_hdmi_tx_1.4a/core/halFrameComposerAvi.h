/*
 * @file:halFrameComposerAvi.h
 *
 *  Created on: Jun 30, 2010
 *      Author: klabadi & dlopo
 */

#ifndef HALFRAMECOMPOSERAVI_H_
#define HALFRAMECOMPOSERAVI_H_

#include "types.h"

void halFrameComposerAvi_RgbYcc(u16 baseAddr, u8 type);

void halFrameComposerAvi_ScanInfo(u16 baseAddr, u8 left);

void halFrameComposerAvi_Colorimetry(u16 baseAddr, unsigned cscITU);

void halFrameComposerAvi_PicAspectRatio(u16 baseAddr, u8 ar);

void halFrameComposerAvi_ActiveAspectRatioValid(u16 baseAddr, u8 valid);

void halFrameComposerAvi_ActiveFormatAspectRatio(u16 baseAddr, u8 left);

void halFrameComposerAvi_IsItContent(u16 baseAddr, u8 it);

void halFrameComposerAvi_ExtendedColorimetry(u16 baseAddr, u8 extColor);

void halFrameComposerAvi_QuantizationRange(u16 baseAddr, u8 range);

void halFrameComposerAvi_NonUniformPicScaling(u16 baseAddr, u8 scale);

void halFrameComposerAvi_VideoCode(u16 baseAddr, u8 code);

void halFrameComposerAvi_HorizontalBarsValid(u16 baseAddr, u8 validity);

void halFrameComposerAvi_HorizontalBars(u16 baseAddr, u16 endTop, u16 startBottom);

void halFrameComposerAvi_VerticalBarsValid(u16 baseAddr, u8 validity);

void halFrameComposerAvi_VerticalBars(u16 baseAddr, u16 endLeft, u16 startRight);

void halFrameComposerAvi_OutPixelRepetition(u16 baseAddr, u8 pr);

#endif /* HALFRAMECOMPOSERAVI_H_ */
