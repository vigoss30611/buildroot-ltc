/*
 * control.h
 *
 *  Created on: Jul 2, 2010
 *
 * Synopsys Inc.
 * SG DWC PT02
 */
/**
 * @file
 * Core control module:
 * Product information
 * Power management
 * Interrupt handling
 *
 */
#ifndef CONTROL_H_
#define CONTROL_H_

#include "types.h"
/**
 * Initializes PHY and core clocks
 * @param baseAddr base address of controller
 * @param dataEnablePolarity data enable polarity
 * @param pixelClock pixel clock [10KHz]
 * @return TRUE if successful
 */
int control_Initialize(u16 baseAddr, u8 dataEnablePolarity, u16 pixelClock);
/**
 * Configures PHY and core clocks
 * @param baseAddr base address of controller
 * @param pClk pixel clock [10KHz]
 * @param pRep pixel repetition factor
 * @param cRes color resolution
 * @param cscOn 1 if colour space converter active
 * @param audioOn 1 if Audio active
 * @param cecOn 1 if Cec module active
 * @param hdcpOn 1 if Hdcp active
 * @return TRUE if successful
 */
int control_Configure(u16 baseAddr, u16 pClk, u8 pRep, u8 cRes, int cscOn,
		int audioOn, int cecOn, int hdcpOn);
/**
 * Go into standby mode: stop all clocks from all modules except for the CEC (refer to CEC for more detail)
 * @param baseAddr base address of controller
 * @return TRUE if successful
 */
int control_Standby(u16 baseAddr);
/**
 * Read product design information
 * @param baseAddr base address of controller
 * @return the design number stored in the hardware
 */
u8 control_Design(u16 baseAddr);
/**
 * Read product revision information
 * @param baseAddr base address of controller
 * @return the revision number stored in the hardware
 */
u8 control_Revision(u16 baseAddr);
/**
 * Read product line information
 * @param baseAddr base address of controller
 * @return the product line stored in the hardware
 */
u8 control_ProductLine(u16 baseAddr);
/**
 * Read product type information
 * @param baseAddr base address of controller
 * @return the product type stored in the hardware
 */
u8 control_ProductType(u16 baseAddr);
/**
 * Check if hardware is compatible with the API
 * @param baseAddr base address of controller
 * @return TRUE if the HDMICTRL API supports the hardware (HDMI TX)
 */
int control_SupportsCore(u16 baseAddr);
/**
 * Check if HDCP is instantiated in hardware
 * @param baseAddr base address of controller
 * @return TRUE if hardware supports HDCP encryption
 */
int control_SupportsHdcp(u16 baseAddr);

/** 
 * Mute controller interrupts
 * @param baseAddr base address of controller
 * @param value mask of the register
 * @return TRUE when successful
 */
int control_InterruptMute(u16 baseAddr, u8 value);
/** 
 * Clear CEC interrupts
 * @param baseAddr base address of controller
 * @param value
 * @return TRUE if successful
 */
int control_InterruptCecClear(u16 baseAddr, u8 value);
/**
 * @param baseAddr base address of controller
 * @return CEC interrupts state 
 */
u8 control_InterruptCecState(u16 baseAddr);
/** 
 * Clear EDID reader (I2C master) interrupts
 * @param baseAddr base address of controller
 * @param value
 * @return TRUE if successful
 */
int control_InterruptEdidClear(u16 baseAddr, u8 value);

/** 
 * @param baseAddr base address of controller
 * @return EDID reader interrupts state (I2C master controller)
 */
u8 control_InterruptEdidState(u16 baseAddr);
/** 
 * Clear phy interrupts
 * @param baseAddr base address of controller
 * @param value
 * @return TRUE if successful
 */
int control_InterruptPhyClear(u16 baseAddr, u8 value);

/**
 * @param baseAddr base address of controller
 * @return PHY interrupts state 
 */
u8 control_InterruptPhyState(u16 baseAddr);
/** 
 * Clear all controller interrputs (except for hdcp)
 * @param baseAddr base address of controller
 * @return TRUE if successful
 */
int control_InterruptClearAll(u16 baseAddr);


#endif /* CONTROL_H_ */
