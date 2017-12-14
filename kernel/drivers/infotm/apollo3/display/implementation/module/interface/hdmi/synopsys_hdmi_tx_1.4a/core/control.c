/*
 * @file:control.c
 *
 *  Synopsys Inc.
 *  SG DWC PT02
 */

#include "control.h"
#include "halMainController.h"
#include "halInterrupt.h"
#include "halIdentification.h"
#include "log.h"
#include "error.h"

/* block offsets */
const u16 ID_BASE_ADDR = 0x0000;
const u16 IH_BASE_ADDR = 0x0100;
static const u16 MC_BASE_ADDR = 0x4000;

int control_Initialize(u16 baseAddr, u8 dataEnablePolarity, u16 pixelClock)
{
	/*  clock gate == 1 => turn off modules */
	halMainController_PixelClockGate(baseAddr + MC_BASE_ADDR, 1);
	halMainController_TmdsClockGate(baseAddr + MC_BASE_ADDR, 1);
	halMainController_PixelRepetitionClockGate(baseAddr + MC_BASE_ADDR, 1);
	halMainController_ColorSpaceConverterClockGate(baseAddr + MC_BASE_ADDR, 1);
	halMainController_AudioSamplerClockGate(baseAddr + MC_BASE_ADDR, 1);
	halMainController_CecClockGate(baseAddr + MC_BASE_ADDR, 1);
	halMainController_HdcpClockGate(baseAddr + MC_BASE_ADDR, 1);
	return TRUE;
}

int control_Configure(u16 baseAddr, u16 pClk, u8 pRep, u8 cRes, int cscOn, int audioOn,
		int cecOn, int hdcpOn)
{
	halMainController_VideoFeedThroughOff(baseAddr + MC_BASE_ADDR,
			cscOn ? 1 : 0);	// cscOn ? 0 : 1);	William
	/*  clock gate == 0 => turn on modules */
	halMainController_PixelClockGate(baseAddr + MC_BASE_ADDR, 0);
	halMainController_TmdsClockGate(baseAddr + MC_BASE_ADDR, 0);
	halMainController_PixelRepetitionClockGate(baseAddr
			+ MC_BASE_ADDR, (pRep > 0) ? 0 : 1);
	halMainController_ColorSpaceConverterClockGate(baseAddr
			+ MC_BASE_ADDR, cscOn ? 0 : 1);
	halMainController_AudioSamplerClockGate(baseAddr + MC_BASE_ADDR,
			audioOn ? 0 : 1);
	halMainController_CecClockGate(baseAddr + MC_BASE_ADDR,
			cecOn ? 0 : 1);
	halMainController_HdcpClockGate(baseAddr + MC_BASE_ADDR,
			hdcpOn ? 0 : 1);
	return TRUE;
}

int control_Standby(u16 baseAddr)
{
	LOG_TRACE();
	/*  clock gate == 1 => turn off modules */
	halMainController_HdcpClockGate(baseAddr + MC_BASE_ADDR, 1);
	/*  CEC is not turned off because it has to answer PINGs even in standby mode */
	halMainController_AudioSamplerClockGate(baseAddr + MC_BASE_ADDR,
			1);
	halMainController_ColorSpaceConverterClockGate(baseAddr
			+ MC_BASE_ADDR, 1);
	halMainController_PixelRepetitionClockGate(baseAddr
			+ MC_BASE_ADDR, 1);
	halMainController_PixelClockGate(baseAddr + MC_BASE_ADDR, 1);
	halMainController_TmdsClockGate(baseAddr + MC_BASE_ADDR, 1);
	return TRUE;
}

u8 control_Design(u16 baseAddr)
{
	LOG_TRACE();
	return halIdentification_Design(baseAddr + ID_BASE_ADDR);
}

u8 control_Revision(u16 baseAddr)
{
	LOG_TRACE();
	return halIdentification_Revision(baseAddr + ID_BASE_ADDR);
}

u8 control_ProductLine(u16 baseAddr)
{
	LOG_TRACE();
	return halIdentification_ProductLine(baseAddr + ID_BASE_ADDR);
}

u8 control_ProductType(u16 baseAddr)
{
	LOG_TRACE();
	return halIdentification_ProductType(baseAddr + ID_BASE_ADDR);
}

int control_SupportsCore(u16 baseAddr)
{
	/*  Line 0xA0 - HDMICTRL */
	/*  Type 0x01 - TX */
	/*  Type 0xC1 - TX with HDCP */
	return control_ProductLine(baseAddr) == 0xA0 && (control_ProductType(baseAddr) == 0x01
			|| control_ProductType(baseAddr) == 0xC1);
}

int control_SupportsHdcp(u16 baseAddr)
{
	/*  Line 0xA0 - HDMICTRL */
	/*  Type 0xC1 - TX with HDCP */
	return control_ProductLine(baseAddr) == 0xA0 && control_ProductType(baseAddr) == 0xC1;
}

int control_InterruptMute(u16 baseAddr, u8 value)
{
	LOG_TRACE1(value);
	halInterrupt_Mute(baseAddr + IH_BASE_ADDR, value);
	return TRUE;
}
int control_InterruptCecClear(u16 baseAddr, u8 value)
{
	LOG_TRACE1(value);
	halInterrupt_CecClear(baseAddr + IH_BASE_ADDR, value);
	return TRUE;
}

u8 control_InterruptCecState(u16 baseAddr)
{
	LOG_TRACE();
	return halInterrupt_CecState(baseAddr + IH_BASE_ADDR);
}

int control_InterruptEdidClear(u16 baseAddr, u8 value)
{
	LOG_TRACE1(value);
	halInterrupt_I2cDdcClear(baseAddr + IH_BASE_ADDR, value);
	return TRUE;
}

u8 control_InterruptEdidState(u16 baseAddr)
{
	LOG_TRACE();
	return halInterrupt_I2cDdcState(baseAddr + IH_BASE_ADDR);
}

int control_InterruptPhyClear(u16 baseAddr, u8 value)
{
	LOG_TRACE1(value);
	halInterrupt_PhyClear(baseAddr + IH_BASE_ADDR, value);
	return TRUE;
}

u8 control_InterruptPhyState(u16 baseAddr)
{
#if 0
	static int i = 0;
	if ((i % 100) == 0)
	{
		LOG_NOTICE("---------");
		LOG_NOTICE2("audio packets   ", halInterrupt_AudioPacketsState(baseAddr + IH_BASE_ADDR));
		LOG_NOTICE2("other packets   ", halInterrupt_OtherPacketsState(baseAddr + IH_BASE_ADDR));
		LOG_NOTICE2("packets overflow", halInterrupt_PacketsOverflowState(baseAddr + IH_BASE_ADDR));
		LOG_NOTICE2("audio sampler   ", halInterrupt_AudioSamplerState(baseAddr + IH_BASE_ADDR));
		LOG_NOTICE2("phy state       ", halInterrupt_PhyState(baseAddr + IH_BASE_ADDR));
		LOG_NOTICE2("i2c ddc state   ", halInterrupt_I2cDdcState(baseAddr + IH_BASE_ADDR));
		LOG_NOTICE2("cec state       ", halInterrupt_CecState(baseAddr + IH_BASE_ADDR));
		LOG_NOTICE2("video packetizer", halInterrupt_VideoPacketizerState(baseAddr + IH_BASE_ADDR));
		LOG_NOTICE2("i2c phy state   ", halInterrupt_I2cPhyState(baseAddr + IH_BASE_ADDR));
	}
	i += 1;
#endif
	LOG_TRACE();
	return halInterrupt_PhyState(baseAddr + IH_BASE_ADDR);
}

int control_InterruptClearAll(u16 baseAddr)
{
	LOG_TRACE();
	halInterrupt_AudioPacketsClear(baseAddr + IH_BASE_ADDR,	(u8)(-1));
	halInterrupt_OtherPacketsClear(baseAddr + IH_BASE_ADDR,	(u8)(-1));
	halInterrupt_PacketsOverflowClear(baseAddr + IH_BASE_ADDR, (u8)(-1));
	halInterrupt_AudioSamplerClear(baseAddr + IH_BASE_ADDR,	(u8)(-1));
	halInterrupt_PhyClear(baseAddr + IH_BASE_ADDR, (u8)(-1));
	halInterrupt_I2cDdcClear(baseAddr + IH_BASE_ADDR, (u8)(-1));
	halInterrupt_CecClear(baseAddr + IH_BASE_ADDR, (u8)(-1));
	halInterrupt_VideoPacketizerClear(baseAddr + IH_BASE_ADDR, (u8)(-1));
	halInterrupt_I2cPhyClear(baseAddr + IH_BASE_ADDR, (u8)(-1));
	return TRUE;
}
