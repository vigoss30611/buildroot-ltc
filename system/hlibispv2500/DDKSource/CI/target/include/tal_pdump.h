/*!
******************************************************************************
 @file   : tal_pdump.h

 @brief	API for the Target Abstraction Layer Parameter Dumping.

 @Author Tom Hollis

 @date   19/08/2010

         <b>Copyright 2010 by Imagination Technologies Limited.</b>\n
         All rights reserved.  No part of this software, either
         material or conceptual may be copied or distributed,
         transmitted, transcribed, stored in a retrieval system
         or translated into any human or computer language in any
         form by any means, electronic, mechanical, manual or
         other-wise, or disclosed to third parties without the
         express written permission of Imagination Technologies
         Limited, Unit 8, HomePark Industrial Estate,
         King's Langley, Hertfordshire, WD4 8LZ, U.K.

@Description
         Target Abstraction Layer (TAL) Device Debug Header File.

 @Platform
	     Platform Independent


******************************************************************************/

#if !defined (__TALPDUMP_H__)
#define __TALPDUMP_H__

#include "tal_defs.h"
#include "pdump_cmds.h"

#if defined (__cplusplus)
extern "C" {
#endif

#ifdef DOXYGEN_CREATE_MODULES
/**
 * @defgroup IMG_TAL Target Abstraction Layer
 * @{
 */
/*-------------------------------------------------------------------------
 * Following elements are in the IMG_TAL documentation module
 *------------------------------------------------------------------------*/
 /**
 * @name PDump functions
 * @{
 */
/*-------------------------------------------------------------------------
 * Following elements are in the PDump group
 *------------------------------------------------------------------------*/
#endif

/*!
******************************************************************************

 @Function				TALPDUMP_SetPdump1MemName

 @Description

 This function sets a base name for the pdump1 memory bus, if set to null
 it will use the pdump2 memory space names and offsets from those names.

 @Input		pszMemName    : Pointer to memory base name.
 @Return	None.

******************************************************************************/
IMG_VOID TALPDUMP_SetPdump1MemName (
	IMG_CHAR*					pszMemName
);


/*!
******************************************************************************

 @Function				TALPDUMP_Comment

 @Description

 This function is used to output a comment to the PDUMP file.
 The Memspace can be set to IMG_NULL to write a comment but not have it
 filtered by the capture routines.

 @Input		hMemSpace		: A handle for the memory space.
 @Input		psCommentText	: A pointer to a string of text to be output to the
								PDUMP file.  The string is prefixed with the comment
								qualifier "-" and terminated by a newline.
 @Return	IMG_RESULT		: IMG_SUCCESS if the memory space is supported by
								the target. Asserts are use to trap unsupported
								memory spaces.

******************************************************************************/
IMG_VOID TALPDUMP_Comment(
	IMG_HANDLE				hMemSpace,
	IMG_CONST IMG_CHAR *		psCommentText
);


/*!
******************************************************************************

 @Function				TALPDUMP_MiscOutput

 @Description

 Exactly the same as TALPDUMP_Comment, but doesn't output the "--" prefix.

 NOTE: Should be used with care as the output may not be recognised by the
 tools reading the PDUMP file.
 NOTE: Potentially writes to both PDUMP1 and PDUMP2 script files.

 @Input		hMemSpace		: A handle for the memory space.
 @Input		pOutputText		: A pointer to a string of text to be output to the
								PDUMP file.  The string is terminated by a newline.
 @Return	None.

******************************************************************************/
IMG_VOID TALPDUMP_MiscOutput(
	IMG_HANDLE				hMemSpace,
	IMG_CHAR *				pOutputText
);

/*!
******************************************************************************

 @Function				TALPDUMP_ConsoleMessage

 @Description

 This function is used to output a message to the PDUMP file
 which will be printed on playback.
 a memspace handle value of IMG_NULL can be  used to write a
 message but not have it filtered by the capture routines.

 @Input		hMemSpace		: A handle for the memory space.
 @Input		psCommentText	: A pointer to a string of text to be output to the
								PDUMP file.  The string is prefixed with the console message
								qualifier "COM" or "CON" and terminated by a newline.
 @Return	None.

******************************************************************************/
IMG_VOID TALPDUMP_ConsoleMessage(
	IMG_HANDLE				hMemSpace,
    IMG_CHAR *              psCommentText
);


/*!
******************************************************************************

 @Function				TALPDUMP_SetConversionSourceData

 @Description

 The function is to be used to set up source file related parameters when TAL is
 used in conversion of PDUMP scripts between formats. Currently, the information is
 used by the TAL in generating debug parameters in Pdump Lite POL-like commands.
 The function should be called as often as needed to ensure that the parameters
 in question are up to date in the resulting converted script.

 @Input		ui32SourceFileID	: The ID number of the source file being converted
 @Input		ui32SourceLineNum	: Current source file line number
 @Return	None.

******************************************************************************/
IMG_VOID TALPDUMP_SetConversionSourceData(
    IMG_UINT32				ui32SourceFileID,
    IMG_UINT32				ui32SourceLineNum
);



/*!
******************************************************************************

 @Function				TALPDUMP_MemSpceCaptureEnable

 @Description

 This function is used to enable capture of PDUMP information for a memory
 space.  A null handle can be used to enable capture of all memory spaces.

 NOTE: A device may have a number of associated memory spaces, all 
 associated memory spaces should be enabled for the PDUMP information to be
 meaningful.

 Capture for the required memory spaces must be enabled before starting the capture.

 @Input		hMemSpace		: The target memory space.
 @Input		bEnable			: If IMG_TRUE, enables PDUMPing for the memory space.
								If IMG_FALSE, disables PDUMPing for the memory space.
 @Output	pbPrevState		: If hMemSpace not null then this
								pointer is used to return the previous PDUMP enable
								state for the specified memory space.
 @Return	None.

******************************************************************************/
IMG_VOID TALPDUMP_MemSpceCaptureEnable (
	IMG_HANDLE						hMemSpace,
	IMG_BOOL						bEnable,
	IMG_BOOL *						pbPrevState
);

/*!
******************************************************************************

 @Function				TALPDUMP_DisableCmds

 @Description

 This function is used to disable the capture of specified PDUMP commands
 for globally, by memory space or for a specific register.

 Disabling the capture of commands should be done before starting the capture.

 @Input		hMemSpace			: The memory space or IMG_NULL.
 @Input		ui32Offset			: The offset, in bytes, of the register to be disable
									or #TAL_OFFSET_ALL.
 @Input		ui32DisableFlags	: Capture disable flags (see TAL_DISABLE_CAPTURE_xxx).
 @Return	None.

******************************************************************************/
IMG_VOID TALPDUMP_DisableCmds(
	IMG_HANDLE						hMemSpace,
	IMG_UINT32						ui32Offset,
	IMG_UINT32						ui32DisableFlags
);

/*!
******************************************************************************

 @Function				TALPDUMP_SetFlags

 @Description

 This function sets the given PDUMP flags

 @Input		ui32Flags		: PDUMP bitwise flags
 @Return	None

******************************************************************************/
IMG_VOID TALPDUMP_SetFlags( IMG_UINT32 ui32Flags );


/*!
******************************************************************************

 @Function				TALPDUMP_ClearFlags

 @Description

 This function clears the given PDUMP flags.
 (0xFFFFFFFF will clear all)

 @Input		ui32Flags		: PDUMP bitwise flags
 @Return	None

******************************************************************************/
IMG_VOID TALPDUMP_ClearFlags( IMG_UINT32 ui32Flags );



/*!
******************************************************************************

 @Function				TALPDUMP_GetFlags

 @Description

 This function gets the current PDUMP flags

 @Return	IMG_UINT32	: Current PDUMP bitwise flags

******************************************************************************/
IMG_UINT32 TALPDUMP_GetFlags(IMG_VOID);


/*!
******************************************************************************

 @Function				TALPDUMP_SetFilename

 @Description

 This function is used to set one of the filenames being used.

 @Input		hMemSpace		: The memory space of the file.
 @Input		eSetFilename	: Defines the filename to be set.
 @Input		psFileName		: Pointer to the filename.

 @Return	None.

******************************************************************************/
IMG_VOID TALPDUMP_SetFilename (
    IMG_HANDLE                      hMemSpace,
	TAL_eSetFilename		        eSetFilename,
	IMG_CHAR *				        psFileName
);

/*!
******************************************************************************

 @Function				TALPDUMP_GetFilename

 @Description

 This function is used to get one of the filenames being used.

 @Input		hMemSpace		: The memory space of the file.
 @Input		eSetFilename	: Defines the filename to get.
 @Return	IMG_CHAR *		: Pointer to the filename.

******************************************************************************/
IMG_CHAR * TALPDUMP_GetFilename (
    IMG_HANDLE                      hMemSpace,
	TAL_eSetFilename		        eSetFilename
);

/*!
******************************************************************************

 @Function				TALPDUMP_SetFileoffset

 @Description

 The function is used in setting the .res or .prm offset parameter in the PDUMP command to a
 specific value. This command is only intended to be used in PDUMP conversion
 process, rather than in PDUMP capture, and should only be used after a filename
 change.

 @Input		hMemSpace		: The memory space of the file.
 @Input		eSetFilename	: Defines the PDUMP file to be set. Only .res and .prm are allowed.
 @Input		ui64FileOffset	: The offset into the file.
 @Return	None.

******************************************************************************/

IMG_VOID TALPDUMP_SetFileoffset (
    IMG_HANDLE                      hMemSpace,
	TAL_eSetFilename		        eSetFilename,
	IMG_UINT64				        ui64FileOffset
);

/*!
******************************************************************************

 @Function				TALPDUMP_AddMemspaceToContext

 @Description

 This function is used to define a PDUMP context.
 NOTE: pszContextName is the "name" of the context and is used as a prefix on the
 PDUMP files.

 @Input		pszContextName      : The "name" of the PDUMP context.
 @Input		hMemSpaceCB   		 : A memspace handle
 @Return    nothing

******************************************************************************/
void TALPDUMP_AddMemspaceToContext (
	IMG_CHAR *	 		pszContextName,
	IMG_HANDLE			hMemSpaceCB
);

/*!
******************************************************************************

 @Function				TALPDUMP_EnableContextCapture

 @Description

 This function is used to enable PDUMPing for a given context.

 @warning API CHANGE no longer disables the other contexts!

 @Input		hMemSpace      : A handle to a memspace in the context
 @Return    None.

******************************************************************************/
IMG_VOID TALPDUMP_EnableContextCapture (
	IMG_HANDLE             hMemSpace
);

/**
 * @brief Disable the PDUMPing for the context of the given memspace
 */
IMG_VOID TALPDUMP_DisableContextCapture (IMG_HANDLE             hMemSpace);

/*!
******************************************************************************

 @Function				TALPDUMP_GetNoContexts

 @Description

 This function is determine the number of Pdump contexts.

 @Return    IMG_UINT32	: The number of Pdump contexts.

******************************************************************************/
IMG_UINT32 TALPDUMP_GetNoContexts (IMG_VOID);

/*!
******************************************************************************

 @Function				TALPDUMP_AddSyncToTDF

 @Description

 This function adds a sync setup to a TDF file so it will be repeated in the pdump.

 @Input		ui32SyncId	: The Sync Identifier.
 @Input		hMemSpace	: a memspace which belongs to the context
							to which the sync is to be added.

 @Return    None.

******************************************************************************/

IMG_VOID TALPDUMP_AddSyncToTDF (
	IMG_HANDLE			hMemSpace,
	IMG_UINT32			ui32SyncId
	
);

/**
 * @brief Adds a SYNC ID to all enabled pdump contexts
 *
 * @Input ui32SyncId sync identifier (>= TAL_NO_SYNC_ID and < TAL_MAX_SYNC_ID)
 */
IMG_VOID TALPDUMP_AddBarrierToTDF(IMG_UINT32 ui32SyncId);

/*!
******************************************************************************

 @Function				TALPDUMP_AddCommentToTDF

 @Description

 This function adds a comment to the Test Description File
 @Input		pcComment	: A comment which will be added to the start of the
							Test Description file.
 @Return    None.

******************************************************************************/

IMG_VOID TALPDUMP_AddCommentToTDF (IMG_CHAR * pcComment);


/*!
******************************************************************************

 @Function				TALPDUMP_AddTargetConfigToTDF

 @Description

 This function adds a link to the target config file to the test description file.

 @Input		pszFilePath		: The relative path to the target config file from
								the TDF which will be added to the Test Description file.
 @Return    None.

******************************************************************************/

IMG_VOID TALPDUMP_AddTargetConfigToTDF (IMG_CHAR * pszFilePath);


/*!
******************************************************************************

 @Function				TALPDUMP_GenerateTDF

 @Description

 This function creates the TDF file in the given location

 @Return    None.

******************************************************************************/
IMG_VOID TALPDUMP_GenerateTDF (IMG_CHAR * psFilename);


/*!
******************************************************************************

 @Function				TALPDUMP_CaptureStart

 @Description

 This function is used to start capture of PDUMP information for those
 peripherals where capture has been enabled.

 Capture for the required memory spaces must be enabled before initiating capture.

 @Input		psTestDirectory	: The path for the captured test parameters and results.
 @Return	None.

******************************************************************************/
IMG_VOID TALPDUMP_CaptureStart(
	IMG_CHAR *							psTestDirectory
);

/*!
******************************************************************************

 @Function				TALPDUMP_CaptureStop

 @Description

 This function is used to stop capture of PDUMP information.

 Capture is automatically disabled for all memory spaces and must be enabled
 prior to initiating the next capture.

 @Return	None.

******************************************************************************/
IMG_VOID TALPDUMP_CaptureStop(IMG_VOID);

/*!
******************************************************************************

 @Function				TALPDUMP_ChangeResFileName

 @Description			Changes the name of the .res file to be used for the
						Pdump context

 @Input		hMemSpace	: A memory space within the relevant context.
 @Input		sFileName	: The new name for the file.
 @Return	None.

******************************************************************************/
IMG_VOID TALPDUMP_ChangeResFileName( 
	IMG_HANDLE		hMemSpace,
	IMG_CHAR *		sFileName 
);

/*!
******************************************************************************

 @Function				TALPDUMP_ChangePrmFileName

 @Description			Changes the name of the .prm file to be used for the
						Pdump context.

 @Input		hMemSpace	: A memory space within the relevant context.
 @Input		sFileName	: The new name for the file.
 @Return	None.

******************************************************************************/
IMG_VOID TALPDUMP_ChangePrmFileName( 
	IMG_HANDLE		hMemSpace,
	IMG_CHAR *		sFileName 
);


/*!
******************************************************************************

 @Function				TALPDUMP_ResetResFileName

 @Description			Resets the current RES file name back to the default.

 @Input		hMemSpace	: A memory space within the relevant context.
 @Return	None.

******************************************************************************/
IMG_VOID TALPDUMP_ResetResFileName( 
	IMG_HANDLE		hMemSpace
);

/*!
******************************************************************************

 @Function				TALPDUMP_ResetPrmFileName

 @Description			Resets the current PRM file name back to the default.

 @Input		hMemSpace	: A memory space within the relevant context.
 @Return	None.

******************************************************************************/
IMG_VOID TALPDUMP_ResetPrmFileName( 
	IMG_HANDLE		hMemSpace
);



/*!
******************************************************************************

 @Function				TALPDUMP_IsCaptureEnabled

 @Description

 This function is establish whether TAL parameter dump capturing is currently enabled.

 @Return	IMG_BOOL	: Returns IMG_TRUE if capture is enabled.

******************************************************************************/

IMG_BOOL TALPDUMP_IsCaptureEnabled(IMG_VOID);

/*!
******************************************************************************

 @Function				TALPDUMP_GetSemaphoreId

 @Description

 This function returns a semaphore id assigned by the TAL.
 It can be defined by a source and destiniation MemSpace or
 user defined by using a null value as source.

 @Input		hSrcMemSpace	: The source memory space or IMG_NULL.
 @Input		hDestMemSpace	: The destination memory space.
								if Source is null this is a user defined ID (not 0).
 @Return	IMG_UINT32		: The alloted semaphore Id.

******************************************************************************/
IMG_UINT32 TALPDUMP_GetSemaphoreId(
	IMG_HANDLE						hSrcMemSpace,
	IMG_HANDLE						hDestMemSpace
);

/*!
******************************************************************************

 @Function				TALPDUMP_Lock

 @Description

 This function is used to write a "LOCK" command to the PDUMP2 output.
 "LOCK" is used to indicate that some important event is about to happen
 (e.g. the start of a render).  When the PDUMP Player is playing back multiple
 simultaneous scripts the "LOCK" command can be used to synchronise the playback.
 When a "LOCK" command is encountered, the playback of commands from the script
 is held up until, an "UNLOCK" command is issued on the same Id. (Much like a semaphore)

 @Input		hMemSpace	: The memory space.
 @Input		ui32SemaId	: Semaphore Id
 @Return	None

******************************************************************************/
IMG_VOID TALPDUMP_Lock(
	IMG_HANDLE						hMemSpace,
	IMG_UINT32						ui32SemaId
);

/*!
******************************************************************************

 @Function				TALPDUMP_Unlock

 @Description

 This function is the "UNLOCK" Semaphore command which compliments
 the "LOCK" command

 @Input		hMemSpace		: The memory space.
 @Input		ui32SemaId		: Semaphore Id
 @Return	None.

******************************************************************************/
IMG_VOID TALPDUMP_Unlock(
	IMG_HANDLE						hMemSpace,
	IMG_UINT32						ui32SemaId
);

/*!
******************************************************************************

 @Function				TALPDUMP_SyncWithId

 @Description

 As TAL_Sync(), but where the user can specify a sync ID, which allows the files 
 being synced to be controlled rather than waiting for all files.

 @Input		hMemSpace		: The memory space.
 @Input		ui32SyncId		: Sync Id 1 to 31 or TAL_NO_SYNC_ID.
 @Return	IMG_RESULT		: returns Success if sync can be performed

******************************************************************************/
IMG_RESULT TALPDUMP_SyncWithId(
	IMG_HANDLE						hMemSpace,
	IMG_UINT32						ui32SyncId
);

/*!
******************************************************************************

 @Function				TALPDUMP_Sync

 @Description

 This function is used to write a "SYNC" command to the PDUMP2 output.
 "SYNC" is used to indicate that some important event is about to happen
 (e.g. the start of a render).  When the PDUMP Player is playing back multiple
 simultaneous scripts the "SYNC" command can be used to synchronise the playback.
 When a "SYNC" command is encountered, the playback of commands from the script
 is held up until, all files reach a "SYNC" point, at which time the playback
 of all files is re-enabled.

 @Input		hMemSpace	: The memory space.
 @Return	None.

******************************************************************************/
IMG_VOID TALPDUMP_Sync(
	IMG_HANDLE						hMemSpace
);

/**
 * @brief Performs a SYNC on all available pdump context
 *
 * This function is usefull when all pdump context need to be synchronised
 * (e.g. for all context to wait for some device memory to be allocated before
 * being able to use it).
 *
 * This is a simple loop on the sync command for each enabled pdump context
 */
IMG_VOID TALPDUMP_Barrier(IMG_VOID);

/**
 * @brief Similar to TALPDUMP_Barrier() but with an identifier.
 *
 * This function is not helping for run time speed (all enabled context still have a sync)
 * but could be usefull to identify different barriers (or to differenciate a barrier and 
 * other sync commands).
 */
IMG_VOID TALPDUMP_BarrierWithId(IMG_UINT32 ui32SyncId);

/*!
******************************************************************************

 @Function				TALPDUMP_SetDefineStrings
 
 @Description 
 
 This function is used to set the define strings tested in TALPDUMP_If.

 @Input		psStrings : The set of defined strings.
 @Return	None

******************************************************************************/
IMG_VOID TALPDUMP_SetDefineStrings(
    TAL_Strings *					psStrings
);

/*!
******************************************************************************

 @Function				TALPDUMP_If

 @Description

 This function is used to test a define, if the string has been defined then 
 the If has passed and the TAL commands which follow will run (be passed to the device)
 until a TALPDUMP_Else or TALPDUMP_EndIf is called. If the string is not defined no TAL commands
 will run in this context until a TALPDUMP_Else or TALPDUMP_EndIf is called. Pdumping will occur
 regardless of the result of the test.

 @Input		hMemSpace		: A memory space in the relevant context.
 @Input		pszDefine		: The string which defines the test.
 @Return	None.

******************************************************************************/
IMG_VOID TALPDUMP_If(
    IMG_HANDLE						hMemSpace,
	IMG_CHAR*						pszDefine
);

/*!
******************************************************************************

 @Function				TALPDUMP_Else

 @Description

 This function switches to the opposite of the result of TALPDUMP_If.
 If all commands between TALPDUMP_If and this call have been ignored, then
 all calls from here until TALPDUMP_EndIf will run and vice versa.

 @Input		hMemSpace		: A memory space in the relevant context.
 @Return	None.

******************************************************************************/
IMG_VOID TALPDUMP_Else(
    IMG_HANDLE						hMemSpace
);

/*!
******************************************************************************

 @Function				TALPDUMP_EndIf

 @Description

 This function ends a block started by TALPDUMP_If.

 @Input		hMemSpace		: A memory space in the relevant context.
 @Return	None.

******************************************************************************/
IMG_VOID TALPDUMP_EndIf(
    IMG_HANDLE						hMemSpace
);

/*!
******************************************************************************

 @Function				TALPDUMP_TestIf

 @Description

 This function is used to test the TALPDUMP_If and associated functions to see if
 commands are currently being skipped.

 @Input		hMemSpace		: A memory space in the relevant context.
 @Return	IMG_BOOL		: True if commands are currently being skipped

******************************************************************************/
IMG_BOOL TALPDUMP_TestIf(
    IMG_HANDLE						hMemSpace
);

#ifdef DOXYGEN_CREATE_MODULES
/**
 * @}
 */
/*-------------------------------------------------------------------------
 * end of the PDump group
 *------------------------------------------------------------------------*/
/**
 * @}
 */
/*-------------------------------------------------------------------------
 * end of the IMG_TAL documentation module
 *------------------------------------------------------------------------*/
#endif 

#if defined (__cplusplus)
}
#endif

#endif //__TALPDUMP_H__

