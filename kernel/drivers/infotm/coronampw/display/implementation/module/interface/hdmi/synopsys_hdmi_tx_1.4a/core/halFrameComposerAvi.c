/*
 * @file:halFrameComposerAvi.c
 *
 *  Created on: Jun 30, 2010
 *      Author: klabadi & dlopo
 */
#include "halFrameComposerAvi.h"
#include "access.h"
#include "log.h"

/* register offsets */
static const u8 FC_AVICONF0 = 0x19;
static const u8 FC_AVICONF1 = 0x1A;
static const u8 FC_AVICONF2 = 0x1B;
static const u8 FC_AVIVID = 0x1C;
static const u8 FC_AVIETB0 = 0x1D;
static const u8 FC_AVIETB1 = 0x1E;
static const u8 FC_AVISBB0 = 0x1F;
static const u8 FC_AVISBB1 = 0x20;
static const u8 FC_AVIELB0 = 0x21;
static const u8 FC_AVIELB1 = 0x22;
static const u8 FC_AVISRB0 = 0x23;
static const u8 FC_AVISRB1 = 0x24;
static const u8 FC_PRCONF = 0xE0;
/* bit shifts */
static const u8 RGBYCC = 0;
static const u8 SCAN_INFO = 4;
static const u8 COLORIMETRY = 6;
static const u8 PIC_ASPECT_RATIO = 4;
static const u8 ACTIVE_FORMAT_ASPECT_RATIO = 0;
static const u8 ACTIVE_FORMAT_AR_VALID = 6;
static const u8 IT_CONTENT = 7;
static const u8 EXT_COLORIMETRY = 4;
static const u8 QUANTIZATION_RANGE = 2;
static const u8 NON_UNIFORM_PIC_SCALING = 0;
static const u8 H_BAR_INFO = 3;
static const u8 V_BAR_INFO = 2;
static const u8 PIXEL_REP_OUT = 0;

void halFrameComposerAvi_RgbYcc(u16 baseAddr, u8 type)
{
	LOG_TRACE1(type);
	access_CoreWrite(type, baseAddr + FC_AVICONF0, RGBYCC, 2);
}

void halFrameComposerAvi_ScanInfo(u16 baseAddr, u8 left)
{
	LOG_TRACE1(left);
	access_CoreWrite(left, baseAddr + FC_AVICONF0, SCAN_INFO, 2);
}

void halFrameComposerAvi_Colorimetry(u16 baseAddr, unsigned cscITU)
{
	LOG_TRACE1(cscITU);
	access_CoreWrite(cscITU, baseAddr + FC_AVICONF1, COLORIMETRY, 2);
}

void halFrameComposerAvi_PicAspectRatio(u16 baseAddr, u8 ar)
{
	LOG_TRACE1(ar);
	access_CoreWrite(ar, baseAddr + FC_AVICONF1, PIC_ASPECT_RATIO, 2);
}

void halFrameComposerAvi_ActiveAspectRatioValid(u16 baseAddr, u8 valid)
{
	LOG_TRACE1(valid);
	access_CoreWrite(valid, baseAddr + FC_AVICONF0, ACTIVE_FORMAT_AR_VALID, 1); /* data valid flag */
}

void halFrameComposerAvi_ActiveFormatAspectRatio(u16 baseAddr, u8 left)
{
	LOG_TRACE1(left);
	access_CoreWrite(left, baseAddr + FC_AVICONF1, ACTIVE_FORMAT_ASPECT_RATIO,
			4);
}

void halFrameComposerAvi_IsItContent(u16 baseAddr, u8 it)
{
	LOG_TRACE1(it);
	access_CoreWrite((it ? 1 : 0), baseAddr + FC_AVICONF2, IT_CONTENT, 1);
}

void halFrameComposerAvi_ExtendedColorimetry(u16 baseAddr, u8 extColor)
{
	LOG_TRACE1(extColor);
	access_CoreWrite(extColor, baseAddr + FC_AVICONF2, EXT_COLORIMETRY, 3);
	access_CoreWrite(0x3, baseAddr + FC_AVICONF1, COLORIMETRY, 2); /*data valid flag */
}

void halFrameComposerAvi_QuantizationRange(u16 baseAddr, u8 range)
{
	LOG_TRACE1(range);
	access_CoreWrite(range, baseAddr + FC_AVICONF2, QUANTIZATION_RANGE, 2);
}

void halFrameComposerAvi_NonUniformPicScaling(u16 baseAddr, u8 scale)
{
	LOG_TRACE1(scale);
	access_CoreWrite(scale, baseAddr + FC_AVICONF2, NON_UNIFORM_PIC_SCALING, 2);
}

void halFrameComposerAvi_VideoCode(u16 baseAddr, u8 code)
{
	LOG_TRACE1(code);
	access_CoreWriteByte(code, baseAddr + FC_AVIVID);
}

void halFrameComposerAvi_HorizontalBarsValid(u16 baseAddr, u8 validity)
{
	access_CoreWrite((validity ? 1 : 0), baseAddr + FC_AVICONF0, H_BAR_INFO, 1); /*data valid flag */
}

void halFrameComposerAvi_HorizontalBars(u16 baseAddr, u16 endTop,
		u16 startBottom)
{
	LOG_TRACE2(endTop, startBottom);
	access_CoreWriteByte((u8) (endTop), baseAddr + FC_AVIETB0);
	access_CoreWriteByte((u8) (endTop >> 8), baseAddr + FC_AVIETB1);
	access_CoreWriteByte((u8) (startBottom), baseAddr + FC_AVISBB0);
	access_CoreWriteByte((u8) (startBottom >> 8), baseAddr + FC_AVISBB1);
}

void halFrameComposerAvi_VerticalBarsValid(u16 baseAddr, u8 validity)
{
	access_CoreWrite((validity ? 1 : 0), baseAddr + FC_AVICONF0, V_BAR_INFO, 1); /*data valid flag */
}

void halFrameComposerAvi_VerticalBars(u16 baseAddr, u16 endLeft, u16 startRight)
{
	LOG_TRACE2(endLeft, startRight);
	access_CoreWriteByte((u8) (endLeft), baseAddr + FC_AVIELB0);
	access_CoreWriteByte((u8) (endLeft >> 8), baseAddr + FC_AVIELB1);
	access_CoreWriteByte((u8) (startRight), baseAddr + FC_AVISRB0);
	access_CoreWriteByte((u8) (startRight >> 8), baseAddr + FC_AVISRB1);
}

void halFrameComposerAvi_OutPixelRepetition(u16 baseAddr, u8 pr)
{
	LOG_TRACE1(pr);
	access_CoreWrite(pr, baseAddr + FC_PRCONF, PIXEL_REP_OUT, 4);
}

