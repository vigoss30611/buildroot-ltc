/*!
******************************************************************************
 @file   : devif_setup.h

 @brief	 Device Interface setup structures

 @Author Tom Hollis

 @date   20/05/2013

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

 <b>Description:</b>\n
         Setup structures for each interface

 <b>Platform:</b>\n
	     Platform Independent

 @Version
	     1.0

******************************************************************************/
/*
******************************************************************************
*/



#if !defined (DEVIF_SETUP_H)
#define DEVIF_SETUP_H

#include <img_types.h>

#if defined (__cplusplus)
extern "C" {
#endif

typedef enum
{
	DEVIF_TYPE_PCI,
	DEVIF_TYPE_BMEM,
	DEVIF_TYPE_SOCKET,
	DEVIF_TYPE_PDUMP1,
	DEVIF_TYPE_DASH,
	DEVIF_TYPE_DIRECT,
	DEVIF_TYPE_NULL,
	DEVIF_TYPE_POSTED
	
}eDevifTypes;

/*!
******************************************************************************
  The wrapper control information.
******************************************************************************/
typedef struct
{	
	/*! host/target cycle ratio              */
	IMG_UINT32		ui32HostTargetCycleRatio;	//<! Ratio of Host clock to Device Clock where Idles cannot be run on the Device
	eDevifTypes		eDevifType;					//<! Type of Device Interface to be used
} DEVIF_sWrapperControlInfo;

typedef struct
{	
    IMG_CHAR *      pszDeviceName;				//<! Device name
    IMG_UINT32      ui32VendorId;				//<! Vendor ID.
	IMG_UINT32      ui32DeviceId;				//<! Device ID. 
	IMG_UINT32      ui32RegBar;					//<! Register Bar.
	IMG_UINT32      ui32RegOffset;				//<! Offset.
	IMG_UINT32      ui32RegSize;				//<! Size.
	IMG_UINT32      ui32MemBar;					//<! Bar.
	IMG_UINT32      ui32MemOffset;				//<! Offset.
	IMG_UINT32      ui32MemSize;				//<! Size.
	IMG_UINT64      ui64BaseAddr;				//<! Device Base Address (required for calculation of virtual address)
	IMG_BOOL		bPCIAddDevOffset;			//<! Include the device offset in PCI address calculations
} DEVIF_sPciInfo;

typedef struct
{
    IMG_CHAR *      pszDeviceName;				//<! Device name
    IMG_CHAR        cDeviceId;					//<! Device ID. 
    IMG_UINT32      ui32MemStartOffset;		//<! Memory start offset
    IMG_UINT32      ui32MemSize;				//<! Memory size
} DEVIF_sBurnMemInfo;

typedef struct 
{
	IMG_CHAR * 		pszDeviceName;				//<! Device name
	IMG_CHAR *		pcDashName;					//<! Dash Name
	IMG_UINT32		ui32DeviceBase;				//<! Base Address of Device(Regs)
	IMG_UINT32		ui32DeviceSize;				//<! Size of Device
	IMG_UINT32		ui32MemoryBase;				//<! Base Address of Memory
	IMG_UINT32		ui32MemorySize;				//<! Memory Size
} DEVIF_sDashIFInfo;

typedef struct _DEVIF_sDirectIFInfo
{
    IMG_CHAR *      				pszDeviceName;				//<! Device name
    struct _DEVIF_sDirectIFInfo	*psParentDevInfo;			//<! Parent Device Info Struct
} DEVIF_sDirectIFInfo;

typedef struct _DEVIF_sDeviceIpInfo
{	
    IMG_CHAR *						pszDeviceName;				//<! Device name
    struct _DEVIF_sDeviceIpInfo	*psParentDevInfo;			//<! Parent Device Name, parent socket on which to start this device.
    IMG_UINT32						ui32PortId;					//<!  Port Id
	IMG_UINT32						ui32IpAddr1;				//<! IP address 1 
	IMG_UINT32						ui32IpAddr2;				//<! IP address 2 
	IMG_UINT32						ui32IpAddr3;				//<! IP address 3
	IMG_UINT32						ui32IpAddr4;				//<! IP address 4 
	IMG_CHAR *						pszRemoteName;				//<! Remote Machine, Host Name (Alternative to IP address)
	IMG_BOOL						bUseBuffer;					//<! Use the socket buffer
} DEVIF_sDeviceIpInfo;

typedef struct
{
    IMG_CHAR *      pszDeviceName;				//<!  Device name
	IMG_CHAR *		cpCommandFile;				//<! File sharing pdump commands
	IMG_CHAR *		cpInputDirectory;			//<! Directory in which files being read should be
	IMG_CHAR *		cpOutputDirectory;			//<! Directory in which files being written should be
	IMG_CHAR *		cpSendData;					//<! File for transfer of data between device and host
	IMG_CHAR *		cpReturnData;				//<! File for returned data
} DEVIF_sPDump1IFInfo;

typedef struct TCONF_sPostIFInfo_tag
{
	IMG_CHAR *      pszDeviceName;				//!< Device name
	IMG_UINT32      ui32VendorId;				//!< Vendor ID.
	IMG_UINT32      ui32DeviceId;				//!< Device ID. 
	IMG_UINT32      ui32RegBar;					//<! Register Bar.
	IMG_UINT32      ui32RegOffset;				//<! Offset.
	IMG_UINT32      ui32RegSize;				//<! Size.
	IMG_UINT32      ui32MemBar;					//<! Bar.
	IMG_UINT32      ui32MemOffset;				//<! Offset.
	IMG_UINT32      ui32MemSize;				//<! Size.
	IMG_UINT64      ui64BaseAddr;				//<! Device Base Address (required for calculation of virtual address)
	IMG_BOOL		bPCIAddDevOffset;			//<! Include the device offset in PCI address calculations
	IMG_UINT32      ui32PostedIfOffset;		//!< Address offset of posted IF registers 
	IMG_UINT32      ui32DeviceNo;				//!< Device No.
	IMG_UINT32      ui32BufferSize;			//!< Size of Command and Read buffers
} DEVIF_sPostIFInfo;

/*!
******************************************************************************
  Base name info
******************************************************************************/
typedef struct
{
	IMG_CHAR *          pszDeviceName;		//! Device name
	IMG_CHAR *			pszRegBaseName;		//!< Register Basename (used in RTL Select Vector)
	IMG_CHAR *			pszMemBaseName;		//!< Memory Basename (used in RTL Select Vector)
} DEVIF_sBaseNameInfo;



/*!
******************************************************************************
  "Full" set of information about the device.
******************************************************************************/
typedef struct
{	
	DEVIF_sWrapperControlInfo	sWrapCtrlInfo;	//!< Wrapper control info
	DEVIF_sBaseNameInfo			sBaseNameInfo;	//!< Device Base Name info
	union
	{
        DEVIF_sPciInfo			sPciInfo;		//!< PCI info
		DEVIF_sBurnMemInfo		sBurnMemInfo;	//!< Burnmem info
		DEVIF_sDashIFInfo		sDashIFInfo;	//!< DASH info
		DEVIF_sDirectIFInfo		sDirectIFInfo;	//!< Direct info
		DEVIF_sDeviceIpInfo		sDeviceIpInfo;	//!< IP info	
		DEVIF_sPDump1IFInfo		sPDump1IFInfo;	//!< PDUMP1 info
		DEVIF_sPostIFInfo		sPostIFInfo;	//!< Post info
    }u;
} DEVIF_sFullInfo;
#if defined (__cplusplus)
}
#endif

#endif /* DEVIF_SETUP_H	*/



