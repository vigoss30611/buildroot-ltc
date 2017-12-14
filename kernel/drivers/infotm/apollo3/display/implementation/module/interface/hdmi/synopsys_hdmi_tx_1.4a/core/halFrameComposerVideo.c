/*
 * @file:halFrameComposerVideo.c
 *
 *  Created on: Jul 1, 2010
 *      Author: klabadi & dlopo
 */
#include "halFrameComposerVideo.h"
#include "access.h"
#include "log.h"

/* register offsets */
static const u8 FC_INVIDCONF = 0x00;
static const u8 FC_INHACTV0 = 0x01;
static const u8 FC_INHACTV1 = 0x02;
static const u8 FC_INHBLANK0 = 0x03;
static const u8 FC_INHBLANK1 = 0x04;
static const u8 FC_INVACTV0 = 0x05;
static const u8 FC_INVACTV1 = 0x06;
static const u8 FC_INVBLANK = 0x07;
static const u8 FC_HSYNCINDELAY0 = 0x08;
static const u8 FC_HSYNCINDELAY1 = 0x09;
static const u8 FC_HSYNCINWIDTH0 = 0x0A;
static const u8 FC_HSYNCINWIDTH1 = 0x0B;
static const u8 FC_VSYNCINDELAY = 0x0C;
static const u8 FC_VSYNCINWIDTH = 0x0D;
static const u8 FC_INFREQ0 = 0x0E;
static const u8 FC_INFREQ1 = 0x0F;
static const u8 FC_INFREQ2 = 0x10;
static const u8 FC_CTRLDUR = 0x11;
static const u8 FC_EXCTRLDUR = 0x12;
static const u8 FC_EXCTRLSPAC = 0x13;
static const u8 FC_CH0PREAM = 0x14;
static const u8 FC_CH1PREAM = 0x15;
static const u8 FC_CH2PREAM = 0x16;
static const u8 FC_PRCONF = 0xE0;

void halFrameComposerVideo_HdcpKeepout(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + FC_INVIDCONF), 7, 1);
}

void halFrameComposerVideo_VSyncPolarity(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + FC_INVIDCONF), 6, 1);
}

void halFrameComposerVideo_HSyncPolarity(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + FC_INVIDCONF), 5, 1);
}

void halFrameComposerVideo_DataEnablePolarity(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + FC_INVIDCONF), 4, 1);
}

void halFrameComposerVideo_DviOrHdmi(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	/* 1: HDMI; 0: DVI */
	access_CoreWrite(bit, (baseAddr + FC_INVIDCONF), 3, 1);
}

void halFrameComposerVideo_VBlankOsc(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + FC_INVIDCONF), 1, 1);
}

void halFrameComposerVideo_Interlaced(u16 baseAddr, u8 bit)
{
	LOG_TRACE1(bit);
	access_CoreWrite(bit, (baseAddr + FC_INVIDCONF), 0, 1);
}

void halFrameComposerVideo_HActive(u16 baseAddr, u16 value)
{
	LOG_TRACE1(value);
	/* 12-bit width */
	access_CoreWriteByte((u8) (value), (baseAddr + FC_INHACTV0));
	access_CoreWrite((u8) (value >> 8), (baseAddr + FC_INHACTV1), 0, 4);
}

void halFrameComposerVideo_HBlank(u16 baseAddr, u16 value)
{
	LOG_TRACE1(value);
	/* 10-bit width */
	access_CoreWriteByte((u8) (value), (baseAddr + FC_INHBLANK0));
	access_CoreWrite((u8) (value >> 8), (baseAddr + FC_INHBLANK1), 0, 2);
}

void halFrameComposerVideo_VActive(u16 baseAddr, u16 value)
{
	LOG_TRACE1(value);
	/* 11-bit width */
	access_CoreWriteByte((u8) (value), (baseAddr + FC_INVACTV0));
	access_CoreWrite((u8) (value >> 8), (baseAddr + FC_INVACTV1), 0, 3);
}

void halFrameComposerVideo_VBlank(u16 baseAddr, u16 value)
{
	LOG_TRACE1(value);
	/* 8-bit width */
	access_CoreWriteByte((u8) (value), (baseAddr + FC_INVBLANK));
}

void halFrameComposerVideo_HSyncEdgeDelay(u16 baseAddr, u16 value)
{
	LOG_TRACE1(value);
	/* 11-bit width */
	access_CoreWriteByte((u8) (value), (baseAddr + FC_HSYNCINDELAY0));
	access_CoreWrite((u8) (value >> 8), (baseAddr + FC_HSYNCINDELAY1), 0, 3);
}

void halFrameComposerVideo_HSyncPulseWidth(u16 baseAddr, u16 value)
{
	LOG_TRACE1(value);
	/* 9-bit width */
	access_CoreWriteByte((u8) (value), (baseAddr + FC_HSYNCINWIDTH0));
	access_CoreWrite((u8) (value >> 8), (baseAddr + FC_HSYNCINWIDTH1), 0, 1);
}

void halFrameComposerVideo_VSyncEdgeDelay(u16 baseAddr, u16 value)
{
	LOG_TRACE1(value);
	/* 8-bit width */
	access_CoreWriteByte((u8) (value), (baseAddr + FC_VSYNCINDELAY));
}

void halFrameComposerVideo_VSyncPulseWidth(u16 baseAddr, u16 value)
{
	LOG_TRACE1(value);
	access_CoreWrite((u8) (value), (baseAddr + FC_VSYNCINWIDTH), 0, 6);
}

void halFrameComposerVideo_RefreshRate(u16 baseAddr, u32 value)
{
	LOG_TRACE1(value);
	/* 20-bit width */
	access_CoreWriteByte((u8) (value >> 0), (baseAddr + FC_INFREQ0));
	access_CoreWriteByte((u8) (value >> 8), (baseAddr + FC_INFREQ1));
	access_CoreWrite((u8) (value >> 16), (baseAddr + FC_INFREQ2), 0, 4);
}

void halFrameComposerVideo_ControlPeriodMinDuration(u16 baseAddr, u8 value)
{
	LOG_TRACE1(value);
	access_CoreWriteByte(value, (baseAddr + FC_CTRLDUR));
}

void halFrameComposerVideo_ExtendedControlPeriodMinDuration(u16 baseAddr,
		u8 value)
{
	LOG_TRACE1(value);
	access_CoreWriteByte(value, (baseAddr + FC_EXCTRLDUR));
}

void halFrameComposerVideo_ExtendedControlPeriodMaxSpacing(u16 baseAddr,
		u8 value)
{
	LOG_TRACE1(value);
	access_CoreWriteByte(value, (baseAddr + FC_EXCTRLSPAC));
}

void halFrameComposerVideo_PreambleFilter(u16 baseAddr, u8 value,
		unsigned channel)
{
	LOG_TRACE1(value);
	if (channel == 0)
		access_CoreWriteByte(value, (baseAddr + FC_CH0PREAM));
	else if (channel == 1)
		access_CoreWrite(value, (baseAddr + FC_CH1PREAM), 0, 6);
	else if (channel == 2)
		access_CoreWrite(value, (baseAddr + FC_CH2PREAM), 0, 6);
	else
		LOG_ERROR2("invalid channel number: ", channel);
}

void halFrameComposerVideo_PixelRepetitionInput(u16 baseAddr, u8 value)
{
	LOG_TRACE1(value);
	access_CoreWrite(value, (baseAddr + FC_PRCONF), 4, 4);
}
