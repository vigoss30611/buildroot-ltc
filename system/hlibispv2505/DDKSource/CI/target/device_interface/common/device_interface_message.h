/*!
********************************************************************************
 @File   device_interface_message.h

 @Brief  Client/Server generic information defining the
		  message passing for device interface over network
 
 @Author Imagination Technologies

 @Date   30/08/2013
 
         <b>Copyright 2013 by Imagination Technologies Limited.</b>\n
         All rights reserved.  No part of this software, either
         material or conceptual may be copied or distributed,
         transmitted, transcribed, stored in a retrieval system
         or translated into any human or computer language in any
         form by any means, electronic, mechanical, manual or
         other-wise, or disclosed to third parties without the
         express written permission of Imagination Technologies
         Limited, Unit 8, HomePark Industrial Estate,
         King's Langley, Hertfordshire, WD4 8LZ, U.K.

*******************************************************************************/
#ifndef __DEVICE_INTERFACE_MESSAGE_H__
#define __DEVICE_INTERFACE_MESSAGE_H__

#define DEVIF_PROTOCOL_MK_VERSION(major, minor) ((major) << 16 | (minor))		//!< Protocol version concatenator (fixed)
#define DEVIF_PROTOCOL_VERSION_MAJOR 2											//!< Protocol Major Version (can change)
#define DEVIF_PROTOCOL_VERSION_MINOR 1											//!< Protocol Minor Version (can change)

//! Protocol version number
#define DEVIF_PROTOCOL_VERSION \
    DEVIF_PROTOCOL_MK_VERSION(DEVIF_PROTOCOL_VERSION_MAJOR, DEVIF_PROTOCOL_VERSION_MINOR)

#define DEVIF_PROTOCOL_MAX_STRING_LEN (1024)			//!< Max string length allowed within protocol

#define DEVIF_INVALID_TIME (0xFFFFFFFFFFFFFFFFULL)		//!< Invalid time value (used when real time cannot be found)

//! Command Id List
enum EDeviceInterfaceCommand {
	E_DEVIF_READREG			= 0x00, 	//!< DeviceAddress - Await response
	E_DEVIF_WRITEREG		= 0x01,		//!< DeviceAddress, Value - no response
	E_DEVIF_LOADREG			= 0x02,		//!< DeviceAddress, NumBytes, Data - no response
	E_DEVIF_READREG_64		= 0x03,		//!< DeviceAddress - Await response
	E_DEVIF_WRITEREG_64		= 0x04,		//!< DeviceAddress - 64bit Value - no response
    E_DEVIF_POLLREG         = 0x05,     //!<
    E_DEVIF_POLLREG_64      = 0x06,     //!<
	E_DEVIF_ALLOC			= 0x10,		//!< DeviceAddress, NumBytes
	E_DEVIF_FREE			= 0x11,		//!< DeviceAddress, - no response
	E_DEVIF_ALLOC_SLAVE 	= 0x12,		//!< Alignment, NumBytes - Await Response (address)
	E_DEVIF_READMEM			= 0x20,		//!< DeviceAddress - Await response
	E_DEVIF_WRITEMEM		= 0x21,		//!< DeviceAddress, Value - no response
	E_DEVIF_COPYHOSTTODEV	= 0x22,		//!< DeviceAddress, NumBytes, Data - no response
	E_DEVIF_COPYDEVTOHOST	= 0x23,		//!< DeviceAddress, NumBytes - Await response
	E_DEVIF_READMEM_64		= 0x24, 	//!< DeviceAddress - Await response
	E_DEVIF_WRITEMEM_64		= 0x25,		//!< DeviceAddress, 64bit Value - no response
    E_DEVIF_POLLMEM         = 0x26,     //!<
    E_DEVIF_POLLMEM_64      = 0x27,     //!<
	E_DEVIF_WRITESLAVE		= 0x30,		//!< DeviceAddress, Value - no response 
	E_DEVIF_IDLE			= 0x80,		//!< idle time (cycles) - no response
	E_DEVIF_AUTOIDLE		= 0x81,		//!< auto idle time (cycles) - no response
	E_DEVIF_END				= 0xa0,     //!< - no response
	E_DEVIF_RESET			= 0xa1,     //!< - no response
	E_DEVIF_TRANSFER_MODE	= 0xf9,		//!< Mode - no response
	E_DEVIF_PRINT			= 0xfa,		//!< String Len, Text to print out - no response
	E_DEVIF_GET_DEV_TIME	= 0xfb,		//!< none, Response 8 byte time value
	E_DEVIF_SEND_COMMENT	= 0xfc,		//!< String Len, Text to add to comment - no response
	E_DEVIF_GET_DEVICENAME	= 0xfd,     //!< Respose is null terminated string
	E_DEVIF_GET_TAG			= 0xfe,     //!< Respose is null terminated string
	E_DEVIF_GET_DEVIF_VER	= 0xff,     //!< Respose 4 byte identifier of devif protocol num

	// Commands to use more than one device over a socket
	// Creates a new socket for a new device
	E_DEVIF_SETUP_DEVICE	= 0xe0,		//!< Device Name - port num
	E_DEVIF_SET_BASENAMES	= 0xe1,		//!< A subdevice allows an additional device to be used on the same socket
	E_DEVIF_USE_DEVICE		= 0xe2,		//!< Device Id(Port Number) - no response
};

//! Transfer mode Id
enum ETransferMode {
	E_DEVIFT_32BIT, 					//!< Use two 32bit words for each transfer
	E_DEVIFT_64BIT,						//!< Use the optimised 64bit tranfer mode
};

//! Prefix id
enum EPrefixType {
	E_DEVIFP_MEM,						//!< Memory Prefix
	E_DEVIFP_REG,						//!< Register Prefix
};

#endif /* __DEVICE_INTERFACE_MESSAGE_H__ */
