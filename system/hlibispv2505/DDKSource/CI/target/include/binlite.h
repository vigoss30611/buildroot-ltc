/*! 
******************************************************************************
 @file   : binlite.h

 @Author Tom Hollis

 @date   06/09/2011
 
         <b>Copyright 2011 by Imagination Technologies Limited.</b>\n
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
         Header file describing enumerated codes for binary lite format

 <b>Platform:</b>\n
	     Platform Independent 

 @Version	
	     1.0 

******************************************************************************/

#if !defined (__BINLITE_H__)
#define __BINLITE_H__


#if defined (__cplusplus)
extern "C" {
#endif

/*!
******************************************************************************

This type defines the Binary Lite commands.
NOTE: To ensure backward compatiblity exiting command IDs should NOT be changed.

******************************************************************************/
typedef enum
{
	TAL_PDUMPL_WRW_REG				= 0x00000000, 	//!< Write word to register
	TAL_PDUMPL_WRW_MEM				= 0x00000001,	//!< Write word to memory
	TAL_PDUMPL_WRW_SLV				= 0x00000002,	//!< Write word to slave port (Deprecated)
	TAL_PDUMPL_RDW_REG				= 0x00000003,	//!< Read word from register
	TAL_PDUMPL_RDW_MEM				= 0x00000004,	//!< Read word from memory
	TAL_PDUMPL_POL_REG				= 0x00000005,	//!< Poll register
	TAL_PDUMPL_POL_MEM				= 0x00000006,	//!< Poll memory
	TAL_PDUMPL_LDB_MEM				= 0x00000007,	//!< Load bytes to memory
	TAL_PDUMPL_LDW_REG				= 0x00000008,	//!< Load words to register
	TAL_PDUMPL_LDW_SLV				= 0x00000009,	//!< Load words to slave port (Deprecated)
	TAL_PDUMPL_IDL					= 0x0000000A,	//!< Idle command
	TAL_PDUMPL_CBP_MEM				= 0x0000000B,	//!< Poll for circular buffer space in memory
	TAL_PDUMPL_INTREG_WRW_REG		= 0x0000000C,	//!< Write word from internal register to register
	TAL_PDUMPL_INTREG_WRW_MEM		= 0x0000000D,	//!< Write word from internal register to memory
	TAL_PDUMPL_INTREG_RDW_REG		= 0x0000000E,	//!< Read word from register into internal register
	TAL_PDUMPL_INTREG_RDW_MEM		= 0x0000000F,	//!< Read word from memory into internal register
	TAL_PDUMPL_INTREG_MOV			= 0x00000010,	//!< Internal register command - MOVE
	TAL_PDUMPL_INTREG_AND			= 0x00000011,	//!< Internal register command - AND
	TAL_PDUMPL_INTREG_OR			= 0x00000012,	//!< Internal register command - OR
	TAL_PDUMPL_INTREG_XOR			= 0x00000013,	//!< Internal register command - XOR
	TAL_PDUMPL_INTREG_NOT			= 0x00000014,	//!< Internal register command - NOT
	TAL_PDUMPL_INTREG_SHR			= 0x00000015,	//!< Internal register command - SHR
	TAL_PDUMPL_INTREG_SHL			= 0x00000016,	//!< Internal register command - SHL
	TAL_PDUMPL_INTREG_ADD			= 0x00000017,	//!< Internal register command - ADD
	TAL_PDUMPL_INTREG_SUB			= 0x00000018,	//!< Internal register command - SUB
	TAL_PDUMPL_INTREG_MUL			= 0x00000019,	//!< Internal register command - MUL
	TAL_PDUMPL_INTREG_IMUL			= 0x0000001A,	//!< Internal register command - IMUL
	TAL_PDUMPL_INTREG_DIV			= 0x0000001B,	//!< Internal register command - DIV
	TAL_PDUMPL_INTREG_IDIV			= 0x0000001C,	//!< Internal register command - IDIV
	TAL_PDUMPL_INTREG_MOD			= 0x0000001D,	//!< Internal register command - MOD
	TAL_PDUMPL_INTREG_EQU			= 0x0000001E,	//!< Internal register command - EQU
	TAL_PDUMPL_INTREG_LT			= 0x0000001F,	//!< Internal register command - LT
	TAL_PDUMPL_INTREG_LTE			= 0x00000020,	//!< Internal register command - LTE
	TAL_PDUMPL_INTREG_GT			= 0x00000021,	//!< Internal register command - GT
	TAL_PDUMPL_INTREG_GTE			= 0x00000022,	//!< Internal register command - GTE
	TAL_PDUMPL_INTREG_NEQ			= 0x00000023,	//!< Internal register command - NEQ
	TAL_PDUMPL_CBP_REG				= 0x00000024,	//!< Poll for circular buffer space in register
	TAL_PDUMPL_SYNC					= 0x00000025,	//!< Multi Script synchronise command	
	TAL_PDUMPL_LOCK					= 0x00000026,	//!< Multi Script semaphore lock command
	TAL_PDUMPL_UNLOCK				= 0x00000027,	//!< Multi Script semaphore unlock command
	TAL_PDUMPL_WRW_MEM_64			= 0x00000028,	//!< Write word to 64bit memory address
	TAL_PDUMPL_RDW_MEM_64			= 0x00000029,	//!< Read word from 64bit memory address
	TAL_PDUMPL_POL_MEM_64			= 0x0000002A,	//!< Poll 64bit memory address	
	TAL_PDUMPL_LDB_MEM_64			= 0x0000002B,	//!< Load bytes to 64bit memory address
	TAL_PDUMPL_CBP_MEM_64			= 0x0000002C,	//!< Poll for circular buffer space in 64bit memory address
	TAL_PDUMPL_INTREG_WRW_MEM_64	= 0x0000002D,	//!< Write word from internal register to 64bit memory address
	TAL_PDUMPL_INTREG_RDW_MEM_64	= 0x0000002E,	//!< Read word from 64bit memory address into internal register
	TAL_PDUMPL_INTREG_MOV_64		= 0x0000002F,	//!< Internal register command - MOVE 64bit value
	TAL_PDUMPL_PADDING				= 0x00000030,	//!< Padding Command - 1byte long only
	TAL_PDUMPL_SAB					= 0x00000031,	//!< Save Bytes, indicates location of output data in memory	
	TAL_PDUMPL_SAB_64				= 0x00000032,	//!< Save Bytes, indicates location of output data in 64bit memory
	TAL_PDUMPL_WRW_REG_64			= 0x00000033, 	//!< Write word to 64bit register address
	TAL_PDUMPL_RDW_REG_64			= 0x00000034,	//!< Read word from 64bit register address
	TAL_PDUMPL_INTREG_WRW_REG_64	= 0x00000035,	//!< Write word from internal register to 64bit register address
	TAL_PDUMPL_INTREG_RDW_REG_64	= 0x00000036,	//!< Read word from 64bit register address into internal register
	TAL_PDUMPL_POL_REG_64			= 0x00000037,	//!< Poll 64bit register address
	
	// The following are versions of existing functions which read or write 64bits instead of 32bit
	// All of these functions use 64bit addresses
	TAL_PDUMPL_WRW64_REG_64			= 0x00000038, 	//!< Write 64bit word to register
	TAL_PDUMPL_WRW64_MEM_64			= 0x00000039,	//!< Write 64bit word to memory
	TAL_PDUMPL_RDW64_REG_64			= 0x0000003A,	//!< Read 64bit word from register
	TAL_PDUMPL_RDW64_MEM_64			= 0x0000003B,	//!< Read 64bit word from memory
	TAL_PDUMPL_POL64_REG_64			= 0x0000003C,	//!< Poll 64bit register
	TAL_PDUMPL_POL64_MEM_64			= 0x0000003D,	//!< Poll 64bit memory
	TAL_PDUMPL_INTREG_WRW64_REG_64	= 0x0000003E,	//!< Write 64bit word from internal register to register
	TAL_PDUMPL_INTREG_WRW64_MEM_64	= 0x0000003F,	//!< Write 64bit word from internal register to memory
	TAL_PDUMPL_INTREG_RDW64_REG_64	= 0x00000040,	//!< Read 64bit word from register into internal register
	TAL_PDUMPL_INTREG_RDW64_MEM_64	= 0x00000041,	//!< Read 64bit word from memory into internal register

	
	TAL_PDUMPL_NO_OPTIMISE			= 0x80000000	//!< Flag which tells the optimiser not to optimise this command

} ePDUMPLCommand;

#define TAL_PDUMPL_SCRIPT_START_SIG		(0x504D4450)		//!< Signature used as first 32bit word in all standard binlite scripts
#define TAL_PDUMPL_SCRIPT_END_SIG		(0x504F5453)		//!< Signature used as last 32bit word in all standard binlite scripts

#define TAL_PDUMPL_COMPSCRIPT_HSTART_SIG	(0x41545348)	//!< Signature used as first 32bit word in all composite (multi-pdump) binlite scripts
#define TAL_PDUMPL_COMPSCRIPT_HEND_SIG		(0x444E4548)	//!< Signature used as last 32bit word in all composite (multi-pdump) binlite scripts

#if defined (__cplusplus)
}
#endif

#endif /* __BINLITE_H__	*/

