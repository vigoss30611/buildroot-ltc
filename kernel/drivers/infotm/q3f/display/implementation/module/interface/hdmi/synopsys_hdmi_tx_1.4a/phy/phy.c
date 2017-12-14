/*
 * phy.c
 *
 *  Synopsys Inc.
 *  SG DWC PT02
 */
#include <linux/delay.h>
#include "phy.h"
#include "halMainController.h"
#include "halSourcePhy.h"
#include "halI2cMasterPhy.h"
#include "system.h"
#include "log.h"
#include "error.h"
#include "access.h"
#include "halI2cMasterPhy.h"

extern int hdmi_direct;

static const u16 PHY_BASE_ADDR = 0x3000;
static const u16 MC_BASE_ADDR = 0x4000;
static const u16 PHY_I2CM_BASE_ADDR = 0x3020;
// William. Critical.
//static const u8 PHY_I2C_SLAVE_ADDR = 0x54;
static const u8 PHY_I2C_SLAVE_ADDR = 0x69;

void phy_WaitforTxReady(u16 baseAddr);

int phy_Initialize(u16 baseAddr, u8 dataEnablePolarity)
{
	LOG_TRACE1(dataEnablePolarity);
#ifndef PHY_THIRD_PARTY
#ifdef PHY_GEN2_TSMC_40LP_2_5V
	halSourcePhy_Gen2TxPowerOn(baseAddr + PHY_BASE_ADDR, 0);
	halSourcePhy_Gen2PDDQ(baseAddr + PHY_BASE_ADDR, 1);
	LOG_NOTICE("GEN 2 TSMC 40LP 2.5V build - TQL");
#endif
#ifdef PHY_GEN2_TSMC_40G_1_8V
	LOG_NOTICE("GEN 2 TSMC 40G 1.8V build - E102");
	halSourcePhy_Gen2TxPowerOn(baseAddr + PHY_BASE_ADDR, 0);
	halSourcePhy_Gen2PDDQ(baseAddr + PHY_BASE_ADDR, 1);
#endif
#ifdef PHY_GEN2_TSMC_65LP_2_5V
	LOG_NOTICE("GEN 2 TSMC 65LP 2.5V build - E104");
	halSourcePhy_Gen2TxPowerOn(baseAddr + PHY_BASE_ADDR, 0);
	halSourcePhy_Gen2PDDQ(baseAddr + PHY_BASE_ADDR, 1);
#endif
#ifdef PHY_TNP
	LOG_NOTICE("TNP build");
#endif
#ifdef PHY_CNP
	LOG_NOTICE("CNP build");
#endif
	// William. Critical.
	//halSourcePhy_InterruptMask(baseAddr + PHY_BASE_ADDR, ~0); /* mask phy interrupts */

	halSourcePhy_DataEnablePolarity(baseAddr + PHY_BASE_ADDR,
			dataEnablePolarity);
	halSourcePhy_InterfaceControl(baseAddr + PHY_BASE_ADDR, 0);
	halSourcePhy_EnableTMDS(baseAddr + PHY_BASE_ADDR, 0);
	halSourcePhy_PowerDown(baseAddr + PHY_BASE_ADDR, 0); /* disable PHY */

	// William. Critical.
	access_Write(0x08, 0x3027);
	access_Write(0x88, 0x3028);
#else
	LOG_NOTICE("Third Party PHY build");
#endif
	return TRUE;
}

int phy_Configure (u16 baseAddr, u16 pClk, u8 cRes, u8 pRep)
{
#ifndef PHY_THIRD_PARTY
#ifdef PHY_CNP
	u16 clk = 0;
	u16 rep = 0;
#endif
	u16 i = 0;
	LOG_TRACE();
	/*  colour resolution 0 is 8 bit colour depth */
	if (cRes == 0)
		cRes = 8;

	if (pRep != 0)
	{
		error_Set(ERR_PIXEL_REPETITION_NOT_SUPPORTED);
		LOG_ERROR2("pixel repetition not supported", pRep);
		return FALSE;
	}

	/* The following is only for PHY_GEN1_CNP, and 1v0 NOT 1v1 */
	/* values are found in document HDMISRC1UHTCcnp_IPCS_DS_0v3.doc
	 * for the HDMISRCGPHIOcnp
	 */
	/* in the cnp PHY interface, the 3 most significant bits are ctrl (which
	 * part block to write) and the last 5 bits are data */
	/* for example 0x6A4a is writing to block  3 (ie. [14:10]) (5-bit blocks)
	 * the bits 0x0A, and  block 2 (ie. [9:5]) the bits 0x0A */
	/* configure PLL after core pixel repetition */
#ifdef PHY_GEN2_TSMC_40LP_2_5V
	halSourcePhy_Gen2TxPowerOn(baseAddr + PHY_BASE_ADDR, 0);
	halSourcePhy_Gen2PDDQ(baseAddr + PHY_BASE_ADDR, 1);
	halMainController_PhyReset(baseAddr + MC_BASE_ADDR, 0);
	halMainController_PhyReset(baseAddr + MC_BASE_ADDR, 1);
	halMainController_HeacPhyReset(baseAddr + MC_BASE_ADDR, 1);
	halSourcePhy_TestClear(baseAddr + PHY_BASE_ADDR, 1);
	halI2cMasterPhy_SlaveAddress(baseAddr + PHY_I2CM_BASE_ADDR, PHY_I2C_SLAVE_ADDR);
	halSourcePhy_TestClear(baseAddr + PHY_BASE_ADDR, 0);

	phy_I2cWrite(baseAddr, 0x0000, 0x13); /* PLLPHBYCTRL */
	phy_I2cWrite(baseAddr, 0x0006, 0x17);
	/* RESISTANCE TERM 133Ohm Cfg  */
	phy_I2cWrite(baseAddr, 0x0005, 0x19); /* TXTERM */
	/* REMOVE CLK TERM */
	phy_I2cWrite(baseAddr, 0x8000, 0x05); /* CKCALCTRL */
	switch (pClk)
	{
		case 2520:
			switch (cRes)
			{
				case 8:
					/* PLL/MPLL Cfg */
					phy_I2cWrite(baseAddr, 0x01e0, 0x06);
					phy_I2cWrite(baseAddr, 0x091c, 0x10); /* CURRCTRL */
					phy_I2cWrite(baseAddr, 0x0000, 0x15); /* GMPCTRL */
					break;
				case 10:
					phy_I2cWrite(baseAddr, 0x21e1, 0x06);
					phy_I2cWrite(baseAddr, 0x091c, 0x10);
					phy_I2cWrite(baseAddr, 0x0000, 0x15);
					break;
				case 12:
					phy_I2cWrite(baseAddr, 0x41e2, 0x06);
					phy_I2cWrite(baseAddr, 0x06dc, 0x10);
					phy_I2cWrite(baseAddr, 0x0000, 0x15);
					break;
				default:
					error_Set(ERR_COLOR_DEPTH_NOT_SUPPORTED);
					LOG_ERROR2("color depth not supported", cRes);
					return FALSE;
			}
			/* PREEMP Cgf 0.00 */
			phy_I2cWrite(baseAddr, 0x8009, 0x09); /* CKSYMTXCTRL */
			/* TX/CK LVL 10 */
			phy_I2cWrite(baseAddr, 0x0210, 0x0E); /* VLEVCTRL */
			break;
		case 2700:
			switch (cRes)
			{
				case 8:
					phy_I2cWrite(baseAddr, 0x01e0, 0x06);
					phy_I2cWrite(baseAddr, 0x091c, 0x10);
					phy_I2cWrite(baseAddr, 0x0000, 0x15);
					break;
				case 10:
					phy_I2cWrite(baseAddr, 0x21e1, 0x06);
					phy_I2cWrite(baseAddr, 0x091c, 0x10);
					phy_I2cWrite(baseAddr, 0x0000, 0x15);
					break;
				case 12:
					phy_I2cWrite(baseAddr, 0x41e2, 0x06);
					phy_I2cWrite(baseAddr, 0x06dc, 0x10);
					phy_I2cWrite(baseAddr, 0x0000, 0x15);
					break;
				default:
					error_Set(ERR_COLOR_DEPTH_NOT_SUPPORTED);
					LOG_ERROR2("color depth not supported", cRes);
					return FALSE;
			}
			phy_I2cWrite(baseAddr, 0x8009, 0x09);
			phy_I2cWrite(baseAddr, 0x0210, 0x0E);
			break;
		case 5400:
			switch (cRes)
			{
				case 8:
					phy_I2cWrite(baseAddr, 0x0140, 0x06);
					phy_I2cWrite(baseAddr, 0x091c, 0x10);
					phy_I2cWrite(baseAddr, 0x0005, 0x15);
					break;
				case 10:
					phy_I2cWrite(baseAddr, 0x2141, 0x06);
					phy_I2cWrite(baseAddr, 0x091c, 0x10);
					phy_I2cWrite(baseAddr, 0x0005, 0x15);
					break;
				case 12:
					phy_I2cWrite(baseAddr, 0x4142, 0x06);
					phy_I2cWrite(baseAddr, 0x06dc, 0x10);
					phy_I2cWrite(baseAddr, 0x0005, 0x15);
					break;
				default:
					error_Set(ERR_COLOR_DEPTH_NOT_SUPPORTED);
					LOG_ERROR2("color depth not supported", cRes);
					return FALSE;
			}
			phy_I2cWrite(baseAddr, 0x8009, 0x09);
			phy_I2cWrite(baseAddr, 0x0210, 0x0E);
			break;
		case 7200:
			switch (cRes)
			{
				case 8:
					phy_I2cWrite(baseAddr, 0x0140, 0x06);
					phy_I2cWrite(baseAddr, 0x06dc, 0x10);
					phy_I2cWrite(baseAddr, 0x0005, 0x15);
					break;
				case 10:
					phy_I2cWrite(baseAddr, 0x2141, 0x06);
					phy_I2cWrite(baseAddr, 0x06dc, 0x10);
					phy_I2cWrite(baseAddr, 0x0005, 0x15);
					break;
				case 12:
					phy_I2cWrite(baseAddr, 0x40a2, 0x06);
					phy_I2cWrite(baseAddr, 0x091c, 0x10);
					phy_I2cWrite(baseAddr, 0x000a, 0x15);
					break;
				default:
					error_Set(ERR_COLOR_DEPTH_NOT_SUPPORTED);
					LOG_ERROR2("color depth not supported", cRes);
					return FALSE;
			}
			phy_I2cWrite(baseAddr, 0x8009, 0x09);
			phy_I2cWrite(baseAddr, 0x0210, 0x0E);
			break;
		case 7425:
			switch (cRes)
			{
				case 8:
					phy_I2cWrite(baseAddr, 0x0140, 0x06);
					phy_I2cWrite(baseAddr, 0x06dc, 0x10);
					phy_I2cWrite(baseAddr, 0x0005, 0x15);
					break;
				case 10:
					phy_I2cWrite(baseAddr, 0x20a1, 0x06);
					phy_I2cWrite(baseAddr, 0x0b5c, 0x10);
					phy_I2cWrite(baseAddr, 0x000a, 0x15);
					break;
				case 12:
					phy_I2cWrite(baseAddr, 0x40a2, 0x06);
					phy_I2cWrite(baseAddr, 0x091c, 0x10);
					phy_I2cWrite(baseAddr, 0x000a, 0x15);
					break;
				default:
					error_Set(ERR_COLOR_DEPTH_NOT_SUPPORTED);
					LOG_ERROR2("color depth not supported", cRes);
					return FALSE;
			}
			phy_I2cWrite(baseAddr, 0x8009, 0x09);
			phy_I2cWrite(baseAddr, 0x0210, 0x0E);
			break;
		case 10800:
			switch (cRes)
			{
				case 8:
					phy_I2cWrite(baseAddr, 0x00a0, 0x06);
					phy_I2cWrite(baseAddr, 0x091c, 0x10);
					phy_I2cWrite(baseAddr, 0x000a, 0x15);
					break;
				case 10:
					phy_I2cWrite(baseAddr, 0x20a1, 0x06);
					phy_I2cWrite(baseAddr, 0x091c, 0x10);
					phy_I2cWrite(baseAddr, 0x000a, 0x15);
					break;
				case 12:
					phy_I2cWrite(baseAddr, 0x40a2, 0x06);
					phy_I2cWrite(baseAddr, 0x06dc, 0x10);
					phy_I2cWrite(baseAddr, 0x000a, 0x15);
					break;
				default:
					error_Set(ERR_COLOR_DEPTH_NOT_SUPPORTED);
					LOG_ERROR2("color depth not supported", cRes);
					return FALSE;
			}
			phy_I2cWrite(baseAddr, 0x8009, 0x09);
			phy_I2cWrite(baseAddr, 0x0210, 0x0E);
			break;
		case 14850:
			switch (cRes)
			{
				case 8:
					phy_I2cWrite(baseAddr, 0x00a0, 0x06);
					phy_I2cWrite(baseAddr, 0x06dc, 0x10);
					phy_I2cWrite(baseAddr, 0x000a, 0x15);
					phy_I2cWrite(baseAddr, 0x8009, 0x09);
					phy_I2cWrite(baseAddr, 0x0210, 0x0E);
					break;
				case 10:
					phy_I2cWrite(baseAddr, 0x2001, 0x06);
					phy_I2cWrite(baseAddr, 0x0b5c, 0x10);
					phy_I2cWrite(baseAddr, 0x000f, 0x15);
					phy_I2cWrite(baseAddr, 0x8009, 0x09);
					phy_I2cWrite(baseAddr, 0x0210, 0x0E);
					break;
				case 12:
					phy_I2cWrite(baseAddr, 0x4002, 0x06);
					phy_I2cWrite(baseAddr, 0x091c, 0x10);
					phy_I2cWrite(baseAddr, 0x000f, 0x15);
					phy_I2cWrite(baseAddr, 0x800b, 0x09);
					phy_I2cWrite(baseAddr, 0x0129, 0x0E);
					break;
				default:
					error_Set(ERR_COLOR_DEPTH_NOT_SUPPORTED);
					LOG_ERROR2("color depth not supported", cRes);
					return FALSE;
			}
			break;
		default:
			error_Set(ERR_PIXEL_CLOCK_NOT_SUPPORTED);
			LOG_ERROR2("pixel clock not supported", pClk);
			return FALSE;
	}
	//phy_I2cWrite(baseAddr, 0x8009, 0x09);		// William. Ref Sam. No effect.
	//phy_I2cWrite(baseAddr, 0x01AB, 0x0E);		// William. Ref Sam. No effect.
	
	halSourcePhy_Gen2EnHpdRxSense(baseAddr + PHY_BASE_ADDR, 1);
	halSourcePhy_Gen2TxPowerOn(baseAddr + PHY_BASE_ADDR, 1);
	halSourcePhy_Gen2PDDQ(baseAddr + PHY_BASE_ADDR, 0);
#else
#ifdef PHY_GEN2_TSMC_40G_1_8V
	halSourcePhy_Gen2TxPowerOn(baseAddr + PHY_BASE_ADDR, 0);
	halSourcePhy_Gen2PDDQ(baseAddr + PHY_BASE_ADDR, 1);
	halMainController_PhyReset(baseAddr + MC_BASE_ADDR, 0);
	halMainController_PhyReset(baseAddr + MC_BASE_ADDR, 1);
	halMainController_HeacPhyReset(baseAddr + MC_BASE_ADDR, 1);
	halSourcePhy_TestClear(baseAddr + PHY_BASE_ADDR, 1);
	halI2cMasterPhy_SlaveAddress(baseAddr + PHY_I2CM_BASE_ADDR, PHY_I2C_SLAVE_ADDR);
	halSourcePhy_TestClear(baseAddr + PHY_BASE_ADDR, 0);

	phy_I2cWrite(baseAddr, 0x0000, 0x13);
	phy_I2cWrite(baseAddr, 0x0002, 0x19);
	phy_I2cWrite(baseAddr, 0x0006, 0x17);
	phy_I2cWrite(baseAddr, 0x8000, 0x05);
	switch (pClk)
	{
		case 2520:
			switch (cRes)
			{
				case 8:			
					phy_I2cWrite(baseAddr, 0x01e0, 0x06);
					phy_I2cWrite(baseAddr, 0x08da, 0x10);
					phy_I2cWrite(baseAddr, 0x0000, 0x15);
					break;
				case 10:
					phy_I2cWrite(baseAddr, 0x21e1, 0x06);
					phy_I2cWrite(baseAddr, 0x08da, 0x10);
					phy_I2cWrite(baseAddr, 0x0000, 0x15);
					break;
				case 12:
					phy_I2cWrite(baseAddr, 0x41e2, 0x06);
					phy_I2cWrite(baseAddr, 0x065a, 0x10);
					phy_I2cWrite(baseAddr, 0x0000, 0x15);
					break;
				default:
					error_Set(ERR_COLOR_DEPTH_NOT_SUPPORTED);
					LOG_ERROR2("color depth not supported", cRes);
					return FALSE;
			}
			phy_I2cWrite(baseAddr, 0x8009, 0x09);
			phy_I2cWrite(baseAddr, 0x0231, 0x0E);
			break;
		case 2700:
			switch (cRes)
			{
				case 8:
					phy_I2cWrite(baseAddr, 0x01e0, 0x06);
					phy_I2cWrite(baseAddr, 0x08da, 0x10);
					phy_I2cWrite(baseAddr, 0x0000, 0x15);
					break;
				case 10:
					phy_I2cWrite(baseAddr, 0x21e1, 0x06);
					phy_I2cWrite(baseAddr, 0x08da, 0x10);
					phy_I2cWrite(baseAddr, 0x0000, 0x15);
					break;
				case 12:
					phy_I2cWrite(baseAddr, 0x41e2, 0x06);
					phy_I2cWrite(baseAddr, 0x065a, 0x10);
					phy_I2cWrite(baseAddr, 0x0000, 0x15);
					break;
				default:
					error_Set(ERR_COLOR_DEPTH_NOT_SUPPORTED);
					LOG_ERROR2("color depth not supported", cRes);
					return FALSE;
			}
			phy_I2cWrite(baseAddr, 0x8009, 0x09);
			phy_I2cWrite(baseAddr, 0x0231, 0x0E);
			break;
		case 5400:
			switch (cRes)
			{
				case 8:
					phy_I2cWrite(baseAddr, 0x0140, 0x06);
					phy_I2cWrite(baseAddr, 0x09da, 0x10);
					phy_I2cWrite(baseAddr, 0x0005, 0x15);
					break;
				case 10:
					phy_I2cWrite(baseAddr, 0x2141, 0x06);
					phy_I2cWrite(baseAddr, 0x09da, 0x10);
					phy_I2cWrite(baseAddr, 0x0005, 0x15);
					break;
				case 12:
					phy_I2cWrite(baseAddr, 0x4142, 0x06);
					phy_I2cWrite(baseAddr, 0x079a, 0x10);
					phy_I2cWrite(baseAddr, 0x0005, 0x15);
					break;
				default:
					error_Set(ERR_COLOR_DEPTH_NOT_SUPPORTED);
					LOG_ERROR2("color depth not supported", cRes);
					return FALSE;
			}
			phy_I2cWrite(baseAddr, 0x8009, 0x09);
			phy_I2cWrite(baseAddr, 0x0231, 0x0E);
			break;
		case 7200:
			switch (cRes)
			{
				case 8:
					phy_I2cWrite(baseAddr, 0x0140, 0x06);
					phy_I2cWrite(baseAddr, 0x079a, 0x10);
					phy_I2cWrite(baseAddr, 0x0005, 0x15);
					break;
				case 10:
					phy_I2cWrite(baseAddr, 0x2141, 0x06);
					phy_I2cWrite(baseAddr, 0x079a, 0x10);
					phy_I2cWrite(baseAddr, 0x0005, 0x15);
					break;
				case 12:
					phy_I2cWrite(baseAddr, 0x40a2, 0x06);
					phy_I2cWrite(baseAddr, 0x0a5a, 0x10);
					phy_I2cWrite(baseAddr, 0x000a, 0x15);
					break;
				default:
					error_Set(ERR_COLOR_DEPTH_NOT_SUPPORTED);
					LOG_ERROR2("color depth not supported", cRes);
					return FALSE;
			}
			phy_I2cWrite(baseAddr, 0x8009, 0x09);
			phy_I2cWrite(baseAddr, 0x0231, 0x0E);
			break;
		case 7425:
			switch (cRes)
			{
				case 8:
					phy_I2cWrite(baseAddr, 0x0140, 0x06);
					phy_I2cWrite(baseAddr, 0x079a, 0x10);
					phy_I2cWrite(baseAddr, 0x0005, 0x15);
					break;
				case 10:
					phy_I2cWrite(baseAddr, 0x20a1, 0x06);
					phy_I2cWrite(baseAddr, 0x0bda, 0x10);
					phy_I2cWrite(baseAddr, 0x000a, 0x15);
					break;
				case 12:
					phy_I2cWrite(baseAddr, 0x40a2, 0x06);
					phy_I2cWrite(baseAddr, 0x0a5a, 0x10);
					phy_I2cWrite(baseAddr, 0x000a, 0x15);
					break;
				default:
					error_Set(ERR_COLOR_DEPTH_NOT_SUPPORTED);
					LOG_ERROR2("color depth not supported", cRes);
					return FALSE;
			}
			phy_I2cWrite(baseAddr, 0x8009, 0x09);
			phy_I2cWrite(baseAddr, 0x0231, 0x0E);
			break;
		case 10800:
			switch (cRes)
			{
				case 8:
					phy_I2cWrite(baseAddr, 0x00a0, 0x06);
					phy_I2cWrite(baseAddr, 0x091c, 0x10);
					phy_I2cWrite(baseAddr, 0x000a, 0x15);
					break;
				case 10:
					phy_I2cWrite(baseAddr, 0x20a1, 0x06);
					phy_I2cWrite(baseAddr, 0x091c, 0x10);
					phy_I2cWrite(baseAddr, 0x000a, 0x15);
					break;
				case 12:
					phy_I2cWrite(baseAddr, 0x40a2, 0x06);
					phy_I2cWrite(baseAddr, 0x06dc, 0x10);
					phy_I2cWrite(baseAddr, 0x000a, 0x15);
					break;
				default:
					error_Set(ERR_COLOR_DEPTH_NOT_SUPPORTED);
					LOG_ERROR2("color depth not supported", cRes);
					return FALSE;
			}
			phy_I2cWrite(baseAddr, 0x8009, 0x09);
			phy_I2cWrite(baseAddr, 0x0231, 0x0E);
			break;
		case 14850:
			switch (cRes)
			{
				case 8:
					phy_I2cWrite(baseAddr, 0x00a0, 0x06);
					phy_I2cWrite(baseAddr, 0x079a, 0x10);
					phy_I2cWrite(baseAddr, 0x000a, 0x15);
					phy_I2cWrite(baseAddr, 0x8009, 0x09);
					phy_I2cWrite(baseAddr, 0x0231, 0x0E);
					break;
				case 10:
					phy_I2cWrite(baseAddr, 0x2001, 0x06);
					phy_I2cWrite(baseAddr, 0x0bda, 0x10);
					phy_I2cWrite(baseAddr, 0x000f, 0x15);
					phy_I2cWrite(baseAddr, 0x800b, 0x09);
					phy_I2cWrite(baseAddr, 0x014a, 0x0E);
					break;
				case 12:
					phy_I2cWrite(baseAddr, 0x4002, 0x06);
					phy_I2cWrite(baseAddr, 0x0a5a, 0x10);
					phy_I2cWrite(baseAddr, 0x000f, 0x15);
					phy_I2cWrite(baseAddr, 0x800b, 0x09);
					phy_I2cWrite(baseAddr, 0x014a, 0x0E);
					break;
				case 16:
					phy_I2cWrite(baseAddr, 0x6003, 0x06);
					phy_I2cWrite(baseAddr, 0x07da, 0x10);
					phy_I2cWrite(baseAddr, 0x000f, 0x15);
					phy_I2cWrite(baseAddr, 0x800b, 0x09);
					phy_I2cWrite(baseAddr, 0x014a, 0x0E);
					break;
				default:
					error_Set(ERR_COLOR_DEPTH_NOT_SUPPORTED);
					LOG_ERROR2("color depth not supported", cRes);
					return FALSE;
			}
			break;
		case 34000:
			switch (cRes)
			{
				case 8:
					phy_I2cWrite(baseAddr, 0x0000, 0x06);
					phy_I2cWrite(baseAddr, 0x07da, 0x10);
					phy_I2cWrite(baseAddr, 0x000f, 0x15);
					phy_I2cWrite(baseAddr, 0x800b, 0x09);
					phy_I2cWrite(baseAddr, 0x0108, 0x0E);
					break;
				default:
					error_Set(ERR_COLOR_DEPTH_NOT_SUPPORTED);
					LOG_ERROR2("color depth not supported", cRes);
					return FALSE;
			}
			break;
		default:
			error_Set(ERR_PIXEL_CLOCK_NOT_SUPPORTED);
			LOG_ERROR2("pixel clock not supported", pClk);
			return FALSE;
	}
	halSourcePhy_Gen2EnHpdRxSense(baseAddr + PHY_BASE_ADDR, 1);
	halSourcePhy_Gen2TxPowerOn(baseAddr + PHY_BASE_ADDR, 1);
	halSourcePhy_Gen2PDDQ(baseAddr + PHY_BASE_ADDR, 0);
#else
#ifdef PHY_GEN2_TSMC_65LP_2_5V
	halSourcePhy_Gen2TxPowerOn(baseAddr + PHY_BASE_ADDR, 0);
	halSourcePhy_Gen2PDDQ(baseAddr + PHY_BASE_ADDR, 1);
	halMainController_PhyReset(baseAddr + MC_BASE_ADDR, 0);
	halMainController_PhyReset(baseAddr + MC_BASE_ADDR, 1);
	halMainController_HeacPhyReset(baseAddr + MC_BASE_ADDR, 1);
	halSourcePhy_TestClear(baseAddr + PHY_BASE_ADDR, 1);
	halI2cMasterPhy_SlaveAddress(baseAddr + PHY_I2CM_BASE_ADDR, PHY_I2C_SLAVE_ADDR);
	halSourcePhy_TestClear(baseAddr + PHY_BASE_ADDR, 0);
	phy_I2cWrite(baseAddr, 0x8009, 0x09);
	phy_I2cWrite(baseAddr, 0x0004, 0x19);
	phy_I2cWrite(baseAddr, 0x0000, 0x13);
	phy_I2cWrite(baseAddr, 0x0006, 0x17);
	phy_I2cWrite(baseAddr, 0x8000, 0x05);
	switch (pClk)
		{
			case 2520:
				switch (cRes)
				{
					case 8:
						phy_I2cWrite(baseAddr, 0x01E0, 0x06);
						phy_I2cWrite(baseAddr, 0x08D9, 0x10);
						phy_I2cWrite(baseAddr, 0x0000, 0x15);
						break;
					case 10:
						/* break; */
					case 12:
						/* break; */
					default:
						error_Set(ERR_COLOR_DEPTH_NOT_SUPPORTED);
						LOG_ERROR2("color depth not supported", cRes);
						return FALSE;
				}
				phy_I2cWrite(baseAddr, 0x0231, 0x0E);
				break;
			case 2700:
				switch (cRes)
				{
					case 8:
						phy_I2cWrite(baseAddr, 0x01E0, 0x06);
						phy_I2cWrite(baseAddr, 0x08D9, 0x10);
						phy_I2cWrite(baseAddr, 0x0000, 0x15);
						break;
					case 10:
						/* break; */
					case 12:
						/* break; */
					default:
						error_Set(ERR_COLOR_DEPTH_NOT_SUPPORTED);
						LOG_ERROR2("color depth not supported", cRes);
						return FALSE;
				}
				phy_I2cWrite(baseAddr, 0x0231, 0x0E);
				break;
			case 5400:
				switch (cRes)
				{
					case 8:
						/* break; */
					case 10:
						/* break; */
					case 12:
						/* break; */
					default:
						error_Set(ERR_COLOR_DEPTH_NOT_SUPPORTED);
						LOG_ERROR2("color depth not supported", cRes);
						return FALSE;
				}
				phy_I2cWrite(baseAddr, 0x0231, 0x0E);
				break;
			case 7200:
				switch (cRes)
				{
					case 8:
						/* break; */
					case 10:
						/* break; */
					case 12:
						/* break; */
					default:
						error_Set(ERR_COLOR_DEPTH_NOT_SUPPORTED);
						LOG_ERROR2("color depth not supported", cRes);
						return FALSE;
				}
				break;
				phy_I2cWrite(baseAddr, 0x0231, 0x0E);
			case 7425:
				switch (cRes)
				{
					case 8:
						phy_I2cWrite(baseAddr, 0x0140, 0x06);
						phy_I2cWrite(baseAddr, 0x06D9, 0x10);
						phy_I2cWrite(baseAddr, 0x0005, 0x15);
						break;
					case 10:
						/* break; */
					case 12:
						/* break; */
					default:
						error_Set(ERR_COLOR_DEPTH_NOT_SUPPORTED);
						LOG_ERROR2("color depth not supported", cRes);
						return FALSE;
				}
				phy_I2cWrite(baseAddr, 0x0231, 0x0E);
				break;
			case 10800:
				switch (cRes)
				{
					case 8:
						phy_I2cWrite(baseAddr, 0x0231, 0x0E);
						/* break; */
					case 10:
						phy_I2cWrite(baseAddr, 0x0231, 0x0E);
						/* break; */
					case 12:
						phy_I2cWrite(baseAddr, 0x0108, 0x0E);
						/* break; */
					default:
						error_Set(ERR_COLOR_DEPTH_NOT_SUPPORTED);
						LOG_ERROR2("color depth not supported", cRes);
						return FALSE;
				}
				break;
			case 14850:
				switch (cRes)
				{
					case 8:
						phy_I2cWrite(baseAddr, 0x00A0, 0x06);
						phy_I2cWrite(baseAddr, 0x06D9, 0x10);
						phy_I2cWrite(baseAddr, 0x000A, 0x15);
						phy_I2cWrite(baseAddr, 0x0231, 0x0E);
						break;
					case 10:
						phy_I2cWrite(baseAddr, 0x2001, 0x06);
						phy_I2cWrite(baseAddr, 0x0BD9, 0x10);
						phy_I2cWrite(baseAddr, 0x000F, 0x15);
						phy_I2cWrite(baseAddr, 0x014A, 0x0E);
						break;
					case 12:
						phy_I2cWrite(baseAddr, 0x4002, 0x06);
						phy_I2cWrite(baseAddr, 0x09D9, 0x10);
						phy_I2cWrite(baseAddr, 0x000F, 0x15);
						phy_I2cWrite(baseAddr, 0x014A, 0x0E);
						break;
					case 16:
						phy_I2cWrite(baseAddr, 0x6003, 0x06);
						phy_I2cWrite(baseAddr, 0x0719, 0x10);
						phy_I2cWrite(baseAddr, 0x000F, 0x15);
						phy_I2cWrite(baseAddr, 0x014A, 0x0E);
						break;
					default:
						error_Set(ERR_COLOR_DEPTH_NOT_SUPPORTED);
						LOG_ERROR2("color depth not supported", cRes);
						return FALSE;
				}
				break;
			case 34000:
				switch (cRes)
				{
					case 8:
						phy_I2cWrite(baseAddr, 0x0000, 0x06);
						phy_I2cWrite(baseAddr, 0x0719, 0x10);
						phy_I2cWrite(baseAddr, 0x000F, 0x15);
						phy_I2cWrite(baseAddr, 0x0129, 0x0E);
						break;
					default:
						error_Set(ERR_COLOR_DEPTH_NOT_SUPPORTED);
						LOG_ERROR2("color depth not supported", cRes);
						return FALSE;
				}
		}

	halSourcePhy_Gen2EnHpdRxSense(baseAddr + PHY_BASE_ADDR, 1);
	halSourcePhy_Gen2TxPowerOn(baseAddr + PHY_BASE_ADDR, 1);
	halSourcePhy_Gen2PDDQ(baseAddr + PHY_BASE_ADDR, 0);
#else
	if (cRes != 8 && cRes != 12)
	{
		error_Set(ERR_COLOR_DEPTH_NOT_SUPPORTED);
		LOG_ERROR2("color depth not supported", cRes);
		return FALSE;
	}
	halSourcePhy_TestClear(baseAddr + PHY_BASE_ADDR, 0);
	halSourcePhy_TestClear(baseAddr + PHY_BASE_ADDR, 1);
	halSourcePhy_TestClear(baseAddr + PHY_BASE_ADDR, 0);
#ifndef PHY_TNP
	switch (pClk)
	{
		case 2520:
			clk = 0x93C1;
			rep = (cRes == 8) ? 0x6A4A : 0x6653;
			break;
		case 2700:
			clk = 0x96C1;
			rep = (cRes == 8) ? 0x6A4A : 0x6653;
			break;
		case 5400:
			clk = 0x8CC3;
			rep = (cRes == 8) ? 0x6A4A : 0x6653;
			break;
		case 7200:
			clk = 0x90C4;
			rep = (cRes == 8) ? 0x6A4A : 0x6654;
			break;
		case 7425:
			clk = 0x95C8;
			rep = (cRes == 8) ? 0x6A4A : 0x6654;
			break;
		case 10800:
			clk = 0x98C6;
			rep = (cRes == 8) ? 0x6A4A : 0x6653;
			break;
		case 14850:
			clk = 0x89C9;
			rep = (cRes == 8) ? 0x6A4A : 0x6654;
			break;
		default:
			error_Set(ERR_PIXEL_CLOCK_NOT_SUPPORTED);
			LOG_ERROR2("pixel clock not supported", pClk);
			return FALSE;
	}
	halSourcePhy_TestClock(baseAddr + PHY_BASE_ADDR, 0);
	halSourcePhy_TestEnable(baseAddr + PHY_BASE_ADDR, 0);
	if (phy_TestControl(baseAddr, 0x1B) != TRUE)
	{
		error_Set(ERR_PHY_TEST_CONTROL);
		return FALSE;
	}
	phy_TestData(baseAddr, (u8)(clk >> 8));
	phy_TestData(baseAddr, (u8)(clk >> 0));
	if (phy_TestControl(baseAddr, 0x1A) != TRUE)
	{
		error_Set(ERR_PHY_TEST_CONTROL);
		return FALSE;
	}
	phy_TestData(baseAddr, (u8)(rep >> 8));
	phy_TestData(baseAddr, (u8)(rep >> 0));
#endif
	if (pClk == 14850 && cRes == 12)
	{
		LOG_NOTICE("Applying Pre-Emphasis");
		if (phy_TestControl(baseAddr, 0x24) != TRUE)
		{
			error_Set(ERR_PHY_TEST_CONTROL);
			return FALSE;
		}
		phy_TestData(baseAddr, 0x80);
		phy_TestData(baseAddr, 0x90);
		phy_TestData(baseAddr, 0xa0);
#ifndef PHY_TNP
		phy_TestData(baseAddr, 0xb0);
		if (phy_TestControl(baseAddr, 0x20) != TRUE)
		{ /*  +11.1ma 3.3 pe */
			error_Set(ERR_PHY_TEST_CONTROL);
			return FALSE;
		}
		phy_TestData(baseAddr, 0x04);
		if (phy_TestControl(baseAddr, 0x21) != TRUE) /*  +11.1 +2ma 3.3 pe */
		{
			error_Set(ERR_PHY_TEST_CONTROL);
			return FALSE;
		}
		phy_TestData(baseAddr, 0x2a);

		if (phy_TestControl(baseAddr, 0x11) != TRUE)
		{
			error_Set(ERR_PHY_TEST_CONTROL);
			return FALSE;
		}
		phy_TestData(baseAddr, 0xf3);
		phy_TestData(baseAddr, 0x93);
#else
		if (phy_TestControl(baseAddr, 0x20) != TRUE)
		{
			error_Set(ERR_PHY_TEST_CONTROL);
			return FALSE;
		}
		phy_TestData(baseAddr, 0x00);
		if (phy_TestControl(baseAddr, 0x21) != TRUE)
		{
			error_Set(ERR_PHY_TEST_CONTROL);
			return FALSE;
		}
		phy_TestData(baseAddr, 0x00);
#endif
	}
	if (phy_TestControl(baseAddr, 0x00) != TRUE)
	{
		error_Set(ERR_PHY_TEST_CONTROL);
		return FALSE;
	}
	halSourcePhy_TestDataIn(baseAddr + PHY_BASE_ADDR, 0x00);
	halMainController_PhyReset(baseAddr + MC_BASE_ADDR, 1); /*  reset PHY */
	halMainController_PhyReset(baseAddr + MC_BASE_ADDR, 0);
	halSourcePhy_PowerDown(baseAddr + PHY_BASE_ADDR, 1); /* enable PHY */
	halSourcePhy_EnableTMDS(baseAddr + PHY_BASE_ADDR, 0); /*  toggle TMDS */
	halSourcePhy_EnableTMDS(baseAddr + PHY_BASE_ADDR, 1);
#endif
#endif
#endif
	/* wait PHY_TIMEOUT no of cycles at most for the pll lock signal to raise ~around 20us max */
	for (i = 0; i < PHY_TIMEOUT; i++)
	{
		//if ((i % 100) == 0)
		//{
			if (halSourcePhy_PhaseLockLoopState(baseAddr + PHY_BASE_ADDR) == TRUE)
			{
				break;
			}
			msleep(2);
		//}
	}
	if (halSourcePhy_PhaseLockLoopState(baseAddr + PHY_BASE_ADDR) != TRUE)
	{
		error_Set(ERR_PHY_NOT_LOCKED);
		LOG_ERROR("PHY PLL not locked");
		return FALSE;
	}
#endif

	phy_WaitforTxReady(baseAddr);	// William
	
	return TRUE;
}

int phy_Standby(u16 baseAddr)
{
#ifndef PHY_THIRD_PARTY
	halSourcePhy_InterruptMask(baseAddr + PHY_BASE_ADDR, ~0); /* mask phy interrupts */
	halSourcePhy_EnableTMDS(baseAddr + PHY_BASE_ADDR, 0);
	halSourcePhy_PowerDown(baseAddr + PHY_BASE_ADDR, 0); /*  disable PHY */
	halSourcePhy_Gen2TxPowerOn(baseAddr, 0);
	halSourcePhy_Gen2PDDQ(baseAddr + PHY_BASE_ADDR, 1);
#endif
	return TRUE;
}

int phy_EnableHpdSense(u16 baseAddr)
{
#ifndef PHY_THIRD_PARTY
	halSourcePhy_Gen2EnHpdRxSense(baseAddr + PHY_BASE_ADDR, 1);
#endif
	return TRUE;
}

int phy_DisableHpdSense(u16 baseAddr)
{
#ifndef PHY_THIRD_PARTY
	halSourcePhy_Gen2EnHpdRxSense(baseAddr + PHY_BASE_ADDR, 0);
#endif
	return TRUE;
}

int phy_HotPlugDetected(u16 baseAddr)
{
	/* MASK		STATUS		POLARITY	INTERRUPT		HPD
	 *   0			0			0			1			0
	 *   0			1			0			0			1
	 *   0			0			1			0			0
	 *   0			1			1			1			1
	 *   1			x			x			0			x
	 */
	int polarity = 0;	
	polarity = halSourcePhy_InterruptPolarityStatus(baseAddr + PHY_BASE_ADDR, 0x02) >> 1;
	if (polarity == halSourcePhy_HotPlugState(baseAddr + PHY_BASE_ADDR))
	{
		halSourcePhy_InterruptPolarity(baseAddr + PHY_BASE_ADDR, 1, !polarity);
		return polarity;
	}
	return !polarity;
	/* return halSourcePhy_HotPlugState(baseAddr + PHY_BASE_ADDR); */
}

int phy_InterruptEnable(u16 baseAddr, u8 value)
{
	LOG_TRACE1(value);
	halSourcePhy_InterruptMask(baseAddr + PHY_BASE_ADDR, value);
	return TRUE;
}
#ifndef PHY_THIRD_PARTY
int phy_TestControl(u16 baseAddr, u8 value)
{
	LOG_TRACE1(value);
	halSourcePhy_TestDataIn(baseAddr + PHY_BASE_ADDR, value);
	halSourcePhy_TestEnable(baseAddr + PHY_BASE_ADDR, 1);
	halSourcePhy_TestClock(baseAddr + PHY_BASE_ADDR, 1);
	halSourcePhy_TestClock(baseAddr + PHY_BASE_ADDR, 0);
	halSourcePhy_TestEnable(baseAddr + PHY_BASE_ADDR, 0);
	return TRUE;
}

int phy_TestData(u16 baseAddr, u8 value)
{
	LOG_TRACE1(value);
	halSourcePhy_TestDataIn(baseAddr + PHY_BASE_ADDR, value);
	halSourcePhy_TestEnable(baseAddr + PHY_BASE_ADDR, 0);
	halSourcePhy_TestClock(baseAddr + PHY_BASE_ADDR, 1);
	halSourcePhy_TestClock(baseAddr + PHY_BASE_ADDR, 0);
	return TRUE;
}

int phy_I2CDone(void)
{
	unsigned int i;
	u8 cIdle;
	LOG_TRACE();
	udelay(1000);
	for (i = 0; i < 20; i++) {
		cIdle = access_Read(0x0108);
		if (cIdle & 0x02) {
			access_Write(0x02, 0x0108);
			break;
		}
		msleep(1);
	}
	if (i == 20)
		LOG_ERROR("Time out while waiting for I2C Master PHY Done Interrupt.\n");
	return 0;
}


int phy_I2cWrite(u16 baseAddr, u16 data, u8 addr)
{
	LOG_TRACE2(data, addr);
	halI2cMasterPhy_RegisterAddress(baseAddr + PHY_I2CM_BASE_ADDR, addr);
	halI2cMasterPhy_WriteData(baseAddr + PHY_I2CM_BASE_ADDR, data);
	halI2cMasterPhy_WriteRequest(baseAddr + PHY_I2CM_BASE_ADDR);
	phy_I2CDone();
	return TRUE;
}

u16 phy_I2cRead(u16 baseAddr, u8 addr)
{
	u16 value;
	LOG_TRACE1(addr);
	halI2cMasterPhy_RegisterAddress(baseAddr + PHY_I2CM_BASE_ADDR, addr);
	halI2cMasterPhy_ReadRequest(baseAddr + PHY_I2CM_BASE_ADDR);
	phy_I2CDone();
	value = halI2cMasterPhy_ReadData(baseAddr + PHY_I2CM_BASE_ADDR);
	return value;
}

void phy_WaitforTxReady(u16 baseAddr)
{
	unsigned int i;
	u8 regaddr;
	u16 value;
	LOG_TRACE();
	regaddr = 0x1A;
	for (i = 0; i < 100; i++) {
		value = phy_I2cRead(baseAddr, regaddr);
		if(value & 0x04)
			break;
	}
	if (i == 100)
		LOG_ERROR("Time out while waiting for I2C Master PHY TxReady.\n");
}

#endif
