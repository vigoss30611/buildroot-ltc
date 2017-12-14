/*! 
******************************************************************************
 @file   : pdump_cmds.h

 @brief	 API for Pdumping Output

 @Author Tom Hollis

 @date   20/08/2008
 
         <b>Copyright 2008 by Imagination Technologies Limited.</b>\n
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
         PDUMP_CMDS API Header File.

 <b>Platform:</b>\n
	     Platform Independent 

 @Version	
	     1.0 

******************************************************************************/
/* 
******************************************************************************
*/

#if !defined (__PDUMP_CMDS_H__)
#define __PDUMP_CMDS_H__

#include "img_defs.h"
#include "img_types.h"
#include "binlite.h"

#if defined (__cplusplus)
extern "C" {
#endif

#if !defined(TAL_MAX_PDUMP_MEMSPACE_NAMES)
    #define TAL_MAX_PDUMP_MEMSPACE_NAMES    (120)    /*!< Max. number of memory space names in PDUMP conetxt */
#endif

#define PDUMP_PREV_CAPTURE (0xFFFFFFFF)


#define TAL_DISABLE_CAPTURE_COMMENT					0x00000001	//!< Disables comments generation in PDUMP output.
#define TAL_DISABLE_CAPTURE_WRW						0x00000002	//!< Disables WRW generation in PDUMP output.
#define TAL_DISABLE_CAPTURE_RDW						0x00000004	//!< Disables RDW generation in PDUMP output. 
#define TAL_DISABLE_CAPTURE_POL						0x00000008	//!< Disables POL generation in PDUMP output. 
#define TAL_DISABLE_CAPTURE_LDB						0x00000010	//!< Disables LDB generation in PDUMP output. 
#define TAL_DISABLE_CAPTURE_WRW_DEVMEMREF			0x00000020	//!< Disables WRW generation in PDUMP output. 
#define TAL_DISABLE_CAPTURE_SAB						0x00000040	//!< Disables SAB generation in PDUMP output. 
#define TAL_DISABLE_CAPTURE_WRW_DEVMEM_DEVMEMREF	0x00000080	//!< Disables WRW_DEVMEM_DEVMEMREF generation in PDUMP output. 
#define TAL_DISABLE_CAPTURE_IDL						0x00000100	//!< Disables IDL generation in PDUMP output. 
#define TAL_DISABLE_CAPTURE_LDW						0x00000200	//!< Disables LDW generation in PDUMP output. 
#define TAL_DISABLE_CAPTURE_SAW						0x00000400	//!< Disables SAW generation in PDUMP output. 
#define TAL_DISABLE_CAPTURE_SII						0x00000800	//!< Disables SII generation in PDUMP output. 
#define TAL_DISABLE_CAPTURE_CBP						0x00001000	//!< Disables CBP generation in PDUMP output. 
#define TAL_DISABLE_CAPTURE_INTERNAL_REG_CMDS		0x00002000	//!< Disables internal register commands generation in PDUMP output. 
#define TAL_DISABLE_CAPTURE_LOOP_CMDS				0x00004000	//!< Disables LOOP commands generation in PDUMP output. 

/*!
 * @brief Maximum time a wait can be when doing a Poll operation
 *
 * Prints a warning if trying to POL that value or longer.
 *
 * @note limit given by HW testbench application - a value greater than that will be considered a lockup
 */
#define TAL_PDUMP_MAX_POL_TIMEOUT 1000000

/*!
******************************************************************************
  This type defines the filename to be set.
******************************************************************************/
typedef enum
{
	TAL_FILENAME_OUT_TXT,			//!<	Set out.txt filename				
	TAL_FILENAME_OUT2_TXT,			//!<	Set out2.txt filename				
	TAL_FILENAME_OUT_RES,			//!<	Set out.res filename				
	TAL_FILENAME_OUT2_RES,			//!<	Set out2.res filename				
	TAL_FILENAME_OUT_PRM,			//!<	Set out.prm filename				
	TAL_FILENAME_OUT2_PRM,			//!<	Set out2.prm filename				
	TAL_FILENAME_OUTL_BIN			//!<	Set outL.bin filename				

} TAL_eSetFilename;

/*!
******************************************************************************

  The memory space types

******************************************************************************/
typedef enum PDUMP_CMD_tag_eMemSpaceType
{
	PDUMP_MEMSPACE_REGISTER,		//!< Memory space is mapped to device registers			
	PDUMP_MEMSPACE_INT_VAR,			//!< Memory space is mapped to internal variables (previously int registers)
	PDUMP_MEMSPACE_SLAVE_PORT,		//!< Memory space is mapped to device slave port (Deprecated)
	PDUMP_MEMSPACE_MEMORY,			//!< Memory space is mapped to "real" memory
	PDUMP_MEMSPACE_MAPPED_MEMORY,	//!< Only used in malloc, indicates that the address is forced
	PDUMP_MEMSPACE_VIRTUAL,			//!< Memory space is mapped to virtual memory
	PDUMP_MEMSPACE_VALUE			//!< Not Memory space, real value

} PDUMP_CMD_eMemSpaceType;


typedef enum PDUMP_CMD_tag_eLoopCommand
{
	PDUMP_LOOP_COMMAND_WDO,
	PDUMP_LOOP_COMMAND_EDO,
	PDUMP_LOOP_COMMAND_SDO,
	PDUMP_LOOP_COMMAND_DOW
} PDUMP_CMD_eLoopCommand;



/*!
******************************************************************************
  PDUMP FLAGS:
  !! This flags must be the same as the PDUMP FLAGS defined in TAL.H !!
******************************************************************************/

typedef enum
{
	TAL_PDUMP_FLAGS_PDUMP1				=		(0x0000001),   // Allows Pdump1 capture to be activated
	TAL_PDUMP_FLAGS_PDUMP2				=		(0x0000002),   // Allows Pdump2 capture to be activated
	TAL_PDUMP_FLAGS_GZIP				=		(0x0000004),   // Allows to use gzip compression
	TAL_PDUMP_FLAGS_RES					=		(0x0000008),   // Allows Res capture to be activated 
	TAL_PDUMP_FLAGS_PRM					=		(0x0000010),   // Allows Prm capture to be activated
	TAL_PDUMP_FLAGS_BINLITE				=		(0x0000020),   // Allows Bin Lite capture to be activated
	TAL_PDUMP_FLAGS_SKIP_PAGEFAULTS	=		(0x0000040),	// Ignores all page faults when pdumping
}TAL_Pdump_Flags;

/*!
******************************************************************************
  32/64 BIT FLAGS:
******************************************************************************/
typedef enum
{
	PDUMP_FLAGS_32BIT	=		(0x0000001),
	PDUMP_FLAGS_64BIT	=		(0x0000002)
} PDUMP_CMD_eWordSize;


typedef struct
{
	IMG_HANDLE				hMemSpace;		//!< The memory space handle.
	IMG_UINT32				ui32BlockId;	//!< The Id of the target Memory Block 
	IMG_UINT64				ui64Value;		//!< The offset or value of the target 
	IMG_UINT64				ui64BlockAddr;	//!< The Physical Block Address for Pdump1 / bin 
	PDUMP_CMD_eMemSpaceType	eMemSpT;		//!< The target type 
}PDUMP_CMD_sTarget, *PDUMP_CMD_psTarget;


typedef struct
{
	PDUMP_CMD_sTarget	stTarget;				//!< The target info 
	IMG_UINT8			*pui8ImageData;			//!< The image data 
	IMG_UINT32			ui32Size;				//!< The total image size
}PDUMP_CMD_sImage;

/*!
******************************************************************************

 @Function				PDUMP_CMD_Initialise

 @Description

 This function is used to initialise the Pdump Commands and should be called at start-up.
 Subsequent calls are ignored.

 @Return	None

******************************************************************************/
IMG_VOID PDUMP_CMD_Initialise (IMG_VOID);

/*!
******************************************************************************

 @Function              PDUMP_CMD_Deinitialise

 @Description

 This function is used to deinitialise the Pdump Commands and should be called at clean-up.
 Subsequent calls are ignored.

 @Return	None

******************************************************************************/
IMG_VOID PDUMP_CMD_Deinitialise (IMG_VOID);

/*!
******************************************************************************

 @Function				PDUMP_CMD_AddMemspaceToPdumpContext
 
  This function allows memoryspaces to be added to contexts dynamically.

 @Input		pszContextName		: The Context Name.

 @Input		hMemSpace			: The Memspace Handle
 
 @return	IMG_HANDLE			: handle to pdump context

******************************************************************************/
IMG_HANDLE PDUMP_CMD_AddMemspaceToPdumpContext(	
	IMG_CHAR *	    pszContextName,
	IMG_HANDLE		hMemSpace
);

/*!
******************************************************************************

 @Function				PDUMP_CMD_CaptureEnablePdumpContext

 @Description

 This function is used to enable PDUMPing for a single context.

 @Input		pszContextName      : The "name" of the PDUMP context.

 @Return    None.

******************************************************************************/
IMG_VOID PDUMP_CMD_CaptureEnablePdumpContext (
	const IMG_CHAR *	    pszContextName
);

/*!
******************************************************************************

 @Function				PDUMP_CMD_CaptureDisablePdumpContext

 @Description

 This function is used to disable PDUMPing for a single context.

 @Input		pszContextName      : The "name" of the PDUMP context.

 @Return    None.

******************************************************************************/
IMG_VOID PDUMP_CMD_CaptureDisablePdumpContext (
	const IMG_CHAR *	    pszContextName
);

/*!
******************************************************************************

 @Function             PDUMP_CMD_MemSpaceRegister

 @Description

 This function sets up a MemSpace.

 @Input		pszMemSpaceName		: Name of the memoryspace.

 @Return	IMG_HANDLE			: Pdump Context Handle

******************************************************************************/
IMG_HANDLE PDUMP_CMD_MemSpaceRegister (
    IMG_CHAR *  pszMemSpaceName
);

/*!
******************************************************************************

 @Function              PDUMP_CMD_CaptureMemSpaceEnable

 @Description

 This function starts pdump capture.

 @Input		hMemSpace		: The Memory Space Handle.

 @Input		bEnable			: Enable or Disable.
 
 @Output	pbPrevState		: The previous enable state

 @Return	None

******************************************************************************/
IMG_VOID PDUMP_CMD_CaptureMemSpaceEnable (
	IMG_HANDLE                      hMemSpace,
    IMG_BOOL                        bEnable,
    IMG_BOOL *                      pbPrevState
);

/*!
******************************************************************************

 @Function              PDUMP_CMD_CaptureStart

 @Description

 This function starts pdump capture.

 @Input		psTestDirectory	: The export directory.

 @Return	None

******************************************************************************/
IMG_VOID PDUMP_CMD_CaptureStart (
	IMG_CHAR * psTestDirectory
);
/*!
******************************************************************************

 @Function              PDUMP_CMD_CaptureStop

 @Description

 This function stops all capture.

 @Return	None


******************************************************************************/
IMG_VOID PDUMP_CMD_CaptureStop (IMG_VOID);

/*!
******************************************************************************

 @Function              PDUMP_CMD_IsCaptureEnabled

 @Description

 This function checks to see if a capture is enabled

 @Return	Enabled.

******************************************************************************/
IMG_BOOL PDUMP_CMD_IsCaptureEnabled (IMG_VOID);

/*!
******************************************************************************

 @Function				PDUMP_CMD_CaptureGetFilename

 @Description

 This function is used to get one of the filenames being used.

 @Input		hMemSpace 		: The memory space handle.

 @Input		eSetFilename	: Defines the filename to get.

 @Return	IMG_CHAR *		: Pointer to the filename.

******************************************************************************/
IMG_CHAR * PDUMP_CMD_CaptureGetFilename (
    IMG_HANDLE                      hMemSpace,
	TAL_eSetFilename		        eSetFilename
);


/*!
******************************************************************************

 @Function				PDUMP_CMD_CaptureDisableCmds

 @Description

 This function is used to disable the capture of specified PDUMP commands
 for globally, by memory space or for a specific register.

 Disabling the capture of commands should be done before starting the capture.

 @Input		hMemSpace	: The memory space Handle

 @Input		ui32Offset	: The offset, in bytes, of the register to be disable
                            of #TAL_OFFSET_ALL.

 @Input		ui32DisableFlags : Capture disable flags (see TAL_DISABLE_CAPTURE_xxx).

 @Return	None.

******************************************************************************/
IMG_VOID PDUMP_CMD_CaptureDisableCmds(
	IMG_HANDLE						hMemSpace,
	IMG_UINT32						ui32Offset,
	IMG_UINT32						ui32DisableFlags
);


/*!
******************************************************************************

 @Function              PDUMP_CMD_CaptureStartPdumpLite

 @Description

 This function starts pdump capture in binary lite format.

 @Input		pszTestDirectory	: The export directory.

 @Return	None

******************************************************************************/
IMG_VOID PDUMP_CMD_CaptureStartPdumpLite(IMG_CHAR * pszTestDirectory);

/*!
******************************************************************************

 @Function				PDUMP_CMD_GenerateTDF
 
 @Description

 This function creates the test descriptor file.

 @Input		psFilename		: The TDF filename

 @Return	None


******************************************************************************/
IMG_VOID PDUMP_CMD_GenerateTDF (IMG_CHAR * psFilename);


/*!
******************************************************************************

 @Function				PDUMP_CMD_AddSyncToTDF

 @Description

 This function allows syncs to be defined in the test descriptor file.

 @Input		ui32SyncId		: The Sync Id to be added

 @Input		hMemSpace	: The Memspace of the sync

 @Return	None


******************************************************************************/
IMG_VOID PDUMP_CMD_AddSyncToTDF (IMG_UINT32 ui32SyncId, IMG_HANDLE hMemSpace);

/*!
******************************************************************************

 @Function				PDUMP_CMD_AddCommentToTDF
 
 @Description

 This function allows comments to be added to the test descriptor file.

 @Input		pcComment		: The comment text

 @Return	None

******************************************************************************/
IMG_VOID PDUMP_CMD_AddCommentToTDF (IMG_CHAR * pcComment);

/*!
******************************************************************************

 @Function				PDUMP_CMD_AddTargetConfigToTDF
 
 @Description

 This function allows a target config file to be added to the test descriptor file.

 @Input		pszFilePath		: The path to the config file

 @Return	None

******************************************************************************/
IMG_VOID PDUMP_CMD_AddTargetConfigToTDF (IMG_CHAR * pszFilePath);

/*!
******************************************************************************

 @Function				PDUMP_CMD_SetPdumpConversionSourceData

 @Description

 The function is to be used to set up source file related parameters when TAL is
 used in conversion of PDUMP scripts between formats. Currently, the information is
 used by the TAL in generating debug parameters in Pdump Lite POL-like commands.
 The function should be called as often as needed to ensure that the parameters
 in question are up to date in the resulting converted script.

 @Input		ui32SourceFileID	: The ID number of the source file being converted.

 @Input		ui32SourceLineNum	: Current source file line number.

 @Return	None.

******************************************************************************/
IMG_VOID PDUMP_CMD_SetPdumpConversionSourceData(
    IMG_UINT32				ui32SourceFileID,
    IMG_UINT32				ui32SourceLineNum
);


/*!
******************************************************************************

 @Function              PDUMP_CMD_ChangePdumpContext

 @Description
 
 This function changes the pdump context of a memory space

 @Input		hMemSpace		: the Memspace to change context.

 @Input		pszContextName		: Name of the new context


 @Return	None.

******************************************************************************/
IMG_VOID PDUMP_CMD_ChangePdumpContext (
    IMG_HANDLE                      hMemSpace,
    IMG_CHAR *                      pszContextName
);

/*!
******************************************************************************

 @Function              PDUMP_CMD_CaptureSetFilename

 @Description
 
 This function sets the filename of the output files

 @Input		hMemSpace	: the memory space.

 @Input		eSetFilename	: The File to change.

 @Input		psFileName		: The Name to change it too.

 @Return	None.

******************************************************************************/
IMG_VOID PDUMP_CMD_CaptureSetFilename (
    IMG_HANDLE                      hMemSpace,
	TAL_eSetFilename		        eSetFilename,
	IMG_CHAR *				        psFileName
);

/*!
******************************************************************************

 @Function             PDUMP_CMD_CaptureSetFileoffset

 @Description
 
 This function sets the file offset of the data files

 @Input		hMemSpace	: The memory space.

 @Input		eSetFilename	: The File to change.

 @Input		ui32FileOffset	: The Offset to change it too.

 @Return	None.

******************************************************************************/
IMG_VOID PDUMP_CMD_CaptureSetFileoffset (
    IMG_HANDLE                      hMemSpace,
	TAL_eSetFilename		        eSetFilename,
	IMG_UINT32				        ui32FileOffset
);

/*!
******************************************************************************

 @Function				PDUMP_CMD_SetBaseAddress

 @Description
 
 This function sets the base address of a memory space for use by Binary Lite
 and Pdump1 Commands

 @Input		hMemSpace		: The memory space handle.

 @Input		ui64BaseAddr	: The base address of the memory space.

 @Return	None.

******************************************************************************/
IMG_VOID PDUMP_CMD_SetBaseAddress (
	IMG_HANDLE					hMemSpace,
	IMG_UINT64                  ui64BaseAddr
);

/*!
******************************************************************************

 @Function				PDUMP_CMD_SetPdump1MemName

 @Description
 
 This function sets a base name for the pdump1 memory bus, if set to null
 it will use the pdump2 memory space names and offsets from those names.

 @Input		pszMemName	: The pdump1 memory name.

 @Return	None.

******************************************************************************/
IMG_VOID PDUMP_CMD_SetPdump1MemName (
	IMG_CHAR*					pszMemName
);

/*!
******************************************************************************

 @Function				PDUMP_CMD_ClearPdumpFlags

 @Description

 This function clears the given PDUMP flags

 @Input		ui32Flags		: PDUMP bitwise flags

 @Return	None

******************************************************************************/

IMG_VOID PDUMP_CMD_ClearFlags(IMG_UINT32 ui32Flags);

/*!
******************************************************************************

 @Function				PDUMP_SetPdumpFlags

 @Description

 This function adds the given PDUMP flags

 @Input		ui32Flags		: PDUMP bitwise flags

 @Return	None.

******************************************************************************/

IMG_VOID PDUMP_CMD_SetFlags(IMG_UINT32 ui32Flags);

#define PDUMP_SetFlags PDUMP_CMD_SetFlags

/*!
******************************************************************************

 @Function				PDUMP_GetPdumpFlags

 @Description

 This function returns the current PDUMP flags

 @Return	IMG_UINT32	: Current PDUMP bitwise flags

******************************************************************************/

IMG_UINT32 PDUMP_CMD_GetFlags(IMG_VOID);

#define PDUMP_GetFlags PDUMP_CMD_GetFlags

/*!
******************************************************************************

 @Function      PDUMP_CMD_CaptureEnableOutputFormats

 @Description
 
 This function allows Pdump1 and Pdump 2 Capture to be activated / deactivated.

 @Input		bEnablePDUMP1	: Activate Pdump1 capture

 @Input		bEnablePDUMP2	: Activate Pdump2 capture

 @Return	None.

******************************************************************************/
IMG_VOID PDUMP_CMD_CaptureEnableOutputFormats (
    IMG_BOOL            bEnablePDUMP1,
    IMG_BOOL            bEnablePDUMP2
);

/*!
******************************************************************************

 @Function              PDUMP_CMD_CaptureEnableResAndPrm

 @Description
 
 This function allows the Res and Prm Capture to be activated / deactivated.

 @Input		bRes	: Activate Res file capture?

 @Input		bPrm	: Activate Prm file capture?

 @Return	None.

******************************************************************************/
IMG_VOID PDUMP_CMD_CaptureEnableResAndPrm (
	IMG_BOOL			bRes,
	IMG_BOOL			bPrm
);

/*!
******************************************************************************

 @Function              PDUMP_CMD_PdumpComment

 @Description
 
 This function adds a comment to a Pdump Script

 @Input		hMemSpace	: Memory Space defines context
								into which comment should be placed.

 @Input		psCommentText	: Text of Comment.

 @Return	None.

******************************************************************************/
IMG_VOID PDUMP_CMD_PdumpComment(
    IMG_HANDLE                                hMemSpace,
    IMG_CONST IMG_CHAR *                      psCommentText
);

/*!
******************************************************************************

 @Function              PDUMP_CMD_PdumpConsoleMessage

 @Description
 
 This function adds a comment to a Pdump Console

 @Input		hMemSpace	: Memory Space defines context
								into which comment should be placed.

 @Input		psCommentText	: Text of Comment.

 @Return	None.

******************************************************************************/
IMG_VOID PDUMP_CMD_PdumpConsoleMessage(
    IMG_HANDLE                      hMemSpace,
    IMG_CHAR *                      psCommentText
);

/*!
******************************************************************************

 @Function              PDUMP_CMD_PdumpMiscOutput

 @Description
 
 This function adds a given line to a Pdump Script

 @Input		hMemSpace	: Memory Space defines context
								into which comment should be placed.

 @Input		psCommentText	: Text of Comment.

 @Return	None.

******************************************************************************/
IMG_VOID PDUMP_CMD_PdumpMiscOutput(
    IMG_HANDLE                      hMemSpace,
    IMG_CHAR *                      psCommentText
);

/*!
******************************************************************************

 @Function              PDUMP_CMD_Sync

 @Description

 This function is used to place an Sync Command into a pdump file

 @Input		hMemSpace	: Memory Space defines context
								into which sync should be placed.

 @Input		ui32SyncId		: The Sync Id

 @Return	None.

******************************************************************************/
IMG_VOID PDUMP_CMD_Sync(
	IMG_HANDLE	hMemSpace,
	IMG_UINT32	ui32SyncId
);

/*!
******************************************************************************

 @Function              PDUMP_CMD_Unlock

 @Description

 This function is used to place an Unlock Command into a pdump file

 @Input		hMemSpace	: Memory Space defines context
								into which unlock should be placed.

 @Input		ui32SemaId		: The Semaphore Id

 @Return	None.

******************************************************************************/
IMG_VOID PDUMP_CMD_Unlock(
	IMG_HANDLE	hMemSpace,
	IMG_UINT32	ui32SemaId
);

/*!
******************************************************************************

 @Function              PDUMP_CMD_Lock

 @Description

 This function is used to place an Lock Command into a pdump file

 @Input		hMemSpace	: Memory Space defines context
								into which lock should be placed.


 @Input		ui32SemaId		: The Semaphore Id

 @Return	None.

******************************************************************************/
IMG_VOID PDUMP_CMD_Lock(
	IMG_HANDLE	hMemSpace,
	IMG_UINT32	ui32SemaId
);

/*!
******************************************************************************

 @Function              PDUMP_CMD_If

 @Description

 This function is used to place an If command into a pdump file

 @Input		hMemSpace	: Memory Space defines context
								into which if should be placed.

 @Input		pszDefine		: The if parameter

 @Return	None.

******************************************************************************/
IMG_VOID PDUMP_CMD_If(
	IMG_HANDLE	hMemSpace,
	IMG_CHAR *	pszDefine
);

/*!
******************************************************************************

 @Function              PDUMP_CMD_Else

 @Description

 This function is used to place an Else command into a pdump file

 @Input		hMemSpace	: Memory Space defines context
								into which else should be placed.

 @Return	None.

******************************************************************************/
IMG_VOID PDUMP_CMD_Else(
	IMG_HANDLE	hMemSpace
);

/*!
******************************************************************************

 @Function              PDUMP_CMD_Endif

 @Description

 This function is used to place an end if command into a pdump file

 @Input		hMemSpace	: Memory Space defines context
								into which endif should be placed.


 @Return	None.

******************************************************************************/
IMG_VOID PDUMP_CMD_Endif(
	IMG_HANDLE	hMemSpace
);



/*!
******************************************************************************

 @Function              PDUMP_CMD_Idle

 @Description

 This function is used to place an Idle Command into a pdump file

 @Input		stTarget		: The Target Struct with Idle Info

 @Return	None.

******************************************************************************/
IMG_VOID PDUMP_CMD_Idle(
	PDUMP_CMD_sTarget		stTarget
);

/*!
******************************************************************************

 @Function              PDUMP_CMD_ReadWord32/64

 @Description

 This function is used to place a read word command into a pdump file

 @Input		stSrc			: The Target Struct with Source Info

 @Input		stDest			: The Target Struct with Destination Info

 @Input		bVerify			: Defines if the read is to be verified

 @Return	None.

******************************************************************************/
IMG_VOID PDUMP_CMD_ReadWord32(
	PDUMP_CMD_sTarget		stSrc,
	PDUMP_CMD_sTarget		stDest,
	IMG_BOOL				bVerify
);

#define PDUMP_CMD_ReadWord PDUMP_CMD_ReadWord32

IMG_VOID PDUMP_CMD_ReadWord64(
	PDUMP_CMD_sTarget		stSrc,
	PDUMP_CMD_sTarget		stDest,
	IMG_BOOL				bVerify
);

/*!
******************************************************************************

 @Function              PDUMP_CMD_WriteWord32/64

 @Description

 This function is used to place a write word command into a pdump file

 @Input		stSrc			: The Target Struct with Source Info

 @Input		stDest			: The Target Struct with Destination Info

 @Input		pszManglerFunc	: The memory mangler function name if valid

 @Input		ui32BitMask		: The bit mask, 0 if invalid.

 @Input		ui64MangledVal	: The Value (mangled and masked if req.) to be written

 @Return	None.

******************************************************************************/
IMG_VOID PDUMP_CMD_WriteWord32(
	PDUMP_CMD_sTarget		stSrc,
	PDUMP_CMD_sTarget		stDest,
	IMG_CHAR *				pszManglerFunc,
	IMG_UINT32				ui32BitMask,
	IMG_UINT64				ui64MangledVal
);


IMG_VOID PDUMP_CMD_WriteWord64(
	PDUMP_CMD_sTarget		stSrc,
	PDUMP_CMD_sTarget		stDest,
	IMG_CHAR *				pszManglerFunc,
	IMG_UINT32				ui32BitMask,
	IMG_UINT64				ui64MangledVal
);

/*!
******************************************************************************

 @Function              PDUMP_CMD_LoadWords

 @Description

 This function is used to place an LDW command into a pdump file
 as well as appropriate data into a PRM file

 @Input		stDest			: The Target Struct with Destination Info

 @Input		pui32Values		: An array of the values to write.

 @Input		ui32WordCount	: The number of words to load.

 @Return	None.

******************************************************************************/
IMG_VOID PDUMP_CMD_LoadWords(
	PDUMP_CMD_sTarget	stDest,
	IMG_PUINT32			pui32Values,
	IMG_UINT32			ui32WordCount
);

/*!
******************************************************************************

 @Function              PDUMP_CMD_SaveWords

 @Description

 This function is used to place an SAW command into a pdump file
 as well as appropriate data into a RES file

 @Input		stSrc			: The Target Struct with Source Info

 @Input		pui32Values		: An array of the values to write.

 @Input		ui32WordCount	: The number of Words to save.

 @Return	None.          

******************************************************************************/
IMG_VOID PDUMP_CMD_SaveWords(
	PDUMP_CMD_sTarget	stSrc,
	IMG_PUINT32			pui32Values,
	IMG_UINT32			ui32WordCount
);

/*!
******************************************************************************

 @Function              PDUMP_CMD_LoadBytes

 @Description

 This function is used to place an LDB command into a pdump file
 as well as appropriate data into a PRM file

 @Input		stDest			: The Target Struct with Destination Info

 @Input		pui32Values		: An array of the values to write.
 
 @Input		ui32Size		: The number of bytes to load.

 @Return	None.

******************************************************************************/
IMG_VOID PDUMP_CMD_LoadBytes(
	PDUMP_CMD_sTarget				stDest,
	IMG_PUINT32						pui32Values,
	IMG_UINT32						ui32Size
);

/*!
******************************************************************************

 @Function              PDUMP_CMD_SaveBytes

 @Description

 This function is used to place an SAB command into a pdump file
 as well as appropriate data into a RES file

 @Input		stSrc			: The Target Struct with Source Info

 @Input		pui8Values		: An array of the values to write.

 @Input		ui32Size		: The number of bytes to save.

 @Return	None.          

******************************************************************************/
IMG_VOID PDUMP_CMD_SaveBytes(
	PDUMP_CMD_sTarget				stSrc,
	IMG_PUINT8						pui8Values,
	IMG_UINT32						ui32Size
);


/*!
******************************************************************************

 @Function              PDUMP_CMD_CircBufPoll32

 @Description

 This function is used to check for space in a circular buffer (CBP)

 @Input		stSrc			: The Target Struct with Source Info

 @Input		ui32WoffValue	: This is the write offset value.

 @Input		ui32PacketSize	: This is the required packet size.

 @Input		ui32BufferSize	: This is the size of the circular buffer.

 @Input		ui32PollCount	: The number of polling loops to perform.

 @Input		ui32TimeOut		: This is the delay between loops.

 @Return	None.          

******************************************************************************/
IMG_VOID PDUMP_CMD_CircBufPoll32(
	PDUMP_CMD_sTarget	stSrc,
	IMG_UINT32			ui32WoffValue,
	IMG_UINT32			ui32PacketSize,
	IMG_UINT32			ui32BufferSize,
	IMG_UINT32			ui32PollCount,
	IMG_UINT32			ui32TimeOut
);
#define PDUMP_CMD_CircBufPoll PDUMP_CMD_CircBufPoll32

/*!
******************************************************************************

 @Function              PDUMP_CMD_CircBufPoll64

 @Description

 This function is a 64bits version of PDUMP_CMD_CircBufPoll32          

******************************************************************************/
IMG_VOID PDUMP_CMD_CircBufPoll64(
	PDUMP_CMD_sTarget	stSrc,
	IMG_UINT64			ui64WoffValue,
	IMG_UINT64			ui64PacketSize,
	IMG_UINT64			ui64BufferSize,
	IMG_UINT32			ui32PollCount,
	IMG_UINT32			ui32TimeOut
);

/*!
******************************************************************************

 @Function              PDUMP_CMD_Poll32

 @Description

 This function is used to check for a given value (POL)

 @Input		stSrc			: The Target Struct with Source Info
  
 @Input		ui32ReqVal		: Required Value of the Poll

 @Input		ui32Enable		: A mask of the bits to be tested

 @Input		ui32PollCount	: The number of Polling loops to perform,
								0 = infinite.

 @Input		ui32Delay		: This is the delay between polling loops

 @Input		ui32Opid		: Id of the comparison function to perform.

 @Input		pszCheckFunc	: function to perform non-standard comparison.


 @Return	None.          

******************************************************************************/
IMG_VOID PDUMP_CMD_Poll32(
	PDUMP_CMD_sTarget	stSrc,
	IMG_UINT32			ui32ReqVal,
	IMG_UINT32			ui32Enable,
	IMG_UINT32			ui32PollCount,
	IMG_UINT32			ui32Delay,
	IMG_UINT32			ui32Opid,
	IMG_CHAR *			pszCheckFunc
);

#define PDUMP_CMD_Poll PDUMP_CMD_Poll32

/*!
******************************************************************************

 @Function              PDUMP_CMD_Poll64

 @Description

 This function is the 64bits version of PDUMP_CMD_Poll32      

******************************************************************************/
IMG_VOID PDUMP_CMD_Poll64(
	PDUMP_CMD_sTarget	stSrc,
	IMG_UINT64			ui64ReqVal,
	IMG_UINT64			ui64Enable,
	IMG_UINT32			ui32PollCount,
	IMG_UINT32			ui32Delay,
	IMG_UINT32			ui32Opid,
	IMG_CHAR *			pszCheckFunc
);

/*!
******************************************************************************

 @Function              PDUMP_CMD_LoopCommand

 @Description

 This function is used to place an WDO, EDO, SDO or DOW command into 
 a pdump file as well as appropriate data into a RES file

 @Input		stSrc				: The Target Struct with Source Info
  
 @Input		ui32CheckFuncIdExt	: The compare operation id

 @Input		ui32RequValue		: The required value

 @Input		ui32Enable			: Is an enable mask AND�s with the value 
								  read before any check o comparison is made

 @Input		ui32LoopCount		: Is the number of times to poll for a match

 @Input		eLoopCommand		: Id of the command (WDO, EDO, SDO or DOW).


 @Return	None. 

******************************************************************************/
IMG_VOID PDUMP_CMD_LoopCommand(
	PDUMP_CMD_sTarget		stSrc,
	IMG_UINT32				ui32CheckFuncIdExt,
	IMG_UINT32    			ui32RequValue,
	IMG_UINT32    			ui32Enable,
	IMG_UINT32				ui32LoopCount,
	PDUMP_CMD_eLoopCommand	eLoopCommand
);

/*!
******************************************************************************

 @Function              PDUMP_CMD_Malloc

 @Description

 This function is used to check for a given value (POL)

 @Input		stTarget		: The Target Struct with Source Info
  
 @Input		ui32Size		: Required Memory Block Size

 @Input		ui32Alignment	: Byte Alignment of Memory

 @Input		pszBlockName	: The name of the block


 @Return	None.          

******************************************************************************/
IMG_VOID PDUMP_CMD_Malloc(
	PDUMP_CMD_sTarget	stTarget,
	IMG_UINT32			ui32Size,
	IMG_UINT32			ui32Alignment,
	IMG_CHAR *			pszBlockName
);

/*!
******************************************************************************

 @Function              PDUMP_CMD_Free

 @Description

 This function is used to check for a given value (POL)

 @Input		stTarget		: The Target Struct with Mem Block Info

 @Return	None.          

******************************************************************************/
IMG_VOID PDUMP_CMD_Free(
	PDUMP_CMD_sTarget	stTarget
);

/*!
******************************************************************************

 @Function              PDUMP_CMD_DumpImage

 @Description

 This function is used to dump image data (SII)

 @Input		pszImageSet		: Prefix used for image files
								generated from these tests.

 @Input		siImg1			: Information about the 1st image.

 @Input		siImg2			: Information about the 2nd image.

 @Input		siImg3			: Information about the 3rd image.

 @Input		ui32PixFmt		: Predefined pixel format.

 @Input		ui32Width		: Width of the image in pixels.

 @Input		ui32Height		: Height of the image in lines.

 @Input		ui32Stride		: Image Stride in bytes.

 @Input		ui32AddrMode	: Address Mode.

 @Return	None.          

******************************************************************************/
IMG_VOID PDUMP_CMD_DumpImage(
	IMG_CHAR*			pszImageSet,
	PDUMP_CMD_sImage	siImg1,
	PDUMP_CMD_sImage	siImg2,
	PDUMP_CMD_sImage	siImg3,
	IMG_UINT32			ui32PixFmt,
	IMG_UINT32			ui32Width,
	IMG_UINT32			ui32Height,
	IMG_UINT32			ui32Stride,
	IMG_UINT32			ui32AddrMode
);

/*!
******************************************************************************

 @Function              PDUMP_CMD_CheckDumpImage

 @Description

 This function is used to check the res file for space for image data (SII)

 @Input		hMemSpace		: The memspace handle

 @Input		ui32Size		: Space needed in file.

 @Return	None.          

******************************************************************************/
IMG_VOID PDUMP_CMD_CheckDumpImage(
	IMG_HANDLE			hMemSpace,
	IMG_UINT32			ui32Size
);

/*!
******************************************************************************

 @Function              PDUMP_CMD_RegisterCommand

 @Description

 This function is used to check the res file for space for image data (SII)

 @Input		ui32CommandId	: BinLite Command Id of Register Action

 @Input		stDest			: Destination Register

 @Input		stOpreg1		: First Operation Register

 @Input		stOpreg2		: Second Operation Register

 @Return	None.          

******************************************************************************/
IMG_VOID PDUMP_CMD_RegisterCommand(
	IMG_UINT32			ui32CommandId,
	PDUMP_CMD_sTarget	stDest,
	PDUMP_CMD_sTarget	stOpreg1,
	PDUMP_CMD_sTarget	stOpreg2
);

/*!
******************************************************************************

 @Function              PDUMP_CMD_TiledRegion

 @Description

 This function is used to set up a tiled region in memory (MMU function)

 @Input		stTarget		: The Target Info

 @Input		ui32RegionId	: Region Id Number.

 @Input		ui64Size		: Size of Region.

 @Input		ui32Stride		: Tile Stride.

 @Return	None.          

******************************************************************************/
IMG_VOID PDUMP_CMD_TiledRegion(
	PDUMP_CMD_sTarget	stTarget,
	IMG_UINT32			ui32RegionId,
	IMG_UINT64			ui64Size,
	IMG_UINT32			ui32Stride
);

/*!
******************************************************************************

 @Function              PDUMP_CMD_SetMMUContext

 @Description

 This function is used to set up a tiled region in memory (MMU function)

 @Input		stTarget		: The Target Info

 @Input		stPageTable		: Page Table info

 @Input		ui32MMUType		: MMU Type (0 = clear).


 @Return	None.

******************************************************************************/
IMG_VOID PDUMP_CMD_SetMMUContext(
	PDUMP_CMD_sTarget	stTarget,
	PDUMP_CMD_sTarget	stPageTable,
	IMG_UINT32			ui32MMUType
);



/*!
******************************************************************************

 @Function              PDUMP_CMD_PdumpEnabled

 @Description

    Returns true if pdump 1 or 2 is enabled.
    

 @Return	IMG_BOOL		: true if pdumping

******************************************************************************/
IMG_BOOL PDUMP_CMD_PdumpEnabled (IMG_VOID);



#if !defined (DOXYGEN_WILL_SEE_THIS)

IMG_VOID PDUMP_CMD_SetResPrmFileName(
	IMG_HANDLE	hMemSpace,
	IMG_BOOL bNotResFile,
	IMG_CHAR* pszFileName,
	IMG_CHAR* pszOut2Prefix
);

IMG_VOID PDUMP_CMD_ResetResPrmFileName(
	IMG_HANDLE	hMemSpace,
	IMG_BOOL bNotResFile
);

#endif


#if defined (__cplusplus)
}
#endif
 
#endif /* __PDUMP_CMDS_H__	*/

