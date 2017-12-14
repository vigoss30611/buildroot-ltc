/*! 
******************************************************************************
 @file   : pdump_cmds.c

 @brief  

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
         PDUMP_CMDS C Source File.

 <b>Platform:</b>\n
	     Platform Independent 

 @Version	
	     1.0 

******************************************************************************/

#include "pdump_cmds.h"
//#include "pdump.h"
#include "mmu.h"
#include <string.h>
#include <stdio.h>

#include <gzip_fileio.h>
#include <tal.h> // tal_defs.h

#include <img_types.h>
#include <img_defs.h>
#include <img_errors.h>

#define PDUMPCMDS_HASHSIZE 0x10

/*
 define lock access function for MeOS, OSA or no thread safe access to pdump:
- pdumpAccessLock: lock the file
- pdumpAccessUnlock: unlock the file
- pdumpAccessInit: create the lock
- pdumpAccessDeinit: destroy the lock
 */
#if defined(__TAL_USE_OSA__)

#include <osa.h>
IMG_HANDLE hFileMutex;  

static IMG_INLINE void pdumpAccessLock()
{
    OSA_CritSectLock(hFileMutex);
}

static IMG_INLINE void pdumpAccessUnlock()
{
    OSA_CritSectUnlock(hFileMutex);
}

static IMG_INLINE void pdumpAccessInit()
{
    OSA_CritSectCreate(&hFileMutex);
}

static IMG_INLINE void pdumpAccessDeinit()
{
    OSA_CritSectDestroy(hFileMutex);
}		

#else

static IMG_INLINE void pdumpAccessLock(){ }	
static IMG_INLINE void pdumpAccessUnlock(){	}	
static IMG_INLINE void pdumpAccessInit(){ }	
static IMG_INLINE void pdumpAccessDeinit(){ }	

#endif

#define DEFAULT_TDF_FILE "auto.tdf"

/*<! Max. Length of TDF comment */
#define MAX_TDF_COMMENT	(1024)
#define DIRECTORY_SEPARATOR         "/"
#define DIRECTORY_SEPARATOR_IMG_CHAR    '/'

/*<! Max. number of custom .prm or .res file names */
#define MAX_CUSTOM_RES_PRM_NAME_NUM	(32)

#ifndef PDMP_CMDS_MSX_DISABLED_OFFSETS
#define PDMP_CMDS_MSX_DISABLED_OFFSETS      (10)            /*!< Maximum number fo disabled offsets...*/
#endif

#define GET_PDUMP_FLAG(uiFlagBit) (gFlags & uiFlagBit) //Gets the value of a given PDUMP flag

// Gets the value of the specific TAL_PDUMP_FLAGS_GZIP flags, which determine if gzip compression is enabled
//#define GET_ENABLE_COMPRESSION ((gFlags & TAL_PDUMP_FLAGS_GZIP) != 0 ? PDUMP_OPEN_WRITE_COMPRESSED : PDUMP_OPEN_WRITE)
#define GET_ENABLE_COMPRESSION ((gFlags & TAL_PDUMP_FLAGS_GZIP) != 0 ? IMG_TRUE : IMG_FALSE)

static const IMG_CHAR* gpszDefaultMemName = "MEM";

#define MAX_CONFIG_LENGTH (128)
static IMG_CHAR acConfigFile[MAX_CONFIG_LENGTH];

/*!
******************************************************************************
  This structure contains the information regarding the handling of a custom
  .res or .prm file.  
******************************************************************************/
typedef struct
{
	IMG_UINT32	ui32CurrentNameId;							/*!< The index of the current file	being used			*/
	IMG_UINT32	ui32NameNum;								/*!< Number of custom file names currently being used	*/
	IMG_CHAR* 	apszNames[MAX_CUSTOM_RES_PRM_NAME_NUM];		/*!< Custom file names currently being used				*/
	IMG_PVOID*	hFiles[MAX_CUSTOM_RES_PRM_NAME_NUM];		/*!< Custom file handles currently being used			*/
	IMG_UINT32	ui32FileSizes[MAX_CUSTOM_RES_PRM_NAME_NUM];	/*!< Size of files										*/
} PDUMP_CMDS_sResPrmFileInfo;



/******************************************************************************
  This structure contains a node of the hash table of
  mem space names, used in PDUMP_CMDS_sPdumpContext struct.
******************************************************************************/

typedef struct PDUMP_CMDS_sMemSpaceNameNode_tag
{
	IMG_CHAR *	    pszMemSpaceName;
	struct PDUMP_CMDS_sMemSpaceNameNode_tag * psNext;

}PDUMP_CMDS_sMemSpaceNameNode;

/*!
******************************************************************************
  This structure contains the pdump2 context.
******************************************************************************/
typedef struct
{
    IMG_CHAR *      pszContextName;         /*!< Context name                       */
    IMG_BOOL		bPdumpEnabled;          /*!< IMG_TRUE if PDUMPing is enabled for this context   */
    IMG_HANDLE      hOut2TxtFile;           /*!< PDUMP2 out2.txt file handle        */
    IMG_HANDLE      hOut2PrmFile;           /*!< PDUMP2 out2.prm file handle        */
    IMG_HANDLE      hOut2ResFile;           /*!< PDUMP2 out2.prm file handle        */
    IMG_HANDLE      hOutLiteBinFile;        /*!< Binary Lite outL.bin file name     */
    IMG_UINT32      ui32Out2PrmFileSize;    /*!< Size of out2.prm file              */
    IMG_UINT32      ui32Out2ResFileSize;    /*!< Size of out2.res file              */
    IMG_UINT32      ui32OutLiteBinFiletSizeInDWords; /*!< Size of outL.bin file     */
    IMG_CHAR *      psOut2TxtFileName;      /*!< PDUMP2 out2.txt file name          */
    IMG_CHAR *      psOut2PrmFileName;      /*!< PDUMP2 out2.prm file name          */
    IMG_CHAR *      psOut2ResFileName;      /*!< PDUMP2 out2.prm file name          */
    IMG_CHAR *      psOutLiteBinFileName;   /*!< Binary Lite outL.bin file name     */
	IMG_UINT32		ui32ActiveSyncs;		/*!< represents 32 booleans showing active syncs */

	PDUMP_CMDS_sMemSpaceNameNode * apsMemSpaceNameNodes[TAL_MAX_MEMSPACES]; /*!< Memory space names      */
	PDUMP_CMDS_sResPrmFileInfo asCustomResFiles;	/*!< Structure used to define custom filenames for .res files	*/
	PDUMP_CMDS_sResPrmFileInfo asCustomPrmFiles;	/*!< Structure used to define custom filenames for .prm files	*/

}PDUMP_CMDS_sPdumpContext, * PDUMP_CMDS_psPdumpContext;


/*!
******************************************************************************
  This structure contains the memory block information.
******************************************************************************/
typedef struct PDUMP_CMDS_sMemBlock_tag
{
	struct PDUMP_CMDS_sMemBlock_tag *	psNextMemBlock;	/*!< Next Memory Block in List			*/
    IMG_UINT32					ui32MemBlockId;			/*!< Id of the Memory Block             */
	IMG_UINT32					ui32Size;				/*!< Size of the Memory Block			*/
	IMG_UINT64					ui64DevAddr;			/*!< The device address of the Block	*/
    IMG_CHAR *					pszBlockName;			/*!< PDUMP1 out.txt file name           */
}PDUMP_CMDS_sMemBlock, *PDUMP_CMDS_psMemBlock;

/*!
******************************************************************************
  This structure contains the memory space information.
******************************************************************************/
typedef struct
{
	IMG_CHAR *					pszMemSpaceName;		/*!< The Name of the MemorySpace			*/
	PDUMP_CMDS_psPdumpContext	psPdumpContext;			/*!< Pointer to associated Pdump Context	*/
	IMG_UINT64					ui64BaseAddress;		/*!< Base Address of the Memory Space */
	IMG_BOOL					bCaptureEnabled;		/*!< IMG_TRUE if capture enabled for this memory space	*/
    IMG_UINT32                  ui32DisableFlags;       /*!< Disable flags for this memory space...*/
    IMG_UINT32                  ui32NoDisabledOffsets;  /*!< Number of disabled offsets                                         */
    IMG_UINT32                  aui32DisableOffset[PDMP_CMDS_MSX_DISABLED_OFFSETS]; /*!< Array of disabled offsets              */
    IMG_UINT32                  aui32DisableFlags[PDMP_CMDS_MSX_DISABLED_OFFSETS];  /*!< Array of corresponding disable flags   */
	IMG_BOOL					bPdumpSoftReg[TAL_NUMBER_OF_INTERNAL_VARS];	/*!< Flag to indicate whether Pdump1 and BinLite should pdump internal reg commands */
	PDUMP_CMDS_psMemBlock		apsMemBlock[PDUMPCMDS_HASHSIZE];		/*!< Array for memory block ids of this specific memory space		*/

}PDUMP_CMDS_sMemSpaceCB, *PDUMP_CMDS_psMemSpaceCB;

/*!
******************************************************************************
  This structure contains the pdump1 context.
******************************************************************************/
typedef struct
{
    IMG_HANDLE      hOutTxtFile;            /*!< PDUMP1 out.txt file handle				*/
    IMG_HANDLE      hOutPrmFile;            /*!< PDUMP1 out.prm file handle			    */
    IMG_HANDLE      hOutResFile;            /*!< PDUMP1 out.prm file handle			    */
    IMG_UINT32      ui32OutPrmFileSize;     /*!< Size of out.prm file				    */
    IMG_UINT32      ui32OutResFileSize;     /*!< Size of out.res file				    */
    IMG_CHAR *      psOutTxtFileName;       /*!< PDUMP1 out.txt file name			    */
    IMG_CHAR *      psOutPrmFileName;       /*!< PDUMP1 out.prm file name				*/
    IMG_CHAR *      psOutResFileName;       /*!< PDUMP1 out.res file name			    */
	IMG_CHAR *		pszMemName;				/*!< Name of the Pdump1 Memory base name	*/
	
	PDUMP_CMDS_psMemSpaceCB	psTempMemspace;		/*!< Storage of current memspace for virtual mem decode */
	PDUMP_CMDS_sResPrmFileInfo asCustomResFiles;	/*!< Structure used to define custom filenames for .res files	*/
	PDUMP_CMDS_sResPrmFileInfo asCustomPrmFiles;	/*!< Structure used to define custom filenames for .prm files	*/

}PDUMP_CMDS_sPdump1Context;

// Function Definitions
IMG_VOID pdump_cmd_WriteWord(
	PDUMP_CMD_sTarget		stSrc,
	PDUMP_CMD_sTarget		stDest,
	IMG_UINT64				ui64MangledVal,
	IMG_UINT32				ui32WordSizeFlag
);

/*!
******************************************************************************
  This structure contains a node of the hash table of
  memory space information.
******************************************************************************/

typedef struct PDUMP_CMDS_sMemSpaceCBNode_tag
{
	/* memory space information */
	PDUMP_CMDS_sMemSpaceCB sMemSpaceCB;
	/* pointer to the next node in the row */
	struct PDUMP_CMDS_sMemSpaceCBNode_tag * psNext;

}PDUMP_CMDS_sMemSpaceCBNode;

/* If mallocs allowed: dynamic hash table */
static PDUMP_CMDS_sMemSpaceCBNode * gapsMemSpaceCBNodes[TAL_MAX_MEMSPACES];


static IMG_BOOL						gbCapturePdumpLite   = IMG_FALSE;	/*!< IMG_TRUE to enable pdump lite capture	             */
static IMG_BOOL						gbCaptureEnabled     = IMG_FALSE;   /*!< IMG_TRUE to enable capture of memory space accesses */

static IMG_UINT32					gui32DisableFlags = 0x0;			/*!< Flags used to disbale capture of specific PDUMP commands */

static PDUMP_CMDS_sPdump1Context	gsPdump1Context;							/*!< Global PDUMP1 context - always used for pdump1    */
static PDUMP_CMDS_sPdumpContext		gsPdumpContext;								/*!< Global PDUMP context - used when no contexts defined    */                           
static PDUMP_CMDS_sPdumpContext		gasPdumpContexts[TAL_MAX_PDUMP_CONTEXTS];   /*!< Defined PDUMP contexts                                  */

static IMG_UINT32				    gui32NoPdumpContexts = 0;                    /*!< No. of PDUMP contexts (0 if global context is being used */
static IMG_UINT32					gui32NoMemSpaces = 0;						/*!< Number of register memory spaces                   */

static IMG_CHAR *   gpszTestDirectory = IMG_NULL;  /*!< Test directory - for PDUMP output                         */

/* Latest caller information used for generation of PDUMPL FileID and LineNum command parameters */
static IMG_UINT32	gui32SourceFileNo = 0;      /*!< Source file number - used in Binary Lite output        */
static IMG_UINT32	gui32SourceFileLineNo = 0;  /*!< Source file line numbner - used in Binary Lite output  */

static IMG_CHAR		acTDFComments[MAX_TDF_COMMENT];	/*!< String to store TDF comments						*/
static IMG_BOOL		gbInitialised = IMG_FALSE;		/*!< Status of Pdump Commands							*/

static IMG_BOOL		gbSABVirtualWarning = IMG_FALSE;	/*!< Has Warning about Binary SAB with virtual address been printed? */

static IMG_CHAR *	gpszTDFFileName = IMG_NULL;			//*! Last used filename for creating a TDF file


static IMG_UINT32 gFlags = 0; // All flags disabled by default

static IMG_UINT32 gui32PdumpLoopLastCmd;				// Last Loop Command
static IMG_UINT64 gui64PdumpFileLoopPos;				// Position in the pdump script of the start of the loop


#include <sys/stat.h> // path stat
#ifdef WIN32
#include <direct.h> // _mkdir
#define MakeDirectory(path) _mkdir(path)
#define S_ISDIR(m) ((m&S_IFDIR) != 0)
#else
// rwx user, group and other
#define MakeDirectory(path) mkdir(path, S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH)
#endif

/**
 * @brief Checks if a directory path exists
 *
 * @return IMG_SUCCESS if the directory exists
 * @return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE if path is not found
 * @return IMG_ERROR_FATAL if path exists and it is not a directory
 * @return IMG_ERROR_INVALID_PARAMETERS if pszDirname is NULL
 */
static IMG_RESULT CheckDir(const IMG_CHAR *pszDirname)
{
	struct stat dirStat;
	
	if ( pszDirname == NULL )
	{
		return IMG_ERROR_INVALID_PARAMETERS;
	}

	if ( stat(pszDirname, &dirStat) != 0 )
	{
		return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE; // path not found
	}

	if ( S_ISDIR(dirStat.st_mode) )
	{
		return IMG_SUCCESS; // it is a directory
	}

	return IMG_ERROR_FATAL;
}

/**
 * @brief Verifies that a directory exists - if not create it
 *
 * @return IMG_SUCCESS if pszDirname exists or was created
 * @return IMG_ERROR_FATAL if pszDirname is an existing file (not a dir)
 * @return IMG_ERROR_INVALID_PARAMETERS if pszDirname is NULL
 *
 * @see Delegates to CheckDir
 */
static IMG_RESULT InsureDir(const IMG_CHAR *pszDirname)
{
	IMG_RESULT ret = CheckDir(pszDirname);
	
	if ( ret == IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE ) // cannot find dir
	{
		if ( MakeDirectory(pszDirname) != 0 )
		{
			ret = IMG_ERROR_FATAL;
		}
		ret = IMG_SUCCESS;
	}
	return ret;
}

/*!
******************************************************************************

 @Function              pdump_cmd_GetMemSpaceCB

******************************************************************************/

PDUMP_CMDS_psMemSpaceCB pdump_cmd_GetMemSpaceCB(
	IMG_UINT32		ui32MemSpaceId
	)
{
	IMG_UINT32 ui32I, ui32Node;
	PDUMP_CMDS_sMemSpaceCBNode * psReturnNode;
	
	ui32Node = ui32MemSpaceId % TAL_MAX_MEMSPACES;
	psReturnNode = gapsMemSpaceCBNodes[ui32Node];

	if (psReturnNode == NULL)
	{
		// Create Memspace
		PDUMP_CMDS_sMemSpaceCBNode * psNode = IMG_CALLOC(1, sizeof(PDUMP_CMDS_sMemSpaceCBNode));
		gapsMemSpaceCBNodes[ui32Node] = psNode;
		psReturnNode = psNode;
	}
	
	for(ui32I = 0; ui32I < ui32MemSpaceId / TAL_MAX_MEMSPACES; ui32I++)
	{
		if(psReturnNode->psNext == NULL)
		{
			// Create Memspace
			PDUMP_CMDS_sMemSpaceCBNode * psNode = IMG_CALLOC(1, sizeof(PDUMP_CMDS_sMemSpaceCBNode));
			psReturnNode->psNext = psNode;
			psReturnNode = psNode;
			break;
		}
		psReturnNode = psReturnNode->psNext;
	}
	
	
	return &psReturnNode->sMemSpaceCB;
}

/*!
******************************************************************************

 @Function              pdump_cmd_CheckContext

******************************************************************************/
 PDUMP_CMDS_psPdumpContext pdump_cmd_CheckContext (PDUMP_CMDS_psMemSpaceCB		psMemSpaceCB)
 {
	PDUMP_CMDS_psPdumpContext	psPdumpContext;
	
	if (psMemSpaceCB != NULL)
	{
		psPdumpContext = psMemSpaceCB->psPdumpContext;
		if (psPdumpContext == IMG_NULL)
		{
			printf("ERROR: Memory Space %s has not been assigned to a pdump context\n", psMemSpaceCB->pszMemSpaceName);
			IMG_ASSERT(IMG_FALSE);
		}
	}
	else
	{
		 /* Ensure that we are using the global context - otherwise we need to have a memory space to
           obtain the PDUMP context...*/
        IMG_ASSERT(gui32NoPdumpContexts == 0);
        psPdumpContext = &gsPdumpContext;
	}
	return psPdumpContext;
 }

/*!
******************************************************************************

 @Function              pdump_cmd_InitResPrmFileName

******************************************************************************/
IMG_VOID pdump_cmd_InitResPrmFileName ( 
	PDUMP_CMDS_sResPrmFileInfo* psFileInfo,
	IMG_CHAR* pszDefaultName
)
{
	// Initialise all function handles to NULL
	memset( psFileInfo->hFiles, 0, MAX_CUSTOM_RES_PRM_NAME_NUM * sizeof(IMG_PVOID*) );
		
	// Initialise all name pointers to NULL
	memset( &psFileInfo->apszNames, 0, MAX_CUSTOM_RES_PRM_NAME_NUM * sizeof(IMG_BYTE) );		

	// Initialise all file lengths to NULL
	memset( &psFileInfo->ui32FileSizes, 0, MAX_CUSTOM_RES_PRM_NAME_NUM * sizeof( IMG_UINT32 ) );
	
	// Add default name to first location
	psFileInfo->apszNames[0] = IMG_STRDUP( pszDefaultName );
	
	// Increment file name number...
	psFileInfo->ui32NameNum = 1;		// there is always the default name

	// Set that we are using the first location
	psFileInfo->ui32CurrentNameId = 0;
}

/*!
******************************************************************************

 @Function              pdump_cmd_StopResPrmFileName

******************************************************************************/
IMG_VOID pdump_cmd_StopResPrmFileName ( 
	PDUMP_CMDS_sResPrmFileInfo* psFileInfo	
)
{
	IMG_UINT32 ui32;
	
	/* 
	Close all files opened apart from the current one, that will be handled
	by the default close commands.
	*/
	pdumpAccessLock();
	for( ui32 = 0; ui32 < psFileInfo->ui32NameNum; ui32++ )
	{
		// skip the current file
		if( ui32 != psFileInfo->ui32CurrentNameId )
		{
			// close all other files
			if (psFileInfo->hFiles[ui32])
			{
				IMG_FILEIO_CloseFile( psFileInfo->hFiles[ui32] );
			}
		}
	}
	pdumpAccessUnlock();
}


/*!
******************************************************************************

 @Function              pdump_cmd_DeInitResPrmFileName

******************************************************************************/
IMG_VOID pdump_cmd_DeInitResPrmFileName ( 
	PDUMP_CMDS_sResPrmFileInfo* psFileInfo	
)
{
	IMG_UINT32 ui32;

	// free names of all files used
	for( ui32 = 0; ui32 < psFileInfo->ui32NameNum; ui32++ )
	{
		if (psFileInfo->apszNames[ui32])
		{
			IMG_FREE( psFileInfo->apszNames[ui32] );
			psFileInfo->apszNames[ui32] = IMG_NULL;
		}
	}
}


/*!
******************************************************************************

 @Function              pdump_cmd_InitPdump1Context

******************************************************************************/
static IMG_VOID pdump_cmd_InitPdump1Context()
{
	// Initialise OutTxt file
	gsPdump1Context.hOutTxtFile         = IMG_NULL;
    gsPdump1Context.psOutTxtFileName    = IMG_STRDUP("out.txt");


	/* --- Initialise default .prm/.res structure --- */
	pdump_cmd_InitResPrmFileName( &gsPdump1Context.asCustomPrmFiles, "out.prm" );	
	pdump_cmd_InitResPrmFileName( &gsPdump1Context.asCustomResFiles, "out.res" );
	
	
	// Initialise current .prm and .res to defaults
	gsPdump1Context.hOutPrmFile         = IMG_NULL;
    gsPdump1Context.hOutResFile         = IMG_NULL;
	gsPdump1Context.psOutPrmFileName	= gsPdump1Context.asCustomPrmFiles.apszNames[0];
	gsPdump1Context.psOutResFileName    = gsPdump1Context.asCustomResFiles.apszNames[0];
	gsPdump1Context.ui32OutPrmFileSize  = gsPdump1Context.asCustomPrmFiles.ui32FileSizes[0];
	gsPdump1Context.ui32OutResFileSize  = gsPdump1Context.asCustomResFiles.ui32FileSizes[0];
	gsPdump1Context.pszMemName			= (IMG_CHAR*)gpszDefaultMemName;
}

/*!
******************************************************************************

 @Function              pdump_cmd_DeinitPdump1Context

******************************************************************************/
static IMG_VOID pdump_cmd_DeinitPdump1Context()
{
	IMG_FREE(gsPdump1Context.psOutTxtFileName);
    
	// free custom .prs and .res names
	pdump_cmd_DeInitResPrmFileName( &gsPdump1Context.asCustomPrmFiles );
	pdump_cmd_DeInitResPrmFileName( &gsPdump1Context.asCustomResFiles );

	// NULL pointers to now deleted filenames
	gsPdump1Context.psOutResFileName = NULL;
	gsPdump1Context.psOutPrmFileName = NULL;

	
	if (gsPdump1Context.pszMemName != gpszDefaultMemName)
		IMG_FREE(gsPdump1Context.pszMemName);
}

/*!
******************************************************************************

 @Function              pdump_cmd_GetMemSpaceName

******************************************************************************/
static IMG_CHAR * pdump_cmd_GetMemSpaceName(
	IMG_UINT32					ui32i,
	PDUMP_CMDS_psPdumpContext	psPdumpContext
	)
{
	IMG_UINT32 ui32iter;
	PDUMP_CMDS_sMemSpaceNameNode * psNode = psPdumpContext->apsMemSpaceNameNodes[ui32i % TAL_MAX_MEMSPACES];

	for (ui32iter = 0; ui32iter < ui32i / TAL_MAX_MEMSPACES; ui32iter++)
	{
		psNode = psNode->psNext;
	}
	if (psNode)
		return psNode->pszMemSpaceName;
	return IMG_NULL;
}


/*!
******************************************************************************

 @Function              pdump_cmd_InitPdumpContext

******************************************************************************/
static IMG_VOID pdump_cmd_InitPdumpContext(
    IMG_CHAR *                  pszContextName,
    PDUMP_CMDS_psPdumpContext	psPdumpContext
    )
{
	IMG_CHAR                acBuffer[200];
	IMG_UINT32              ui32;

	psPdumpContext->hOut2TxtFile        = IMG_NULL;
    psPdumpContext->hOut2PrmFile        = IMG_NULL;
    psPdumpContext->hOut2ResFile        = IMG_NULL;
    psPdumpContext->hOutLiteBinFile     = IMG_NULL;
	psPdumpContext->ui32Out2PrmFileSize  = 0;
	psPdumpContext->ui32Out2ResFileSize  = 0;
	psPdumpContext->ui32OutLiteBinFiletSizeInDWords = 0;
    
	for (ui32 = 0; ui32 < TAL_MAX_MEMSPACES; ui32++)
	{
		psPdumpContext->apsMemSpaceNameNodes[ui32] = IMG_NULL;
	}

	if (pszContextName == IMG_NULL)
    {
        psPdumpContext->psOut2TxtFileName   = IMG_STRDUP("out2.txt");
        psPdumpContext->psOutLiteBinFileName = IMG_STRDUP("outL.bin");

		/* --- Initialise default .prm/.res structure --- */
		pdump_cmd_InitResPrmFileName( &psPdumpContext->asCustomPrmFiles, "out2.prm" );	
		pdump_cmd_InitResPrmFileName( &psPdumpContext->asCustomResFiles, "out2.res" );
		psPdumpContext->psOut2PrmFileName   = psPdumpContext->asCustomPrmFiles.apszNames[0];
        psPdumpContext->psOut2ResFileName   = psPdumpContext->asCustomResFiles.apszNames[0];
        
		psPdumpContext->pszContextName = IMG_NULL;
    }
    else
    {  
		sprintf(acBuffer, "%s_out2.txt", pszContextName);
        psPdumpContext->psOut2TxtFileName   = IMG_STRDUP(acBuffer);

        sprintf(acBuffer, "%s_outL.bin", pszContextName);
        psPdumpContext->psOutLiteBinFileName = IMG_STRDUP(acBuffer);
        		
		sprintf(acBuffer, "%s_out2.prm", pszContextName);
        pdump_cmd_InitResPrmFileName( &psPdumpContext->asCustomPrmFiles, acBuffer );
		psPdumpContext->psOut2PrmFileName   =  psPdumpContext->asCustomPrmFiles.apszNames[0];
        
		sprintf(acBuffer, "%s_out2.res", pszContextName);
        pdump_cmd_InitResPrmFileName( &psPdumpContext->asCustomResFiles, acBuffer );
		psPdumpContext->psOut2ResFileName   =  psPdumpContext->asCustomResFiles.apszNames[0];		
		
		psPdumpContext->pszContextName = IMG_STRDUP(pszContextName);

    }
    psPdumpContext->bPdumpEnabled = IMG_TRUE;
	psPdumpContext->ui32ActiveSyncs = 0;
}

/*!
******************************************************************************

 @Function              pdump_cmd_DeinitPdumpContext

******************************************************************************/
static IMG_VOID pdump_cmd_DeinitPdumpContext(
    PDUMP_CMDS_psPdumpContext	psPdumpContext
    )
{
	PDUMP_CMDS_sMemSpaceNameNode *psNode, *psNodeNext;
	IMG_UINT32 ui32 = 0;
	IMG_CHAR * pszName;
    
	IMG_FREE(psPdumpContext->psOut2TxtFileName);
    IMG_FREE(psPdumpContext->psOutLiteBinFileName);
    
	// free custom .prs and .res names
	pdump_cmd_DeInitResPrmFileName( &psPdumpContext->asCustomPrmFiles );
	pdump_cmd_DeInitResPrmFileName( &psPdumpContext->asCustomResFiles );

	// NULL pointers to now deleted filenames
	psPdumpContext->psOut2ResFileName = NULL;
	psPdumpContext->psOut2PrmFileName = NULL;
	
	if (psPdumpContext->pszContextName != IMG_NULL)
    {
        IMG_FREE(psPdumpContext->pszContextName);
    }

	pszName = pdump_cmd_GetMemSpaceName(ui32, psPdumpContext);
	while (pszName != IMG_NULL)
    {
        IMG_FREE(pszName);
		ui32++;
		pszName = pdump_cmd_GetMemSpaceName(ui32, psPdumpContext);
    }

	for (ui32 = 0; ui32 < TAL_MAX_MEMSPACES; ui32++)
	{
		psNode = psPdumpContext->apsMemSpaceNameNodes[ui32];
		while (psNode != IMG_NULL)
		{
			psNodeNext = psNode->psNext;
			free(psNode);
			psNode=psNodeNext;
		}
	}
}

/*!
******************************************************************************

 @Function             pdump_cmd_GetContext

******************************************************************************/
static PDUMP_CMDS_psPdumpContext pdump_cmd_GetContext (
    IMG_CHAR *		pszContextName
)
{
	IMG_UINT32	ui32;
	if(pszContextName == NULL)
		return &gsPdumpContext;

    /* Check to see if context already defined...*/
    for (ui32=0; ui32<gui32NoPdumpContexts; ui32++)
    {
        if (strcmp(pszContextName, gasPdumpContexts[ui32].pszContextName) == 0)
        {
            return &gasPdumpContexts[ui32];
        }
    }
	
	return IMG_NULL;
}

/*!
******************************************************************************

 @Function              pdump_cmd_CheckSlitRequired

******************************************************************************/
static IMG_VOID pdump_cmd_CheckSlitRequired(
    IMG_CHAR **             ppszFileName,
    IMG_HANDLE *            phFile,
    IMG_UINT32 *            pui32FileSize,
    IMG_UINT32              ui32Size
)
{
    IMG_CHAR *              pszTmpFileName = IMG_NULL;
	IMG_CHAR *				pszCaptureFile = IMG_NULL;
    IMG_UINT32              ui32StrSize;

    /* If file not open...*/
    if (*phFile == IMG_NULL)
    {
        /* Return...*/
        return;
    }

    /* If this would take us over 2GB...*/
    if ((*pui32FileSize + ui32Size) >= 0x80000000)
    {
pdumpAccessLock();
		/* Close file */
		IMG_FILEIO_CloseFile(*phFile);
pdumpAccessUnlock();

        /* Get size of file name....*/
        ui32StrSize = (IMG_UINT32)strlen(*ppszFileName);
        IMG_ASSERT((*ppszFileName)[ui32StrSize-4] == '.');
        if ( ((*ppszFileName)[ui32StrSize-5] >= 'A') && ((*ppszFileName)[ui32StrSize-5] <= 'Z') )
        {
            (*ppszFileName)[ui32StrSize-5]++;
        }
        else
        {
            pszTmpFileName = IMG_STRDUP(*ppszFileName);
            pszTmpFileName[ui32StrSize-4] = 0;
			*ppszFileName = IMG_REALLOC(*ppszFileName, ui32StrSize + 2);
            sprintf(*ppszFileName, "%sA.%s", pszTmpFileName, &pszTmpFileName[ui32StrSize-3]);
        }

        /* Create filename directory...*/
        IMG_ASSERT(gpszTestDirectory != IMG_NULL);
        ui32StrSize = (IMG_UINT32)strlen(gpszTestDirectory);
        if ( (ui32StrSize == 0) || (gpszTestDirectory[ui32StrSize-1] == DIRECTORY_SEPARATOR_IMG_CHAR) )
        {
			pszCaptureFile = IMG_MALLOC(ui32StrSize + strlen(*ppszFileName) + 1);
            sprintf(pszCaptureFile, "%s%s", gpszTestDirectory, *ppszFileName);
        }
        else
        {
			pszCaptureFile = IMG_MALLOC(ui32StrSize + strlen(*ppszFileName) + 2);
            sprintf(pszCaptureFile, "%s" DIRECTORY_SEPARATOR "%s", gpszTestDirectory, *ppszFileName);
        }

pdumpAccessLock();
		IMG_FILEIO_OpenFile(pszCaptureFile, "wb", phFile, GET_ENABLE_COMPRESSION);
pdumpAccessUnlock();

		if (pszCaptureFile) IMG_FREE(pszCaptureFile);
        *pui32FileSize = 0;
    }
}

/*!
******************************************************************************

 @Function              pdmup_cmd_getMemBlock

******************************************************************************/
static IMG_UINT32 pdmup_cmd_Hash(IMG_UINT32 ui32MemBlockId)
{
	return ui32MemBlockId & 0xF;
}

/*!
******************************************************************************

 @Function              pdmup_cmd_getMemBlock

******************************************************************************/
static PDUMP_CMDS_psMemBlock pdmup_cmd_getMemBlock(
	PDUMP_CMDS_psMemSpaceCB	psMemSpaceCB,
	IMG_UINT32 ui32MemBlockId
	)
{
	PDUMP_CMDS_psMemBlock	psMemBlock;
	IMG_UINT32				ui32Hash = pdmup_cmd_Hash(ui32MemBlockId);
	IMG_ASSERT(ui32MemBlockId != TAL_NO_MEM_BLOCK);
	IMG_ASSERT(psMemSpaceCB->apsMemBlock[ui32Hash] != IMG_NULL);
	/* Hash Table using last digit in hex word to define container */
	psMemBlock = psMemSpaceCB->apsMemBlock[ui32Hash];
    while (psMemBlock->ui32MemBlockId != ui32MemBlockId)
	{
		psMemBlock = psMemBlock->psNextMemBlock;
		if (psMemBlock == IMG_NULL)
		{
			printf("ERROR PDUMP_CMD Memory Block Id %d not recognised\n", ui32MemBlockId);
			IMG_ASSERT(psMemBlock == IMG_NULL);
		}
	}
	return psMemBlock;
}

/*!
******************************************************************************

 @Function              pdmup_cmd_addMemBlock

******************************************************************************/
static IMG_VOID pdmup_cmd_addMemBlock(
	PDUMP_CMDS_psMemSpaceCB	psMemSpaceCB,
	IMG_UINT32				ui32MemBlockId,
	IMG_CHAR*				pszMemBlockName,
	IMG_UINT32				ui32Size,
	IMG_UINT64				ui64DevAddr
)
{
	PDUMP_CMDS_psMemBlock	psMemBlock = (PDUMP_CMDS_psMemBlock)IMG_MALLOC(sizeof(PDUMP_CMDS_sMemBlock));
	IMG_UINT32				ui32Hash = pdmup_cmd_Hash(ui32MemBlockId);
	IMG_ASSERT(ui32MemBlockId != TAL_NO_MEM_BLOCK);

    psMemBlock->ui32MemBlockId = ui32MemBlockId;
	psMemBlock->ui32Size = ui32Size;
	psMemBlock->ui64DevAddr = ui64DevAddr;
	psMemBlock->pszBlockName = IMG_STRDUP(pszMemBlockName);
	psMemBlock->psNextMemBlock = psMemSpaceCB->apsMemBlock[ui32Hash];
	psMemSpaceCB->apsMemBlock[ui32Hash] = psMemBlock;
}

/*!
******************************************************************************

 @Function              pdmup_cmd_removeMemBlock

******************************************************************************/
static IMG_VOID pdmup_cmd_removeMemBlock(
		PDUMP_CMDS_psMemSpaceCB	psMemSpaceCB,
		IMG_UINT32	ui32MemBlockId
)
{
	PDUMP_CMDS_psMemBlock	psMemBlock;
	PDUMP_CMDS_psMemBlock	psTmpMemBlock;
	IMG_UINT32				ui32Hash = pdmup_cmd_Hash(ui32MemBlockId);
	IMG_ASSERT(ui32MemBlockId != TAL_NO_MEM_BLOCK);
	IMG_ASSERT(psMemSpaceCB->apsMemBlock[ui32Hash] != IMG_NULL);

	psMemBlock = psMemSpaceCB->apsMemBlock[ui32Hash];
	if (psMemBlock->ui32MemBlockId == ui32MemBlockId)
	{
		psMemSpaceCB->apsMemBlock[ui32Hash] = psMemBlock->psNextMemBlock;
		IMG_FREE(psMemBlock->pszBlockName);
		IMG_FREE(psMemBlock);
	}
	else
	{
		IMG_ASSERT(psMemBlock->psNextMemBlock != IMG_NULL);
		while (psMemBlock->psNextMemBlock->ui32MemBlockId != ui32MemBlockId)
		{
			psMemBlock = psMemBlock->psNextMemBlock;
			if (psMemBlock->psNextMemBlock == IMG_NULL)
			{
				printf("ERROR PDUMP_CMD Memory Block Id %d not recognised\n", ui32MemBlockId);
				IMG_ASSERT(psMemBlock == IMG_NULL);
			}
		}
		psTmpMemBlock = psMemBlock->psNextMemBlock;
		psMemBlock->psNextMemBlock = psTmpMemBlock->psNextMemBlock;
		IMG_FREE(psTmpMemBlock->pszBlockName);
		IMG_FREE(psTmpMemBlock);
	}
}

/*!
******************************************************************************

 @Function              pdmup_cmd_MakePdump2

******************************************************************************/
static IMG_VOID pdmup_cmd_MakePdump2(
	IMG_CHAR *				cBuffer,
	PDUMP_CMD_sTarget		stTarget,
	IMG_BOOL				bNoOffset)
{
	PDUMP_CMDS_psMemSpaceCB		psMemSpaceCB = IMG_NULL;
	PDUMP_CMDS_psMemBlock		psMemBlock = IMG_NULL;

	if (stTarget.hMemSpace != NULL)
	{
		psMemSpaceCB = (PDUMP_CMDS_psMemSpaceCB)stTarget.hMemSpace;
	}
	switch (stTarget.eMemSpT)
	{
	/* Output Register or Slave Port Pdump2 */
	case PDUMP_MEMSPACE_REGISTER:
	case PDUMP_MEMSPACE_SLAVE_PORT:
		IMG_ASSERT(psMemSpaceCB != IMG_NULL);
		if (bNoOffset)
		{
			sprintf(cBuffer, ":%s", psMemSpaceCB->pszMemSpaceName);
		}
		else
		{
			sprintf(cBuffer, ":%s:0x%" IMG_I64PR "X", psMemSpaceCB->pszMemSpaceName, stTarget.ui64Value);
		}
		break;
	/* Output Internal Register Pdump2 */
	case PDUMP_MEMSPACE_INT_VAR:
		IMG_ASSERT(psMemSpaceCB != IMG_NULL);
		sprintf(cBuffer, ":%s:$%d", psMemSpaceCB->pszMemSpaceName, IMG_UINT64_TO_UINT32(stTarget.ui64Value));
		break;
	case PDUMP_MEMSPACE_VALUE:
		sprintf(cBuffer, "0x%" IMG_I64PR "X", stTarget.ui64Value);
		break;
	/* Output Memory Pdump2 */
	case PDUMP_MEMSPACE_MEMORY:
	case PDUMP_MEMSPACE_MAPPED_MEMORY:
		IMG_ASSERT(psMemSpaceCB != IMG_NULL);
		IMG_ASSERT(stTarget.ui32BlockId != TAL_NO_MEM_BLOCK);
		psMemBlock = pdmup_cmd_getMemBlock(psMemSpaceCB, stTarget.ui32BlockId);

		if (bNoOffset)
		{
			sprintf(cBuffer, ":%s:%s", psMemSpaceCB->pszMemSpaceName, psMemBlock->pszBlockName);
		}
		else
		{
			sprintf(cBuffer, ":%s:%s:0x%" IMG_I64PR "X", psMemSpaceCB->pszMemSpaceName, psMemBlock->pszBlockName, stTarget.ui64Value);
		}

		break;
	/* Output Virtual Memory Pdump2 */
	case PDUMP_MEMSPACE_VIRTUAL:
		IMG_ASSERT(psMemSpaceCB != IMG_NULL);
		if (stTarget.ui32BlockId == TAL_DEV_VIRT_QUALIFIER_NONE)
		{
			if (bNoOffset)
			{
				sprintf(cBuffer, ":%s:v", psMemSpaceCB->pszMemSpaceName);
			}
			else
			{
				sprintf(cBuffer, ":%s:v:0x%" IMG_I64PR "X", psMemSpaceCB->pszMemSpaceName, stTarget.ui64Value);
			}
		}
		else
		{
			if (bNoOffset)
			{
				sprintf(cBuffer, ":%s:v%d", psMemSpaceCB->pszMemSpaceName, stTarget.ui32BlockId);
			}
			else
			{
				sprintf(cBuffer, ":%s:v%d:0x%" IMG_I64PR "X", psMemSpaceCB->pszMemSpaceName, stTarget.ui32BlockId, stTarget.ui64Value);
			}
		}
		break;
	default:
		printf("PDUMP_CMD ERROR : Unknown Memspace Type, %s, in WRW Output\n", psMemSpaceCB->pszMemSpaceName);
		IMG_ASSERT(IMG_FALSE);
		break;
	}
	return;
}

/*!
******************************************************************************

 @Function              pdmup_cmd_MakePdump1

******************************************************************************/
static IMG_VOID pdmup_cmd_MakePdump1(
	IMG_CHAR *				cBuffer,
	PDUMP_CMD_sTarget		stTarget
)
{
	IMG_UINT64					ui64MemSpaceOffset;
	IMG_CHAR *					pszMemName;
	PDUMP_CMDS_psMemSpaceCB		psMemSpaceCB = IMG_NULL;

	if (stTarget.hMemSpace != NULL)
	{
		psMemSpaceCB = (PDUMP_CMDS_psMemSpaceCB)stTarget.hMemSpace;
	}

	switch (stTarget.eMemSpT)
	{
	/* Output Register or Slave Port Pdump1 */
	case PDUMP_MEMSPACE_REGISTER:
	case PDUMP_MEMSPACE_SLAVE_PORT:
		sprintf(cBuffer, ":%s:%08X", psMemSpaceCB->pszMemSpaceName, IMG_UINT64_TO_UINT32(stTarget.ui64Value));
		break;
	case PDUMP_MEMSPACE_MEMORY:
	case PDUMP_MEMSPACE_MAPPED_MEMORY:
		// Setup Pdump1 to use either memory blocks and offsets or offset all from the base address
		if (psMemSpaceCB->ui64BaseAddress == TAL_BASE_ADDR_UNDEFINED || gsPdump1Context.pszMemName == IMG_NULL)
		{
#ifdef PDUMP1_NO_RELATIVE_OFFSET
			ui64MemSpaceOffset = stTarget.ui64BlockAddr; // For historical reasons this can't be done (- psMemSpaceCB->ui64BaseAddress;
#else
			ui64MemSpaceOffset = stTarget.ui64BlockAddr - psMemSpaceCB->ui64BaseAddress; // This is the way it should work
#endif
			pszMemName = psMemSpaceCB->pszMemSpaceName;
		}
		else
		{
			ui64MemSpaceOffset = stTarget.ui64BlockAddr;
			pszMemName = gsPdump1Context.pszMemName;
		}

		if ( (ui64MemSpaceOffset >> 32) > 0 )
		{
			sprintf(cBuffer, ":%s:%016" IMG_I64PR "X ", pszMemName, ui64MemSpaceOffset + stTarget.ui64Value);
		}
		else
		{
			sprintf(cBuffer, ":%s:%08X", pszMemName, IMG_UINT64_TO_UINT32(ui64MemSpaceOffset) + IMG_UINT64_TO_UINT32(stTarget.ui64Value));
		}
		break;
	case PDUMP_MEMSPACE_VIRTUAL:
		if (stTarget.ui32BlockId == TAL_NO_MEM_BLOCK)
		{
			sprintf(cBuffer, ":%s:v:%08X", psMemSpaceCB->pszMemSpaceName, IMG_UINT64_TO_UINT32(stTarget.ui64BlockAddr) + IMG_UINT64_TO_UINT32(stTarget.ui64Value));
		}
		else
		{
			sprintf(cBuffer, ":%s:v%d:%08X", psMemSpaceCB->pszMemSpaceName, stTarget.ui32BlockId, IMG_UINT64_TO_UINT32(stTarget.ui64BlockAddr) + IMG_UINT64_TO_UINT32(stTarget.ui64Value));
		}
		break;
	/* Output Internal Register Pdump1 */
	case PDUMP_MEMSPACE_INT_VAR:
		sprintf(cBuffer, "$%d", IMG_UINT64_TO_UINT32(stTarget.ui64Value));
		break;
	case PDUMP_MEMSPACE_VALUE:
		sprintf(cBuffer, "%08X", IMG_UINT64_TO_UINT32(stTarget.ui64Value));		
		break;
	default:
		printf("PDUMP_CMD ERROR : Unknown Memspace Type, %s, in Output\n", psMemSpaceCB->pszMemSpaceName);
		IMG_ASSERT(IMG_FALSE);
		break;
	}
	return;
}

/*!
******************************************************************************

 @Function              pdump_cmd_ClosePdumpContext

******************************************************************************/
static IMG_VOID pdump_cmd_ClosePdumpContext(
    PDUMP_CMDS_psPdumpContext psPdumpContext
	)
{
	if (gbCapturePdumpLite)
	{
pdumpAccessLock();
		IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, TAL_PDUMPL_SCRIPT_END_SIG);

		/* Seek to the second word in the file, and write the length of the script */
		IMG_FILEIO_SeekFile(psPdumpContext->hOutLiteBinFile, 4, SEEK_SET);
		IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, psPdumpContext->ui32OutLiteBinFiletSizeInDWords);

		/* Close file */
		IMG_FILEIO_CloseFile(psPdumpContext->hOutLiteBinFile);
pdumpAccessUnlock();
	}

	/* Close files */
	if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP2))
	{
pdumpAccessLock();
		// close default files
		IMG_FILEIO_CloseFile(psPdumpContext->hOut2TxtFile);
		IMG_FILEIO_CloseFile(psPdumpContext->hOut2PrmFile);
		IMG_FILEIO_CloseFile(psPdumpContext->hOut2ResFile);
pdumpAccessUnlock();
		
		if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PRM))
		{
			pdump_cmd_StopResPrmFileName( &psPdumpContext->asCustomPrmFiles );
		}
		if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_RES))
		{
			pdump_cmd_StopResPrmFileName( &psPdumpContext->asCustomResFiles );
		}		
	}
}


/*!
******************************************************************************

 @Function              pdmup_cmd_OpenPdumpContext

******************************************************************************/
static IMG_VOID pdmup_cmd_OpenPdumpContext(
    PDUMP_CMDS_psPdumpContext psPdumpContext
)
{
    IMG_UINT32				ui32Size;
    IMG_CHAR *              pszCaptureOut2TxtFile = IMG_NULL;
	IMG_CHAR *				pszCaptureOut2PrmFile = IMG_NULL;
    IMG_CHAR *              pszCaptureOut2ResFile = IMG_NULL;
	IMG_RESULT startPdump;

	PDUMP_CMDS_psMemSpaceCB		psMemSpaceCB;

	if ( (startPdump = InsureDir(gpszTestDirectory)) != IMG_SUCCESS )
	{
		fprintf(stderr, "WARNING: failed to insure pdump directory '%s' exists (is it a file?)\n", gpszTestDirectory ? gpszTestDirectory : "");
		IMG_ASSERT(startPdump==IMG_SUCCESS); // die if we have to
		return; // exit anyway
	}

    /* Create filename directory...*/
    ui32Size = (IMG_UINT32)strlen(gpszTestDirectory);
    if ( (ui32Size == 0) || (gpszTestDirectory[ui32Size-1] == DIRECTORY_SEPARATOR_IMG_CHAR) )
    {
		pszCaptureOut2TxtFile = IMG_MALLOC(ui32Size + strlen(psPdumpContext->psOut2TxtFileName) + 1);
        sprintf(pszCaptureOut2TxtFile, "%s%s", gpszTestDirectory, psPdumpContext->psOut2TxtFileName);
		pszCaptureOut2PrmFile = IMG_MALLOC(ui32Size + strlen(psPdumpContext->psOut2PrmFileName) + 1);
        sprintf(pszCaptureOut2PrmFile, "%s%s", gpszTestDirectory, psPdumpContext->psOut2PrmFileName);
		pszCaptureOut2ResFile = IMG_MALLOC(ui32Size + strlen(psPdumpContext->psOut2ResFileName) + 1);
        sprintf(pszCaptureOut2ResFile, "%s%s", gpszTestDirectory, psPdumpContext->psOut2ResFileName);
		    }
    else
    {
		pszCaptureOut2TxtFile = IMG_MALLOC(ui32Size + strlen(psPdumpContext->psOut2TxtFileName) + 2);
        sprintf(pszCaptureOut2TxtFile, "%s" DIRECTORY_SEPARATOR "%s", gpszTestDirectory, psPdumpContext->psOut2TxtFileName);
		pszCaptureOut2PrmFile = IMG_MALLOC(ui32Size + strlen(psPdumpContext->psOut2PrmFileName) + 2);
        sprintf(pszCaptureOut2PrmFile, "%s" DIRECTORY_SEPARATOR "%s", gpszTestDirectory, psPdumpContext->psOut2PrmFileName);
		pszCaptureOut2ResFile = IMG_MALLOC(ui32Size + strlen(psPdumpContext->psOut2ResFileName) + 2);
        sprintf(pszCaptureOut2ResFile, "%s" DIRECTORY_SEPARATOR "%s", gpszTestDirectory, psPdumpContext->psOut2ResFileName);
    }

    /* Open files - conditional on the appropriate txt files being enabled. */
    if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP2))
    {
pdumpAccessLock();
        startPdump = IMG_FILEIO_OpenFile(pszCaptureOut2TxtFile, "wb", &psPdumpContext->hOut2TxtFile, GET_ENABLE_COMPRESSION);
		if ( startPdump != IMG_SUCCESS )
		{
			fprintf(stderr, "failed to open %s\n", pszCaptureOut2TxtFile);
			goto lock_exit;
		}
		
		if (psPdumpContext->pszContextName != IMG_NULL)
		{
			IMG_UINT32		i;
			/* Output to PDUMP file ...*/
			IMG_FILEIO_WriteToFile(psPdumpContext->hOut2TxtFile, "PDC ", 4);
			IMG_FILEIO_WriteToFile(psPdumpContext->hOut2TxtFile, psPdumpContext->pszContextName, (IMG_UINT32)strlen(psPdumpContext->pszContextName));
			i = 0;
			for(i = 0; i < gui32NoMemSpaces; i++)
			{
				psMemSpaceCB = pdump_cmd_GetMemSpaceCB(i);
				if (psMemSpaceCB->psPdumpContext == psPdumpContext)
				{
					IMG_FILEIO_WriteToFile(psPdumpContext->hOut2TxtFile, " ", 1);
					IMG_FILEIO_WriteToFile(psPdumpContext->hOut2TxtFile, psMemSpaceCB->pszMemSpaceName, (IMG_UINT32)strlen(psMemSpaceCB->pszMemSpaceName));
				}
			}
			IMG_FILEIO_WriteToFile(psPdumpContext->hOut2TxtFile, "\n", 1);
		}
		
		if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PRM))
		{
			startPdump = IMG_FILEIO_OpenFile(pszCaptureOut2PrmFile, "wb", &psPdumpContext->hOut2PrmFile, GET_ENABLE_COMPRESSION);
			if ( startPdump != IMG_SUCCESS )
			{
				fprintf(stderr, "failed to open %s\n", pszCaptureOut2PrmFile);
				IMG_FILEIO_CloseFile(psPdumpContext->hOut2TxtFile);
				goto lock_exit;
			}
			psPdumpContext->ui32Out2PrmFileSize = 0;
		}
		if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_RES))
		{
			startPdump = IMG_FILEIO_OpenFile(pszCaptureOut2ResFile, "wb", &psPdumpContext->hOut2ResFile, GET_ENABLE_COMPRESSION);
			if ( startPdump != IMG_SUCCESS )
			{
				fprintf(stderr, "failed to open %s\n", pszCaptureOut2ResFile);
				IMG_FILEIO_CloseFile(psPdumpContext->hOut2TxtFile);
				if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PRM))
				{
					IMG_FILEIO_CloseFile(psPdumpContext->hOut2PrmFile);
				}
			}
			psPdumpContext->ui32Out2ResFileSize = 0;
		}

lock_exit:
pdumpAccessUnlock();
	}

	if(pszCaptureOut2TxtFile) IMG_FREE(pszCaptureOut2TxtFile);
	if(pszCaptureOut2PrmFile) IMG_FREE(pszCaptureOut2PrmFile);
	if(pszCaptureOut2ResFile) IMG_FREE(pszCaptureOut2ResFile);
	IMG_ASSERT(startPdump == IMG_SUCCESS); // now we can die if we have to (files are closed and memory is clean)
}

/*!
******************************************************************************

 @Function              pdump_cmd_OpenPdumpBinaryLiteContext

******************************************************************************/
static IMG_VOID pdump_cmd_OpenPdumpBinaryLiteContext(
    PDUMP_CMDS_sPdumpContext * psPdumpContext
)
{
    IMG_UINT32              ui32Size;
    IMG_CHAR *              pszCaptureOutLiteBinFile = IMG_NULL;

    /* Create filename directory...*/
    ui32Size = (IMG_UINT32)strlen(gpszTestDirectory);
    if ( (ui32Size == 0) || (gpszTestDirectory[ui32Size-1] == DIRECTORY_SEPARATOR_IMG_CHAR) )
    {
		pszCaptureOutLiteBinFile = IMG_MALLOC(ui32Size + strlen(psPdumpContext->psOutLiteBinFileName) + 1);
        sprintf(pszCaptureOutLiteBinFile, "%s%s", gpszTestDirectory, psPdumpContext->psOutLiteBinFileName);
    }
    else
    {
		pszCaptureOutLiteBinFile = IMG_MALLOC(ui32Size + strlen(psPdumpContext->psOutLiteBinFileName) + 2);
        sprintf(pszCaptureOutLiteBinFile, "%s" DIRECTORY_SEPARATOR "%s", gpszTestDirectory, psPdumpContext->psOutLiteBinFileName);
    }

pdumpAccessLock();
    /* Open file */
    IMG_FILEIO_OpenFile(pszCaptureOutLiteBinFile, "wb", &psPdumpContext->hOutLiteBinFile, GET_ENABLE_COMPRESSION);

	/* Set all of these to 0 for consistency */
    gsPdump1Context.ui32OutPrmFileSize = 0;
	psPdumpContext->ui32Out2PrmFileSize = 0;
    gsPdump1Context.ui32OutResFileSize = 0;
    psPdumpContext->ui32Out2ResFileSize = 0;

    /* Output the start of file signature */
	IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, TAL_PDUMPL_SCRIPT_START_SIG);
	IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, 0);						/* Write a temporary length field */
pdumpAccessUnlock();

	/* Size of the "payload" of the binary lite script */
	psPdumpContext->ui32OutLiteBinFiletSizeInDWords = 0;
	if(pszCaptureOutLiteBinFile) IMG_FREE(pszCaptureOutLiteBinFile);
}


/*!
******************************************************************************

 @Function				pdump_cmd_CheckCmdDisabled

******************************************************************************/
IMG_BOOL pdump_cmd_CheckCmdDisabled(
	PDUMP_CMDS_psMemSpaceCB			psMemSpaceCB,
	IMG_UINT32						ui32Offset,
	IMG_UINT32						ui32DisableFlags
)
{
    IMG_UINT32                      i;

    /* If disable globally...*/
    if (
            (!gbCaptureEnabled) ||
            ((gui32DisableFlags & ui32DisableFlags) != 0)
        )
    {
        return IMG_TRUE;
    }

    /* If disabled for mempry space...*/
    if (
            (!psMemSpaceCB->bCaptureEnabled) ||
            ((psMemSpaceCB->ui32DisableFlags & ui32DisableFlags) != 0)
        )
    {
        return IMG_TRUE;
    }

    /* Check if disabled for register...*/
    for (i=0; i<psMemSpaceCB->ui32NoDisabledOffsets; i++)
    {
        if (psMemSpaceCB->aui32DisableOffset[i] == ui32Offset)
        {
            return ((psMemSpaceCB->aui32DisableFlags[i] & ui32DisableFlags) != 0);
        }
    }

    /* Not disabled...*/
    return IMG_FALSE;
}


/*!
******************************************************************************

 @Function              PDUMP_CMD_Initialise

******************************************************************************/
IMG_VOID PDUMP_CMD_Initialise ()
{
	if (gbInitialised != IMG_TRUE)
	{
		/* Initialise the global PDUMP1 context...*/
		pdump_cmd_InitPdump1Context();
		pdump_cmd_InitPdumpContext(IMG_NULL, &gsPdumpContext);
		acTDFComments[0] = 0;
		acConfigFile[0] = 0;

		pdumpAccessInit();
	}

	/* Set intialised to TRUE */
	gbInitialised = IMG_TRUE;
}

/*!
******************************************************************************

 @Function              PDUMP_CMD_Deinitialise

******************************************************************************/
IMG_VOID PDUMP_CMD_Deinitialise ()
{
	PDUMP_CMDS_psMemSpaceCB		psMemSpaceCB;
	IMG_UINT32	ui32, ui32i;
	PDUMP_CMDS_sMemSpaceCBNode *psNode, *psNodeNext;

	if (gbInitialised == IMG_TRUE)
	{
        //Be sure to stop the PDUMPING to avoid memory leaks
        PDUMP_CMD_CaptureStop();

		/* Initialise the global PDUMP1 context...*/
		pdump_cmd_DeinitPdump1Context();
		pdump_cmd_DeinitPdumpContext(&gsPdumpContext);

		/* Free any additional contexts...*/
		for (ui32=0; ui32<gui32NoPdumpContexts; ui32++)
		{
			pdump_cmd_DeinitPdumpContext(&gasPdumpContexts[ui32]);
		}

		gui32NoPdumpContexts = 0;

		for(ui32 = 0; ui32 < gui32NoMemSpaces; ui32++)
		{
			psMemSpaceCB = pdump_cmd_GetMemSpaceCB(ui32);
			for(ui32i = 0; ui32i < PDUMPCMDS_HASHSIZE; ui32i++)
			{
				while (psMemSpaceCB->apsMemBlock[ui32i] != IMG_NULL)
				{
					pdmup_cmd_removeMemBlock(psMemSpaceCB, psMemSpaceCB->apsMemBlock[ui32i]->ui32MemBlockId);
				}
			}
			IMG_FREE(psMemSpaceCB->pszMemSpaceName);
		}

		for ( ui32 = 0; ui32 < gui32NoMemSpaces && ui32 < TAL_MAX_MEMSPACES; ui32++)
		{
			psNode = gapsMemSpaceCBNodes[ui32];
			while (psNode != IMG_NULL)
			{
				psNodeNext = psNode->psNext;
				free(psNode);
				psNode = psNodeNext;
			}
			gapsMemSpaceCBNodes[ui32] = NULL;
		}
		gui32NoMemSpaces = 0;

		pdumpAccessDeinit();
	}

	/* Set intialised to FALSE */
	gbInitialised = IMG_FALSE;
}

/*!
******************************************************************************

 @Function				PDUMP_CMD_AddMemspaceToPdumpContext

******************************************************************************/
IMG_HANDLE PDUMP_CMD_AddMemspaceToPdumpContext(	
	IMG_CHAR *	    pszContextName,
	IMG_HANDLE		hMemSpace
)
{
	IMG_UINT32					ui32MemSpaceId, ui32;
    PDUMP_CMDS_psPdumpContext	psPdumpContext = IMG_NULL;
	PDUMP_CMDS_psMemSpaceCB		psMemSpaceCB = NULL;
	IMG_CHAR					*pszName;
	
	/* Check to see if context already defined...*/
    psPdumpContext = pdump_cmd_GetContext(pszContextName);
	
    // Setup new context
    if (psPdumpContext == IMG_NULL)
	{
	  	IMG_ASSERT(gui32NoPdumpContexts < TAL_MAX_PDUMP_CONTEXTS);

		/* If this is the first PDUMP context to be defined...*/
		if (gui32NoPdumpContexts == 0)
		{
			/* Decouple ALL of the memory spaces from the global PDUMP context...*/
			for (ui32MemSpaceId=0; ui32MemSpaceId<gui32NoMemSpaces; ui32MemSpaceId++)
			{
				psMemSpaceCB = pdump_cmd_GetMemSpaceCB(ui32MemSpaceId);
				psMemSpaceCB->psPdumpContext = IMG_NULL;
			}
		}

		/* Get access to a new PDUMP context...*/
		psPdumpContext = &gasPdumpContexts[gui32NoPdumpContexts++];

		/* Initialise the context...*/
		pdump_cmd_InitPdumpContext(pszContextName, psPdumpContext);
	}
	
	psMemSpaceCB = (PDUMP_CMDS_psMemSpaceCB)hMemSpace;
	
	// Check name doesn't already exist
	ui32 = 0;
	pszName = pdump_cmd_GetMemSpaceName(ui32, psPdumpContext);
	while (pszName != IMG_NULL)
	{
		if (strcmp(pszName, psMemSpaceCB->pszMemSpaceName) == 0)
		{
			// Already have this entry
			return (IMG_HANDLE)psPdumpContext;
		}
		pszName = pdump_cmd_GetMemSpaceName(ui32++, psPdumpContext);
	}
	// Add name to context
	psMemSpaceCB->psPdumpContext = psPdumpContext;
	return (IMG_HANDLE)psPdumpContext;
}			

/*!
******************************************************************************

 @Function             PDUMP_CMD_MemSpaceRegister

******************************************************************************/
IMG_HANDLE PDUMP_CMD_MemSpaceRegister (
    IMG_CHAR *  pszMemSpaceName
)
{
	PDUMP_CMDS_psMemSpaceCB psMemSpaceCB;

	psMemSpaceCB = pdump_cmd_GetMemSpaceCB(gui32NoMemSpaces);

    /* Setup local information relating to this memory space */
    memset(psMemSpaceCB, 0, sizeof(PDUMP_CMDS_sMemSpaceCB));
    psMemSpaceCB->ui64BaseAddress = TAL_BASE_ADDR_UNDEFINED;
	psMemSpaceCB->pszMemSpaceName = IMG_STRDUP(pszMemSpaceName);
	psMemSpaceCB->bCaptureEnabled  = IMG_FALSE;
	psMemSpaceCB->psPdumpContext = IMG_NULL;
	
    /* Update no. of memory spaces */
    gui32NoMemSpaces++;
	return (IMG_HANDLE)psMemSpaceCB;
}

/*!
******************************************************************************

 @Function              pdump_cmd_CaptureMemSpaceEnable

******************************************************************************/
IMG_VOID pdump_cmd_CaptureMemSpaceEnable (
	PDUMP_CMDS_psMemSpaceCB			psMemSpaceCB,
    IMG_BOOL                        bEnable,
    IMG_BOOL *                      pbPrevState
)
{
	if(pbPrevState)
		*pbPrevState = psMemSpaceCB->bCaptureEnabled;
	/* Set new state */
	psMemSpaceCB->bCaptureEnabled = bEnable;
}


/*!
******************************************************************************

 @Function              PDUMP_CMD_CaptureMemSpaceEnable

******************************************************************************/
IMG_VOID PDUMP_CMD_CaptureMemSpaceEnable (
    IMG_HANDLE                      hMemSpace,
    IMG_BOOL                        bEnable,
    IMG_BOOL *                      pbPrevState
)
{
	IMG_UINT32 ui32I;
	PDUMP_CMDS_psMemSpaceCB psMemSpaceCB = (PDUMP_CMDS_psMemSpaceCB)hMemSpace;

    /* If we are to enable all...*/
    if (hMemSpace == NULL)
    {
        /* Enable capture for all memory spaces...*/
        for (ui32I=0; ui32I<gui32NoMemSpaces; ui32I++)
        {
			psMemSpaceCB = pdump_cmd_GetMemSpaceCB(ui32I);
			pdump_cmd_CaptureMemSpaceEnable(psMemSpaceCB, bEnable, pbPrevState);
        }
    }
    else
    {
		/* Check memory space Id */
		pdump_cmd_CaptureMemSpaceEnable(psMemSpaceCB, bEnable, pbPrevState);
    }

    return;
}

/*!
******************************************************************************

 @Function              PDUMP_CMD_CaptureStart

******************************************************************************/
IMG_VOID PDUMP_CMD_CaptureStart(IMG_CHAR * psTestDirectory)
{
    IMG_UINT32				ui32I;
	IMG_UINT32				ui32Size;
    IMG_CHAR *              pszCaptureOutTxtFile = IMG_NULL;
    IMG_CHAR *              pszCaptureOutPrmFile = IMG_NULL;
    IMG_CHAR *              pszCaptureOutResFile = IMG_NULL;

	PDUMP_CMDS_psMemSpaceCB		psMemSpaceCB;
	IMG_RESULT result;

	if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_BINLITE))
	{
		PDUMP_CMD_CaptureStartPdumpLite(psTestDirectory);
		return;
	}
	/* Check capture is enabled for at least one memory space...*/
    for (ui32I=0; ui32I < gui32NoMemSpaces; ui32I++)
    {
		psMemSpaceCB = pdump_cmd_GetMemSpaceCB(ui32I);
        if (psMemSpaceCB->bCaptureEnabled)
        {
            break;
        }
    }
    if (ui32I >= gui32NoMemSpaces)
    {
        /* ERROR: TAL -  No memory space enabled for capture */
        printf("ERROR: PDUMP - No memory space enabled for capture\n");
        IMG_ASSERT(IMG_FALSE);
    }

    /* Take a copy of the directory path...*/
    gpszTestDirectory = IMG_STRDUP(psTestDirectory);

	/* If no PDUMP contexts defined...*/
	if (gui32NoPdumpContexts <= 1)
	{
		/* Open the global context...*/
		pdmup_cmd_OpenPdumpContext(&gsPdumpContext);
	}
	else
	{
		/* Open all enabled contexts...*/
		for (ui32I=0; ui32I<gui32NoPdumpContexts; ui32I++)
		{
            if (gasPdumpContexts[ui32I].bPdumpEnabled)
            {
			    pdmup_cmd_OpenPdumpContext(&gasPdumpContexts[ui32I]);
            }
		}
		PDUMP_CMD_GenerateTDF(DEFAULT_TDF_FILE);
	}

	    /* Open files - conditional on the appropriate txt files being enabled. */
    if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP1))
    {
		/* Create filename directory...*/
		ui32Size = (IMG_UINT32)strlen(gpszTestDirectory);
		if ( (ui32Size == 0) || (gpszTestDirectory[ui32Size-1] == DIRECTORY_SEPARATOR_IMG_CHAR) )
		{
			pszCaptureOutTxtFile = IMG_MALLOC(ui32Size + strlen(gsPdump1Context.psOutTxtFileName) + 1);
			sprintf(pszCaptureOutTxtFile,  "%s%s", gpszTestDirectory, gsPdump1Context.psOutTxtFileName);
			pszCaptureOutPrmFile = IMG_MALLOC(ui32Size + strlen(gsPdump1Context.psOutPrmFileName) + 1);
			sprintf(pszCaptureOutPrmFile,  "%s%s", gpszTestDirectory, gsPdump1Context.psOutPrmFileName);
			pszCaptureOutResFile = IMG_MALLOC(ui32Size + strlen(gsPdump1Context.psOutResFileName) + 1);
			sprintf(pszCaptureOutResFile,  "%s%s", gpszTestDirectory, gsPdump1Context.psOutResFileName);
		}
		else
		{
			pszCaptureOutTxtFile = IMG_MALLOC(ui32Size + strlen(gsPdump1Context.psOutTxtFileName) + 2);
			sprintf(pszCaptureOutTxtFile,  "%s" DIRECTORY_SEPARATOR "%s", gpszTestDirectory, gsPdump1Context.psOutTxtFileName);
			pszCaptureOutPrmFile = IMG_MALLOC(ui32Size + strlen(gsPdump1Context.psOutPrmFileName) + 2);
			sprintf(pszCaptureOutPrmFile,  "%s" DIRECTORY_SEPARATOR "%s", gpszTestDirectory, gsPdump1Context.psOutPrmFileName);
			pszCaptureOutResFile = IMG_MALLOC(ui32Size + strlen(gsPdump1Context.psOutResFileName) + 2);
			sprintf(pszCaptureOutResFile,  "%s" DIRECTORY_SEPARATOR "%s", gpszTestDirectory, gsPdump1Context.psOutResFileName);
		}

pdumpAccessLock();

        result = IMG_FILEIO_OpenFile(pszCaptureOutTxtFile, "wb", &gsPdump1Context.hOutTxtFile, GET_ENABLE_COMPRESSION);
		if (result != IMG_SUCCESS)
		{
			printf("ERROR: PDUMP - Failed to open file %s\n", pszCaptureOutTxtFile);
			IMG_ASSERT(IMG_FALSE);
		}
		if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PRM))
		{
			IMG_FILEIO_OpenFile(pszCaptureOutPrmFile, "wb", &gsPdump1Context.hOutPrmFile, GET_ENABLE_COMPRESSION);
			gsPdump1Context.ui32OutPrmFileSize = 0;
		}
		if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_RES))
		{
			IMG_FILEIO_OpenFile(pszCaptureOutResFile, "wb", &gsPdump1Context.hOutResFile,GET_ENABLE_COMPRESSION);
			gsPdump1Context.ui32OutResFileSize = 0;
		}

pdumpAccessUnlock();

		if(pszCaptureOutTxtFile) IMG_FREE(pszCaptureOutTxtFile);
		if(pszCaptureOutPrmFile) IMG_FREE(pszCaptureOutPrmFile);
		if(pszCaptureOutResFile) IMG_FREE(pszCaptureOutResFile);
	}
	gbCapturePdumpLite = IMG_FALSE;
	   
	/* Set capture of any memory space accesses to TRUE */
    gbCaptureEnabled = IMG_TRUE;
}

/*!
******************************************************************************

 @Function              PDUMP_CMD_CaptureStop

******************************************************************************/
IMG_VOID PDUMP_CMD_CaptureStop ()
{
	PDUMP_CMDS_psMemSpaceCB		psMemSpaceCB;
    IMG_UINT32              ui32I;

	/* If capture is enabled...*/
    if (!gbCaptureEnabled)return;

	// Ensure the TDF is up to date
	if (gpszTDFFileName)
	{
		PDUMP_CMD_GenerateTDF(gpszTDFFileName);
		IMG_FREE(gpszTDFFileName);
		gpszTDFFileName = IMG_NULL;
	}

	if (gpszTestDirectory) IMG_FREE(gpszTestDirectory);

 	/* Disable capture for all memory spaces...*/
	for (ui32I=0; ui32I<gui32NoMemSpaces; ui32I++)
	{
		psMemSpaceCB = pdump_cmd_GetMemSpaceCB(ui32I);
		psMemSpaceCB->bCaptureEnabled = IMG_FALSE;
	}

    /* Re-enable capture of all commands */
    gui32DisableFlags = 0x0;

	/* If no PDUMP contexts defined...*/
	if (gui32NoPdumpContexts == 0)
	{
		/* Close the global context...*/
		pdump_cmd_ClosePdumpContext(&gsPdumpContext);
	}
	else
	{
		/* Close all "open" contexts...*/
		for (ui32I=0; ui32I<gui32NoPdumpContexts; ui32I++)
		{
            /* If PDUMPing enabled...*/
            if (gasPdumpContexts[ui32I].bPdumpEnabled)
            {
				pdump_cmd_ClosePdumpContext(&gasPdumpContexts[ui32I]);
            }
            /* Enable PDUMPing...*/
            gasPdumpContexts[ui32I].bPdumpEnabled = IMG_TRUE;
		}
	}
	/* Close Pdump1 context */
	if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP1))
	{
pdumpAccessLock();
		// close default files
		IMG_FILEIO_CloseFile(gsPdump1Context.hOutTxtFile);
		IMG_FILEIO_CloseFile(gsPdump1Context.hOutResFile);
		IMG_FILEIO_CloseFile(gsPdump1Context.hOutPrmFile);
pdumpAccessUnlock();
				
		if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PRM))
		{
			pdump_cmd_StopResPrmFileName( &gsPdump1Context.asCustomPrmFiles );
		}
		if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_RES))
		{
			pdump_cmd_StopResPrmFileName( &gsPdump1Context.asCustomResFiles );
		}
	}

	/* Re-enable all capture flags...*/
	gFlags |= TAL_PDUMP_FLAGS_PDUMP1;
	gFlags |= TAL_PDUMP_FLAGS_PDUMP2;
	gFlags |= TAL_PDUMP_FLAGS_RES;
	gFlags |= TAL_PDUMP_FLAGS_PRM;
	 
	/* Set capture of any memory space accesses to FALSE */
    gbCaptureEnabled = IMG_FALSE;
}


/*!
******************************************************************************

 @Function				PDUMP_CMD_CaptureDisableCmds

******************************************************************************/
IMG_VOID PDUMP_CMD_CaptureDisableCmds(
	IMG_HANDLE						hMemSpace,
	IMG_UINT32						ui32Offset,
	IMG_UINT32						ui32DisableFlags
)
{
    PDUMP_CMDS_psMemSpaceCB         psMemSpaceCB = (PDUMP_CMDS_psMemSpaceCB)hMemSpace;
    IMG_UINT32                      i;

    if (hMemSpace == NULL)
    {
        /* Set disable flags */
        gui32DisableFlags = ui32DisableFlags;
        return;
    }

    if (ui32Offset == TAL_OFFSET_ALL)
    {
        psMemSpaceCB->ui32DisableFlags = ui32DisableFlags;
        return;
    }

    /* Currently only support disabling of WRW and RDW on registers for this...*/
    IMG_ASSERT( (ui32DisableFlags & ~(TAL_DISABLE_CAPTURE_WRW | TAL_DISABLE_CAPTURE_RDW)) == 0); 

    /* Check if the offset has already been disabled...*/
    for (i=0; i<psMemSpaceCB->ui32NoDisabledOffsets; i++)
    {
        if (psMemSpaceCB->aui32DisableOffset[i] == ui32Offset)
        {
            psMemSpaceCB->aui32DisableFlags[i] = ui32DisableFlags;
            return;
        }
    }

    /* Check we can register another offset...*/
    IMG_ASSERT(psMemSpaceCB->ui32NoDisabledOffsets < PDMP_CMDS_MSX_DISABLED_OFFSETS);

    /* Register this offset and flags...*/
    psMemSpaceCB->aui32DisableOffset[psMemSpaceCB->ui32NoDisabledOffsets] = ui32Offset;
    psMemSpaceCB->aui32DisableFlags[psMemSpaceCB->ui32NoDisabledOffsets] = ui32DisableFlags;
    psMemSpaceCB->ui32NoDisabledOffsets++;
}

/*!
******************************************************************************

 @Function              PDUMP_CMD_CaptureStartPdumpLite

******************************************************************************/
IMG_VOID PDUMP_CMD_CaptureStartPdumpLite(
    IMG_CHAR *      pszTestDirectory
)
{
	PDUMP_CMDS_psMemSpaceCB		psMemSpaceCB;
    IMG_UINT32					ui32I;


    /* Check capture is enabled for at least one memory space...*/
    for (ui32I=0; ui32I < gui32NoMemSpaces; ui32I++)
    {
		psMemSpaceCB = pdump_cmd_GetMemSpaceCB(ui32I);
        if (psMemSpaceCB->bCaptureEnabled)
        {
            break;
        }
    }
    if (ui32I >= gui32NoMemSpaces)
    {
        printf("ERROR: PDUMP - No memory space enabled for capture\n");
        IMG_ASSERT(IMG_FALSE);
    }

    /* Take a copy of the directory path...*/
    gpszTestDirectory = IMG_STRDUP(pszTestDirectory);

	/* If no PDUMP contexts defined...*/
	if (gui32NoPdumpContexts == 0)
	{
		/* Open the global context...*/
		pdump_cmd_OpenPdumpBinaryLiteContext(&gsPdumpContext);
	}
	else
	{
		/* Open all enabled contexts...*/
		for (ui32I=0; ui32I<gui32NoPdumpContexts; ui32I++)
		{
            if (gasPdumpContexts[ui32I].bPdumpEnabled)
            {
			    pdump_cmd_OpenPdumpBinaryLiteContext(&gasPdumpContexts[ui32I]);
            }
		}
	}

    /* Set capture of any memory space accesses to TRUE */
    gbCapturePdumpLite	= IMG_TRUE;
	gFlags &= (~TAL_PDUMP_FLAGS_PDUMP1);
    gFlags &= (~TAL_PDUMP_FLAGS_PDUMP2);
	gbCaptureEnabled	= IMG_TRUE;
}

/*!
******************************************************************************

 @Function				PDUMP_CMD_GenerateTDF

******************************************************************************/
IMG_VOID PDUMP_CMD_GenerateTDF(IMG_CHAR * psFilename)
{
	FILE *		fTdf;
	IMG_UINT	i, j;
	IMG_CHAR*	buffer, *tempStr;

  // Keep a temporary store of the TDF filename and then we can rewrite it at the end of the test
  tempStr = IMG_STRDUP(psFilename);
	if (IMG_NULL != gpszTDFFileName) IMG_FREE(gpszTDFFileName);
	gpszTDFFileName = tempStr;
	
	buffer = IMG_MALLOC(strlen(gpszTestDirectory) + strlen(gpszTDFFileName) + 2);

	IMG_ASSERT(buffer);
	if(strcmp(gpszTestDirectory, ""))
	{
		sprintf(buffer, "%s/%s", gpszTestDirectory, gpszTDFFileName);
	}
	else
	{
		sprintf(buffer, "%s", gpszTDFFileName);
	}

	fTdf = fopen (buffer, "wb");
	IMG_FREE(buffer);
	if (fTdf ==	IMG_NULL)
	{
		printf("ERROR: Cannot open TDF file %s for writing\n", gpszTDFFileName);
		IMG_ASSERT(IMG_FALSE);
	}
	fprintf(fTdf, "%s", acTDFComments);
	fprintf(fTdf, "SCRIPTS DEVNAME");
	if (gui32NoPdumpContexts == 0)
	{
		fprintf(fTdf, " %s", gsPdumpContext.psOut2TxtFileName);
	}
	for (j = 0; j < gui32NoPdumpContexts; j++)
	{
		if ( gasPdumpContexts[j].bPdumpEnabled == IMG_TRUE )
		{
			fprintf(fTdf, " %s", gasPdumpContexts[j].psOut2TxtFileName);
		}
	}
	fprintf(fTdf, "\n");

	for (i = 0; i < 32; i++) // sync ID
	{
		IMG_UINT activeCtx = 0;
		fprintf(fTdf, "SYNC DEVNAME %d", i);
		for (j = 0; j < gui32NoPdumpContexts; j++)
		{
			if ( gasPdumpContexts[j].bPdumpEnabled == IMG_TRUE )
			{
				activeCtx++;
				if (gasPdumpContexts[j].ui32ActiveSyncs & (1 << i) )
				{
					fprintf(fTdf, " %d", activeCtx);
				}
			}
		}
		fprintf(fTdf, "\n");
	}

	if (acConfigFile[0] != 0)
	{
		fprintf(fTdf, "CONFIG %s", acConfigFile);
	}
	i = fclose (fTdf);
	IMG_ASSERT (i == 0);
}

/*!
******************************************************************************

 @Function				PDUMP_CMD_AddSyncToTDF

******************************************************************************/
IMG_VOID PDUMP_CMD_AddSyncToTDF (IMG_UINT32 ui32SyncId, IMG_HANDLE hMemSpace)
{
	PDUMP_CMDS_psPdumpContext psPdumpContext;
	PDUMP_CMDS_psMemSpaceCB		psMemSpaceCB = (PDUMP_CMDS_psMemSpaceCB)hMemSpace;

	psPdumpContext = psMemSpaceCB->psPdumpContext;
	psPdumpContext->ui32ActiveSyncs |= (1 << ui32SyncId);
}

/*!
******************************************************************************

 @Function				PDUMP_CMD_AddCommentToTDF

******************************************************************************/
IMG_VOID PDUMP_CMD_AddCommentToTDF(IMG_CHAR * pcComment)
{
	/*Check there is enough room in the comment string */
	IMG_ASSERT ( (strlen(acTDFComments) + strlen(pcComment)) <= MAX_TDF_COMMENT );
	strcat( acTDFComments, "--");
	strcat( acTDFComments, pcComment );
	strcat( acTDFComments, "\n");
}

/*!
******************************************************************************

 @Function				PDUMP_CMD_AddTargetConfigToTDF

******************************************************************************/
IMG_VOID PDUMP_CMD_AddTargetConfigToTDF(IMG_CHAR * pszFilePath)
{
	/*Check there is enough room in the string buffer */
	IMG_ASSERT ( strlen(pszFilePath) <= MAX_CONFIG_LENGTH );
	IMG_MEMCPY(acConfigFile, pszFilePath, strlen(pszFilePath));
	acConfigFile[strlen(pszFilePath)] = 0; // Terminate String
}

/*!
******************************************************************************

 @Function              PDUMP_CMD_SetPdumpConversionSourceData

******************************************************************************/
IMG_VOID PDUMP_CMD_SetPdumpConversionSourceData(
    IMG_UINT32				ui32SourceFileID,
    IMG_UINT32				ui32SourceLineNum
)
{
	/* Set the global fileid and linenum variables */
	gui32SourceFileNo = ui32SourceFileID;
	gui32SourceFileLineNo = ui32SourceLineNum;
}

/*!
******************************************************************************

 @Function				PDUMP_CMD_SetBaseAddress

******************************************************************************/
IMG_VOID PDUMP_CMD_SetBaseAddress (
	IMG_HANDLE					hMemSpace,
	IMG_UINT64                  ui64BaseAddr
)
{
	PDUMP_CMDS_psMemSpaceCB		psMemSpaceCB = (PDUMP_CMDS_psMemSpaceCB)hMemSpace;
	psMemSpaceCB->ui64BaseAddress = ui64BaseAddr;
}

/*!
******************************************************************************

 @Function				PDUMP_CMD_SetPdump1MemName

******************************************************************************/
IMG_VOID PDUMP_CMD_SetPdump1MemName (
	IMG_CHAR*					pszMemName
)
{
	if (pszMemName == IMG_NULL)
	{
		gsPdump1Context.pszMemName = IMG_NULL;
	}
	else
	{
		gsPdump1Context.pszMemName = IMG_STRDUP(pszMemName);
	}
}

/*!
******************************************************************************

 @Function              PDUMP_CMD_ChangePdumpContext1

******************************************************************************/
IMG_VOID PDUMP_CMD_ChangePdumpContext1 (
    IMG_UINT32                      ui32MemSpaceId,
    IMG_CHAR *                      pszContextName,
    IMG_UINT32                      ui32FileNameLength
)
{
	PDUMP_CMDS_psPdumpContext psPdumpContext = pdump_cmd_GetContext(pszContextName);
	PDUMP_CMDS_psMemSpaceCB		psMemSpaceCB = pdump_cmd_GetMemSpaceCB(ui32MemSpaceId);
	(void)ui32FileNameLength;
	psMemSpaceCB->psPdumpContext = psPdumpContext;
}


/*!
******************************************************************************

 @Function              PDUMP_CMD_CaptureSetFilename

******************************************************************************/
IMG_VOID PDUMP_CMD_CaptureSetFilename (
    IMG_HANDLE                      hMemSpace,
	TAL_eSetFilename		        eSetFilename,
	IMG_CHAR *				        psFileName
)
{
    PDUMP_CMDS_psPdumpContext psPdumpContext;
	PDUMP_CMDS_psMemSpaceCB		psMemSpaceCB = (PDUMP_CMDS_psMemSpaceCB)hMemSpace;
	
    /* If the id is not PDUMP_MEMSPACE_UNDEFINED...*/
    if (hMemSpace != NULL)
    {
        psPdumpContext = psMemSpaceCB->psPdumpContext;
    }
    else
    {
        /* Ensure that we are using the global context - otherwise we need to have a memory space to
           obtain the PDUMP context...*/
        IMG_ASSERT(gui32NoPdumpContexts == 0);
        psPdumpContext = &gsPdumpContext;
    }

    /* Branch on file type...*/
	switch(eSetFilename)
	{
	case TAL_FILENAME_OUT_TXT:			/*!	Set out.txt filename				*/
        IMG_FREE(gsPdump1Context.psOutTxtFileName);
		gsPdump1Context.psOutTxtFileName = IMG_STRDUP(psFileName);
		break;

	case TAL_FILENAME_OUT_RES:			/*!	Set out.res filename				*/
        IMG_FREE(gsPdump1Context.asCustomResFiles.apszNames[0]);
		gsPdump1Context.asCustomResFiles.apszNames[0] = IMG_STRDUP(psFileName);
		gsPdump1Context.psOutResFileName = gsPdump1Context.asCustomResFiles.apszNames[0];
		break;

	case TAL_FILENAME_OUT_PRM:			/*!	Set out.prn filename				*/
		IMG_FREE(gsPdump1Context.asCustomPrmFiles.apszNames[0]);
		gsPdump1Context.asCustomPrmFiles.apszNames[0] = IMG_STRDUP(psFileName);
		gsPdump1Context.psOutPrmFileName = gsPdump1Context.asCustomPrmFiles.apszNames[0];
		break;

	case TAL_FILENAME_OUT2_TXT:			/*!	Set out2.txt filename				*/
        IMG_FREE(psPdumpContext->psOut2TxtFileName);
		psPdumpContext->psOut2TxtFileName = IMG_STRDUP(psFileName);
		break;

	case TAL_FILENAME_OUT2_RES:			/*!	Set out2.res filename				*/
        IMG_FREE(psPdumpContext->asCustomResFiles.apszNames[0]);
		psPdumpContext->asCustomResFiles.apszNames[0] = IMG_STRDUP(psFileName);
		psPdumpContext->psOut2ResFileName = psPdumpContext->asCustomResFiles.apszNames[0];
		break;

	case TAL_FILENAME_OUT2_PRM:			/*!	Set out2.prn filename				*/
        IMG_FREE(psPdumpContext->asCustomPrmFiles.apszNames[0]);
		psPdumpContext->asCustomPrmFiles.apszNames[0] = IMG_STRDUP(psFileName);
		psPdumpContext->psOut2PrmFileName = psPdumpContext->asCustomPrmFiles.apszNames[0];
		break;

	case TAL_FILENAME_OUTL_BIN:			/*!	Set outL.bin filename				*/
        IMG_FREE(psPdumpContext->psOutLiteBinFileName);
		psPdumpContext->psOutLiteBinFileName = IMG_STRDUP(psFileName);
		break;

	}
}

/*!
******************************************************************************

 @Function             PDUMP_CMD_CaptureSetFileoffset

******************************************************************************/
IMG_VOID PDUMP_CMD_CaptureSetFileoffset (
    IMG_HANDLE                      hMemSpace,
	TAL_eSetFilename		        eSetFilename,
	IMG_UINT32				        ui32FileOffset
)
{
	PDUMP_CMDS_psPdumpContext psPdumpContext;
	PDUMP_CMDS_psMemSpaceCB		psMemSpaceCB = (PDUMP_CMDS_psMemSpaceCB)hMemSpace;

    if (hMemSpace != NULL)
    {
        psPdumpContext = psMemSpaceCB->psPdumpContext;
    }
    else
    {
        /* Ensure that we are using the global context - otherwise we need to have a memory space to
           obtain the PDUMP context...*/
        IMG_ASSERT(gui32NoPdumpContexts == 0);
        psPdumpContext = &gsPdumpContext;
    }

	/* Branch on file type...*/
    switch(eSetFilename)
	{

	case TAL_FILENAME_OUT_RES:			/*!	Set out.res offset				*/
		gsPdump1Context.ui32OutResFileSize = ui32FileOffset;
		break;

	case TAL_FILENAME_OUT_PRM:			/*!	Set out.prm offset				*/
		gsPdump1Context.ui32OutPrmFileSize = ui32FileOffset;
		break;

	case TAL_FILENAME_OUT2_RES:			/*!	Set out2.res offset				*/
		psPdumpContext->ui32Out2ResFileSize = ui32FileOffset;
		break;

	case TAL_FILENAME_OUT2_PRM:			/*!	Set out2.prm offset				*/
		psPdumpContext->ui32Out2PrmFileSize = ui32FileOffset;
		break;

	default:
		IMG_ASSERT(IMG_FALSE);
	}
}

/*!
******************************************************************************

 @Function				PDUMP_CMD_ClearPdumpFlags

******************************************************************************/

IMG_VOID PDUMP_CMD_ClearFlags(IMG_UINT32 ui32Flags)
{
	gFlags &= (~ui32Flags);
}

/*!
******************************************************************************

 @Function				PDUMP_CMD_SetPdumpFlags

******************************************************************************/

IMG_VOID PDUMP_CMD_SetFlags(IMG_UINT32 ui32Flags)
{
	gFlags |= ui32Flags;
}


/*!
******************************************************************************

 @Function				PDUMP_CMD_GetFlags

******************************************************************************/

IMG_UINT32 PDUMP_CMD_GetFlags()
{
	/* Cannot call this function in kernel mode */
#ifdef IMG_KERNEL_MODULE
	IMG_ASSERT(IMG_FALSE);
	return IMG_NULL;
#endif

	return gFlags;
}



/*!
******************************************************************************

 @Function              PDUMP_CMD_CaptureEnableOutputFormats

******************************************************************************/
IMG_VOID PDUMP_CMD_CaptureEnableOutputFormats (
    IMG_BOOL            bEnablePDUMP1,
    IMG_BOOL            bEnablePDUMP2
)
{
    // Clear flags then set accordingly
	gFlags &= ~(TAL_PDUMP_FLAGS_PDUMP1 | TAL_PDUMP_FLAGS_PDUMP2);
	gFlags |= bEnablePDUMP1 ? TAL_PDUMP_FLAGS_PDUMP1 : 0;
    gFlags |= bEnablePDUMP2 ? TAL_PDUMP_FLAGS_PDUMP2 : 0;
}


/*!
******************************************************************************

 @Function              PDUMP_CMD_CaptureEnableResAndPrm

******************************************************************************/
IMG_VOID PDUMP_CMD_CaptureEnableResAndPrm (
	IMG_BOOL			bRes,
	IMG_BOOL			bPrm
)
{
    // Clear flags then set accordingly
	gFlags &= ~(TAL_PDUMP_FLAGS_RES | TAL_PDUMP_FLAGS_PRM);
	gFlags |= bRes ? TAL_PDUMP_FLAGS_RES : 0;
    gFlags |= bPrm ? TAL_PDUMP_FLAGS_PRM : 0;
}

/*!
******************************************************************************

 @Function				PDUMP_CMD_CaptureEnablePdumpContext

******************************************************************************/
IMG_VOID PDUMP_CMD_CaptureEnablePdumpContext (
	const IMG_CHAR *	    pszContextName
)
{
    IMG_UINT32				ui32I;

    IMG_ASSERT(gui32NoPdumpContexts != 0);
	/* For all contexts...*/
	for (ui32I=0; ui32I<gui32NoPdumpContexts; ui32I++)
	{
        /* If this context...*/
        if (strcmp(pszContextName, gasPdumpContexts[ui32I].pszContextName) == 0)
        {
            /* Enable PDUMPing...*/
            gasPdumpContexts[ui32I].bPdumpEnabled = IMG_TRUE;
        }
        else
        {
            /* Disable PDUMPing...*/
            gasPdumpContexts[ui32I].bPdumpEnabled = IMG_FALSE;
        }
	}
}

/*
******************************************************************************

 @Function				PDUMP_CMD_CaptureDisablePdumpContext

******************************************************************************/
IMG_VOID PDUMP_CMD_CaptureDisablePdumpContext (IMG_CONST IMG_CHAR *pszContextName)
{
	IMG_UINT32 ui32I;
    IMG_ASSERT(gui32NoPdumpContexts != 0);

	for (ui32I=0; ui32I<gui32NoPdumpContexts; ui32I++)
	{
		if (IMG_STRCMP(pszContextName, gasPdumpContexts[ui32I].pszContextName) == 0)
        {
            gasPdumpContexts[ui32I].bPdumpEnabled = IMG_FALSE;
			break;
        }
	}
}

/*!
******************************************************************************

 @Function              PDUMP_CMD_PdumpComment

******************************************************************************/
IMG_VOID PDUMP_CMD_PdumpComment(
    IMG_HANDLE                      hMemSpace,
    IMG_CONST IMG_CHAR *            psCommentText
)
{
	PDUMP_CMDS_psMemSpaceCB		psMemSpaceCB = (PDUMP_CMDS_psMemSpaceCB)hMemSpace;
	PDUMP_CMDS_psPdumpContext	psPdumpContext = pdump_cmd_CheckContext(psMemSpaceCB);
	

	/* Check to see if we should be capturing this */
	if (psMemSpaceCB != NULL && psMemSpaceCB->bCaptureEnabled != IMG_TRUE)
		return;

	if ( (gbCaptureEnabled != IMG_TRUE)
		|| ((gui32DisableFlags & TAL_DISABLE_CAPTURE_COMMENT) != 0) )
		return;


pdumpAccessLock();
    if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP2))			/* Pdump2 */
    {
        /* Output to PDUMP file ...*/
        IMG_FILEIO_WriteToFile(psPdumpContext->hOut2TxtFile, "-- ", 3);
        IMG_FILEIO_WriteToFile(psPdumpContext->hOut2TxtFile, psCommentText, (IMG_UINT32)strlen(psCommentText));
        IMG_FILEIO_WriteToFile(psPdumpContext->hOut2TxtFile, "\n", 1);
		IMG_FILEIO_Flush(psPdumpContext->hOut2TxtFile);
    }
    if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP1))			/* Pdump1 */
    {
        /* Output to PDUMP file ...*/
        IMG_FILEIO_WriteToFile(gsPdump1Context.hOutTxtFile, "-- ", 3);
        IMG_FILEIO_WriteToFile(gsPdump1Context.hOutTxtFile, psCommentText, (IMG_UINT32)strlen(psCommentText));
        IMG_FILEIO_WriteToFile(gsPdump1Context.hOutTxtFile, "\n", 1);
		IMG_FILEIO_Flush(gsPdump1Context.hOutTxtFile);
    }
pdumpAccessUnlock();
}

/*!
******************************************************************************

 @Function              PDUMP_CMD_PdumpConsoleMessage

******************************************************************************/
IMG_VOID PDUMP_CMD_PdumpConsoleMessage(
    IMG_HANDLE                      hMemSpace,
    IMG_CHAR *                      psCommentText
)
{
	PDUMP_CMDS_psMemSpaceCB		psMemSpaceCB = (PDUMP_CMDS_psMemSpaceCB)hMemSpace;
	PDUMP_CMDS_psPdumpContext	psPdumpContext = pdump_cmd_CheckContext(psMemSpaceCB);
	

	/* Check to see if we should be capturing this */
	if (psMemSpaceCB != NULL && psMemSpaceCB->bCaptureEnabled != IMG_TRUE)
		return;

	if ( (gbCaptureEnabled != IMG_TRUE)
		|| ((gui32DisableFlags & TAL_DISABLE_CAPTURE_COMMENT) != 0) )
		return;

pdumpAccessLock();
    if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP2))			/* Pdump2 */
    {
        /* Output to PDUMP file ...*/
        IMG_FILEIO_WriteToFile(psPdumpContext->hOut2TxtFile, "CON \"", 5);
        IMG_FILEIO_WriteToFile(psPdumpContext->hOut2TxtFile, psCommentText, (IMG_UINT32)strlen(psCommentText));
        IMG_FILEIO_WriteToFile(psPdumpContext->hOut2TxtFile, "\"\n", 2);
    }
    if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP1))			/* Pdump1 */
    {
        /* Output to PDUMP file ...*/
        IMG_FILEIO_WriteToFile(gsPdump1Context.hOutTxtFile, "COM \"", 5);
        IMG_FILEIO_WriteToFile(gsPdump1Context.hOutTxtFile, psCommentText, (IMG_UINT32)strlen(psCommentText));
        IMG_FILEIO_WriteToFile(gsPdump1Context.hOutTxtFile, "\"\n", 2);
	}
pdumpAccessUnlock();
}

/*!
******************************************************************************

 @Function              PDUMP_CMD_PdumpMiscOutput

******************************************************************************/
IMG_VOID PDUMP_CMD_PdumpMiscOutput(
    IMG_HANDLE                      hMemSpace,
    IMG_CHAR *                      psCommentText
)
{
	PDUMP_CMDS_psMemSpaceCB		psMemSpaceCB = (PDUMP_CMDS_psMemSpaceCB)hMemSpace;
	PDUMP_CMDS_psPdumpContext	psPdumpContext = pdump_cmd_CheckContext(psMemSpaceCB);
	
	/* Check to see if we should be capturing this */
	if (psMemSpaceCB != NULL && psMemSpaceCB->bCaptureEnabled != IMG_TRUE)
		return;

	if ( (gbCaptureEnabled != IMG_TRUE)
		|| ((gui32DisableFlags & TAL_DISABLE_CAPTURE_COMMENT) != 0) )
		return;

pdumpAccessLock();
    if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP2))			/* Pdump2 */
    {
        /* Output to PDUMP file ...*/
        IMG_FILEIO_WriteToFile(psPdumpContext->hOut2TxtFile, psCommentText, (IMG_UINT32)strlen(psCommentText));
        IMG_FILEIO_WriteToFile(psPdumpContext->hOut2TxtFile, "\n", 1);
    }
    if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP1))			/* Pdump1 */
    {
        /* Output to PDUMP file ...*/
        IMG_FILEIO_WriteToFile(gsPdump1Context.hOutTxtFile, psCommentText, (IMG_UINT32)strlen(psCommentText));
        IMG_FILEIO_WriteToFile(gsPdump1Context.hOutTxtFile, "\n", 1);
	}
pdumpAccessUnlock();
	if (gbCapturePdumpLite)			/* Bin Lite */
	{
		IMG_ASSERT(IMG_FALSE);
	}
}

/*!
******************************************************************************

 @Function              PDUMP_CMD_Sync

******************************************************************************/
IMG_VOID PDUMP_CMD_Sync(
    IMG_HANDLE  hMemSpace,
	IMG_UINT32	ui32SyncId
)
{
	PDUMP_CMDS_psMemSpaceCB		psMemSpaceCB = (PDUMP_CMDS_psMemSpaceCB)hMemSpace;
	PDUMP_CMDS_psPdumpContext	psPdumpContext = pdump_cmd_CheckContext(psMemSpaceCB);
	IMG_CHAR					cBuffer[100];

	/* Check to see if we should be capturing this */
	if ( gbCaptureEnabled != IMG_TRUE ) return;
	if ( psPdumpContext == &gsPdumpContext ) return;

	if (psMemSpaceCB != NULL && psMemSpaceCB->bCaptureEnabled != IMG_TRUE)
		return;

    if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP2))			/* Pdump2 */
    {
		if (ui32SyncId == TAL_NO_SYNC_ID)
		{
			sprintf(cBuffer, "SYNC\n");
		}
		else
		{
			IMG_ASSERT(ui32SyncId > 0);
			IMG_ASSERT(ui32SyncId <= 63);
			sprintf(cBuffer, "SYNC %d\n", ui32SyncId);
		}

pdumpAccessLock();
		IMG_FILEIO_WriteToFile(psPdumpContext->hOut2TxtFile, cBuffer, (IMG_UINT32)strlen(cBuffer));
pdumpAccessUnlock();
    }
    if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP1))			/* Pdump1 */
    {
		/* Nothing to do */
	}
	if (gbCapturePdumpLite)			/* Bin Lite */
	{
pdumpAccessLock();
		IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, TAL_PDUMPL_SYNC);
		IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, gui32SourceFileNo);
		IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, gui32SourceFileLineNo);
		IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, ui32SyncId);
		psPdumpContext->ui32OutLiteBinFiletSizeInDWords += 4;
pdumpAccessUnlock();
	}
}

/*!
******************************************************************************

 @Function              PDUMP_CMD_Lock

******************************************************************************/
IMG_VOID PDUMP_CMD_Lock(
    IMG_HANDLE  hMemSpace,
	IMG_UINT32	ui32SemaId
)
{
	PDUMP_CMDS_psMemSpaceCB		psMemSpaceCB = (PDUMP_CMDS_psMemSpaceCB)hMemSpace;
	PDUMP_CMDS_psPdumpContext	psPdumpContext = pdump_cmd_CheckContext(psMemSpaceCB);
	IMG_CHAR					cBuffer[100];

	/* Check to see if we should be capturing this */
	if ( gbCaptureEnabled != IMG_TRUE ) return;
	if ( psPdumpContext == &gsPdumpContext ) return;

	if (psMemSpaceCB != NULL && psMemSpaceCB->bCaptureEnabled != IMG_TRUE)
		return;

    if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP2))			/* Pdump2 */
    {
		sprintf(cBuffer, "LOCK %d\n", ui32SemaId);
pdumpAccessLock();
		IMG_FILEIO_WriteToFile(psPdumpContext->hOut2TxtFile, cBuffer, (IMG_UINT32)strlen(cBuffer));
pdumpAccessUnlock();
    }
    if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP1))			/* Pdump1 */
    {
		/*** No point in write this to the out.txt file ***/
		printf("Lock Command Not Supported in Pdump1, Multi Context Pdump cannot be converted\n");
		IMG_ASSERT(IMG_FALSE);
	}
	if (gbCapturePdumpLite)			/* Bin Lite */
	{
pdumpAccessLock();
		IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, TAL_PDUMPL_LOCK);
		IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, gui32SourceFileNo);
		IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, gui32SourceFileLineNo);
		IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, ui32SemaId);
		psPdumpContext->ui32OutLiteBinFiletSizeInDWords += 4;
pdumpAccessUnlock();
	}
}

/*!
******************************************************************************

 @Function              PDUMP_CMD_Unlock

******************************************************************************/
IMG_VOID PDUMP_CMD_Unlock(
    IMG_HANDLE  hMemSpace,
	IMG_UINT32	ui32SemaId
)
{
	PDUMP_CMDS_psMemSpaceCB		psMemSpaceCB = (PDUMP_CMDS_psMemSpaceCB)hMemSpace;
	PDUMP_CMDS_psPdumpContext	psPdumpContext = pdump_cmd_CheckContext(psMemSpaceCB);
	IMG_CHAR					cBuffer[100];

	/* Check to see if we should be capturing this */
	if ( gbCaptureEnabled != IMG_TRUE ) return;
	if ( psPdumpContext == &gsPdumpContext ) return;
	
	if (psMemSpaceCB != NULL && psMemSpaceCB->bCaptureEnabled != IMG_TRUE)
		return;

    if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP2))			/* Pdump2 */
    {
pdumpAccessLock();
		sprintf(cBuffer, "UNLOCK %d\n", ui32SemaId);
		IMG_FILEIO_WriteToFile(psPdumpContext->hOut2TxtFile, cBuffer, (IMG_UINT32)strlen(cBuffer));
pdumpAccessUnlock();
    }
    if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP1))			/* Pdump1 */
    {
		/*** No point in write this to the out.txt file ***/
		printf("Unlock Command Not Supported in Pdump1, Multi Context Pdump cannot be converted\n");
    	IMG_ASSERT(IMG_FALSE);
	}
	if (gbCapturePdumpLite)			/* Bin Lite */
	{
pdumpAccessLock();
		IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, TAL_PDUMPL_UNLOCK);
		IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, gui32SourceFileNo);
		IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, gui32SourceFileLineNo);
		IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, ui32SemaId);
		psPdumpContext->ui32OutLiteBinFiletSizeInDWords += 4;
pdumpAccessUnlock();
	}
}

/*!
******************************************************************************

 @Function              PDUMP_CMD_If

******************************************************************************/
IMG_VOID PDUMP_CMD_If(
    IMG_HANDLE  hMemSpace,
	IMG_CHAR *	pszDefine
)
{
	PDUMP_CMDS_psMemSpaceCB		psMemSpaceCB = (PDUMP_CMDS_psMemSpaceCB)hMemSpace;
	PDUMP_CMDS_psPdumpContext	psPdumpContext = pdump_cmd_CheckContext(psMemSpaceCB);
	IMG_CHAR					cBuffer[100];

	/* Check to see if we should be capturing this */
	if ( gbCaptureEnabled != IMG_TRUE ) return;
	
	if (psMemSpaceCB != NULL && psMemSpaceCB->bCaptureEnabled != IMG_TRUE)
		return;

    if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP2))			/* Pdump2 */
    {
		sprintf(cBuffer, "IF %s\n", pszDefine);
pdumpAccessLock();
		IMG_FILEIO_WriteToFile(psPdumpContext->hOut2TxtFile, cBuffer, (IMG_UINT32)strlen(cBuffer));
pdumpAccessUnlock();
    }
}

/*!
******************************************************************************

 @Function              PDUMP_CMD_Else

******************************************************************************/
IMG_VOID PDUMP_CMD_Else(
    IMG_HANDLE  hMemSpace
)
{
	PDUMP_CMDS_psMemSpaceCB		psMemSpaceCB = (PDUMP_CMDS_psMemSpaceCB)hMemSpace;
	PDUMP_CMDS_psPdumpContext	psPdumpContext = pdump_cmd_CheckContext(psMemSpaceCB);
	IMG_CHAR					cBuffer[100];

	/* Check to see if we should be capturing this */
	if ( gbCaptureEnabled != IMG_TRUE ) return;
	
	if (psMemSpaceCB != NULL && psMemSpaceCB->bCaptureEnabled != IMG_TRUE)
		return;

    if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP2))			/* Pdump2 */
    {
		sprintf(cBuffer, "ELSE\n");
pdumpAccessLock();
		IMG_FILEIO_WriteToFile(psPdumpContext->hOut2TxtFile, cBuffer, (IMG_UINT32)strlen(cBuffer));
pdumpAccessUnlock();
    }
}

/*!
******************************************************************************

 @Function              PDUMP_CMD_Endif

******************************************************************************/
IMG_VOID PDUMP_CMD_Endif(
    IMG_HANDLE  hMemSpace
)
{
	PDUMP_CMDS_psMemSpaceCB		psMemSpaceCB = (PDUMP_CMDS_psMemSpaceCB)hMemSpace;
	PDUMP_CMDS_psPdumpContext	psPdumpContext = pdump_cmd_CheckContext(psMemSpaceCB);
	IMG_CHAR					cBuffer[100];

	/* Check to see if we should be capturing this */
	if ( gbCaptureEnabled != IMG_TRUE ) return;
	
	if (psMemSpaceCB != NULL && psMemSpaceCB->bCaptureEnabled != IMG_TRUE)
		return;

    if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP2))			/* Pdump2 */
    {
		sprintf(cBuffer, "FI\n");

pdumpAccessLock();
		IMG_FILEIO_WriteToFile(psPdumpContext->hOut2TxtFile, cBuffer, (IMG_UINT32)strlen(cBuffer));
pdumpAccessUnlock();
    }
}

/*!
******************************************************************************

 @Function              PDUMP_CMD_Idle

******************************************************************************/
IMG_VOID PDUMP_CMD_Idle(
	PDUMP_CMD_sTarget		stTarget
)
{
	PDUMP_CMDS_psMemSpaceCB		psMemSpaceCB = (PDUMP_CMDS_psMemSpaceCB)stTarget.hMemSpace;
	PDUMP_CMDS_psPdumpContext	psPdumpContext = pdump_cmd_CheckContext(psMemSpaceCB);
	IMG_CHAR					cBuffer[100];


		/* Check to see if we should be capturing this */
	if ( (gbCaptureEnabled != IMG_TRUE) || ((gui32DisableFlags & TAL_DISABLE_CAPTURE_IDL) != 0) ) return;
	if (psMemSpaceCB != NULL && psMemSpaceCB->bCaptureEnabled != IMG_TRUE)
		return;

	if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP2))			/* Pdump2 */
	{
		if(stTarget.hMemSpace != NULL)
		{
			psPdumpContext = psMemSpaceCB->psPdumpContext;
			sprintf(cBuffer, "IDL :%s %" IMG_I64PR "d\n", psMemSpaceCB->pszMemSpaceName, stTarget.ui64Value);
		}	
		else
		{
			psMemSpaceCB = pdump_cmd_GetMemSpaceCB(0);
			psPdumpContext = psMemSpaceCB->psPdumpContext;
			sprintf(cBuffer, "IDL %" IMG_I64PR "d\n", stTarget.ui64Value);
        }

pdumpAccessLock();
        IMG_FILEIO_WriteToFile(psPdumpContext->hOut2TxtFile, cBuffer, (IMG_UINT32)strlen(cBuffer));
pdumpAccessUnlock();
	}
	if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP1))			/* Pdump1 */
	{
		sprintf(cBuffer, "IDL %08X\n", IMG_UINT64_TO_UINT32(stTarget.ui64Value));
pdumpAccessLock();
		IMG_FILEIO_WriteToFile(gsPdump1Context.hOutTxtFile, cBuffer, (IMG_UINT32)strlen(cBuffer));
pdumpAccessUnlock();
	}
	if (gbCapturePdumpLite)			/* Bin Lite */
	{
		if(stTarget.hMemSpace == NULL)
		{
			psMemSpaceCB = pdump_cmd_GetMemSpaceCB(0);
		}
		psPdumpContext = psMemSpaceCB->psPdumpContext;

pdumpAccessLock();
		IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, TAL_PDUMPL_IDL);
		IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, gui32SourceFileNo);
		IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, gui32SourceFileLineNo);
		IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, IMG_UINT64_TO_UINT32(stTarget.ui64Value));
		psPdumpContext->ui32OutLiteBinFiletSizeInDWords += 4;
pdumpAccessUnlock();
	}

}

/*!
******************************************************************************

 @Function              pdump_cmds_RecordReadMemory

******************************************************************************/
IMG_RESULT pdump_cmds_RecordReadMemory(
    IMG_UINT64              ui64MemoryAddress,
    IMG_UINT32              ui32Size,
    IMG_UINT8 *             pui8Buffer
)
{

	IMG_CHAR					cBuffer[100], caSrc[50];
	PDUMP_CMD_sTarget			stSrc;
	PDUMP_CMDS_psPdumpContext	psPdumpContext = pdump_cmd_CheckContext(gsPdump1Context.psTempMemspace);

	stSrc.eMemSpT = PDUMP_MEMSPACE_MEMORY;
	stSrc.ui64BlockAddr = ui64MemoryAddress;
	stSrc.ui32BlockId = TAL_NO_MEM_BLOCK;
	stSrc.hMemSpace = gsPdump1Context.psTempMemspace;
	stSrc.ui64Value = 0;


	if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP1))
	{
		pdmup_cmd_MakePdump1(caSrc, stSrc);
        
        if (pui8Buffer == 0)
		{
			sprintf(cBuffer, "-- SAB v%s %08X %s -- offset 0x%08X (page fault)\n", caSrc,
					ui32Size, gsPdump1Context.psOutResFileName,
                    gsPdump1Context.ui32OutResFileSize);
		}
		else
		{
			sprintf(cBuffer, "SAB %s %08X %s -- offset 0x%08X\n", caSrc,
					ui32Size, gsPdump1Context.psOutResFileName,
                    gsPdump1Context.ui32OutResFileSize);
        }
		/* Output to PDUMP file ...*/

		pdumpAccessLock();
		IMG_FILEIO_WriteToFile(gsPdump1Context.hOutTxtFile, cBuffer, (IMG_UINT32)strlen(cBuffer));
		pdumpAccessUnlock();

		if (!GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_RES))
			gsPdump1Context.ui32OutResFileSize += ui32Size;
        
		return 0;
	}
    
	if (gbCapturePdumpLite)
	{
		if (pui8Buffer == 0)
            return 1;
        pdumpAccessLock();
		if ((ui64MemoryAddress >> 32) > 0)
		{
			IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, TAL_PDUMPL_SAB_64);
		}
		else
		{
			IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, TAL_PDUMPL_SAB);
		}

		IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, gui32SourceFileNo);
		IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, gui32SourceFileLineNo);
        
		if ((ui64MemoryAddress >> 32) > 0)
		{
			IMG_FILEIO_Write64BitWordToFile(psPdumpContext->hOutLiteBinFile, ui64MemoryAddress);
			psPdumpContext->ui32OutLiteBinFiletSizeInDWords ++;
		}
		else
			IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, IMG_UINT64_TO_UINT32(ui64MemoryAddress));
        
		IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, ui32Size);
		psPdumpContext->ui32OutLiteBinFiletSizeInDWords += 5;
		pdumpAccessUnlock();

		return 0;
	}
    
	return 1;
}

/*!
******************************************************************************

 @Function              pdump_cmd_VirtualSaveBytesBreakUp

******************************************************************************/
IMG_RESULT pdump_cmd_VirtualSaveBytesBreakUp(
	PDUMP_CMD_sTarget	stSrc,
	IMG_PUINT8			pui8Values,
	IMG_UINT32			ui32Size )
{
	IMG_ASSERT( stSrc.eMemSpT == PDUMP_MEMSPACE_VIRTUAL );
	if (stSrc.ui32BlockId == TAL_DEV_VIRT_QUALIFIER_NONE)
		return IMG_ERROR_INVALID_ID;
	MMU_PageReadCallback (stSrc.ui32BlockId, pdump_cmds_RecordReadMemory);
	gsPdump1Context.psTempMemspace = stSrc.hMemSpace;

	return MMU_ReadVmem (stSrc.ui64Value ,ui32Size ,(IMG_PUINT32)pui8Values , stSrc.ui32BlockId);
}

/*!
******************************************************************************

 @Function              pdump_cmd_SaveBytes

******************************************************************************/
IMG_VOID pdump_cmd_SaveBytes(
	PDUMP_CMD_sTarget	stSrc,
	IMG_PUINT8			pui8Values,
	IMG_UINT32			ui32Size
)	
{
	PDUMP_CMDS_psMemSpaceCB		psMemSpaceCB = (PDUMP_CMDS_psMemSpaceCB)stSrc.hMemSpace;
	PDUMP_CMDS_psPdumpContext	psPdumpContext = pdump_cmd_CheckContext(psMemSpaceCB);
	PDUMP_CMDS_psMemBlock		psMemBlock = IMG_NULL;
	IMG_CHAR					cBuffer[200], caSrc[50];

	switch (stSrc.eMemSpT)
	{
	case PDUMP_MEMSPACE_MEMORY:
	case PDUMP_MEMSPACE_VIRTUAL:
	case PDUMP_MEMSPACE_REGISTER:
		break;
	default:
		printf("PDUMP_CMD ERROR : Incompatible Memspace %s Type in SAB Output\n", psMemSpaceCB->pszMemSpaceName);
		IMG_ASSERT(IMG_FALSE);
		break;
	}

	if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP2))			/* Pdump2 */
    {
        /* Check if the file needs to be split...*/
        pdump_cmd_CheckSlitRequired(&psPdumpContext->psOut2ResFileName, &psPdumpContext->hOut2ResFile, &psPdumpContext->ui32Out2ResFileSize, ui32Size);

        /* Create PDUMP and write command...*/
		if (stSrc.ui32BlockId != TAL_NO_MEM_BLOCK)
		{
			psMemBlock = pdmup_cmd_getMemBlock(psMemSpaceCB, stSrc.ui32BlockId);

			if (ui32Size == psMemBlock->ui32Size)
			{
				pdmup_cmd_MakePdump2(caSrc, stSrc, IMG_TRUE);
				sprintf(cBuffer, "SAB %s %d %s\n", 
					caSrc, psPdumpContext->ui32Out2ResFileSize, psPdumpContext->psOut2ResFileName);
			}
			else
			{
				pdmup_cmd_MakePdump2(caSrc, stSrc, IMG_FALSE);
				sprintf(cBuffer, "SAB %s %d %d %s\n", 
					caSrc, ui32Size, psPdumpContext->ui32Out2ResFileSize, psPdumpContext->psOut2ResFileName);

			}
		}
		else
		{
			pdmup_cmd_MakePdump2(caSrc, stSrc, IMG_FALSE);
			sprintf(cBuffer, "SAB %s 0x%X %d %s\n", 
				caSrc, ui32Size, psPdumpContext->ui32Out2ResFileSize, psPdumpContext->psOut2ResFileName);

		}

pdumpAccessLock();
        IMG_FILEIO_WriteToFile(psPdumpContext->hOut2TxtFile, cBuffer, (IMG_UINT32)strlen(cBuffer));
pdumpAccessUnlock();
		//psPdumpContext->ui32Out2ResFileSize += ui32Size;
    }
    if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP1) || gbCapturePdumpLite)			/* Pdump1 or Binary Lite */
    {
		IMG_RESULT rResult = IMG_ERROR_NOT_INITIALISED;
        /* Check if the file needs to be split...*/
        pdump_cmd_CheckSlitRequired(&gsPdump1Context.psOutResFileName, &gsPdump1Context.hOutResFile, &gsPdump1Context.ui32OutResFileSize, ui32Size);
		
		if (stSrc.eMemSpT == PDUMP_MEMSPACE_VIRTUAL)
		{
			rResult = pdump_cmd_VirtualSaveBytesBreakUp(stSrc, pui8Values, ui32Size);
		}
		switch(rResult)
		{
		case IMG_SUCCESS:
			break;
		case IMG_ERROR_NOT_INITIALISED:
		case IMG_ERROR_INVALID_ID:
			if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP1))			/* Pdump 1 */
			{
				pdmup_cmd_MakePdump1(caSrc, stSrc);
				sprintf(cBuffer, "SAB %s %08X %s -- offset 0x%08X\n", caSrc,
						ui32Size, gsPdump1Context.psOutResFileName, gsPdump1Context.ui32OutResFileSize);
				/* Output to PDUMP file ...*/

pdumpAccessLock();
				IMG_FILEIO_WriteToFile(gsPdump1Context.hOutTxtFile, cBuffer, (IMG_UINT32)strlen(cBuffer));
pdumpAccessUnlock();

				if (!GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_RES))
					gsPdump1Context.ui32OutResFileSize += ui32Size;
			}
			if (gbCapturePdumpLite)			/* Bin Lite */
			{
				IMG_UINT64 ui64Src = stSrc.ui64BlockAddr + stSrc.ui64Value;
				if (stSrc.eMemSpT == PDUMP_MEMSPACE_VIRTUAL)
				{
					printf("WARNING: SaveBytes (SAB) command with virtual address cannot be converted to Binary Lite\n");
					if (gbSABVirtualWarning == IMG_FALSE)
					{
						printf("INFO: SAB virtual address can be converted to physical address using MMU context command\n");
						gbSABVirtualWarning = IMG_TRUE;
					}
					break;
				}
pdumpAccessLock();
				if (ui64Src >> 32 > 0)
				{
					IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, TAL_PDUMPL_SAB_64);
				}
				else
				{
					IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, TAL_PDUMPL_SAB);
				}
				IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, gui32SourceFileNo);
				IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, gui32SourceFileLineNo);
				if (ui64Src >> 32 > 0)
				{
					IMG_FILEIO_Write64BitWordToFile(psPdumpContext->hOutLiteBinFile, ui64Src);
					psPdumpContext->ui32OutLiteBinFiletSizeInDWords ++;
				}
				else
				{
					IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, IMG_UINT64_TO_UINT32(ui64Src));
				}
				IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, ui32Size);
				psPdumpContext->ui32OutLiteBinFiletSizeInDWords += 5;
pdumpAccessUnlock();
			}
			break;
		case IMG_ERROR_MMU_PAGE_CATALOGUE_FAULT:
			printf("ERROR: Cannot decode virtual address 0x%" IMG_I64PR "X, Page Category Fault\n", stSrc.ui64Value);
			IMG_ASSERT(IMG_FALSE);
			break;
		case IMG_ERROR_MMU_PAGE_DIRECTORY_FAULT:
			printf("ERROR: Cannot decode virtual address 0x%" IMG_I64PR "X, Page Directory Fault\n", stSrc.ui64Value);
			IMG_ASSERT(IMG_FALSE);
			break;
		case IMG_ERROR_MMU_PAGE_TABLE_FAULT:
			if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_SKIP_PAGEFAULTS))
				printf("WARNING: Page Table Faults found when decoding virtual address  0x%" IMG_I64PR "X", stSrc.ui64Value);
			else
			{
				printf("ERROR: Cannot decode virtual address 0x%" IMG_I64PR "X, Page Table Fault\n", stSrc.ui64Value);
				IMG_ASSERT(IMG_FALSE);
			}
			break;
		default:
			printf("ERROR: Cannot decode virtual address 0x%" IMG_I64PR "X\n", stSrc.ui64Value);
			IMG_ASSERT(IMG_FALSE);
			break;
		}
    }
	
	/* Output to PDUMP file ...*/
	if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_RES))
	{
pdumpAccessLock();
		if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP2))			/* Pdump2 */
		{
			IMG_FILEIO_WriteToFile(psPdumpContext->hOut2ResFile, (IMG_CHAR*)pui8Values, ui32Size);
			psPdumpContext->ui32Out2ResFileSize += ui32Size;
		}
		if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP1))			/* Pdump1 */
		{
			IMG_FILEIO_WriteToFile(gsPdump1Context.hOutResFile, (IMG_CHAR*)pui8Values, ui32Size);
			gsPdump1Context.ui32OutResFileSize += ui32Size;
		}
pdumpAccessUnlock();
	}
}


/*!
******************************************************************************

 @Function              pdump_cmd_ReadWord

******************************************************************************/
IMG_VOID pdump_cmd_ReadWord(
	PDUMP_CMD_sTarget		stSrc,
	PDUMP_CMD_sTarget		stDest,
	IMG_BOOL				bVerify,
	IMG_UINT32				ui32WordSizeFlag
)
{
	PDUMP_CMDS_psMemSpaceCB		psMemSpaceCBSrc = (PDUMP_CMDS_psMemSpaceCB)stSrc.hMemSpace;
	PDUMP_CMDS_psMemSpaceCB		psMemSpaceCBDest = (PDUMP_CMDS_psMemSpaceCB)stDest.hMemSpace; 
	PDUMP_CMDS_psPdumpContext	psPdumpContext;
	IMG_CHAR					caDest[50], caSrc[50], cBuffer[100];
	IMG_UINT32					ui32BinLiteType = 0;
	IMG_UINT64					ui64Src = 0;
	IMG_CHAR *					sReadCommand;
	
	/* Check to see if we should be capturing this */
	if (pdump_cmd_CheckCmdDisabled(psMemSpaceCBSrc, IMG_UINT64_TO_UINT32(stSrc.ui64Value), TAL_DISABLE_CAPTURE_RDW))
    {
		return;
    }

	IMG_ASSERT(psMemSpaceCBSrc != NULL);

	if (bVerify)
	{
		pdump_cmd_SaveBytes(stSrc, (IMG_PUINT8)&stDest.ui64Value, 4);
		return;
	}
	psPdumpContext = pdump_cmd_CheckContext(psMemSpaceCBSrc);
	
    /* Check Source and Desination Types and Create BinLite ...*/
	switch (stDest.eMemSpT)
	{
	/* Write word into internal register */
	case PDUMP_MEMSPACE_INT_VAR:
		psMemSpaceCBSrc->bPdumpSoftReg[stDest.ui64Value] = IMG_TRUE;
		switch (stSrc.eMemSpT)
		{
		case PDUMP_MEMSPACE_MEMORY:
			ui64Src = stSrc.ui64BlockAddr + stSrc.ui64Value;
			if(ui32WordSizeFlag == PDUMP_FLAGS_64BIT)
				ui32BinLiteType = TAL_PDUMPL_INTREG_RDW64_MEM_64;
			else if (ui64Src >> 32 > 0)
				ui32BinLiteType = TAL_PDUMPL_INTREG_RDW_MEM_64;
			else
				ui32BinLiteType = TAL_PDUMPL_INTREG_RDW_MEM;
			break;
		default:
			/* Use Write Command instead of read */
			pdump_cmd_WriteWord(stSrc, stDest, 0, ui32WordSizeFlag);
			return;
		}
		break;
	/* Read a Value back */
	case PDUMP_MEMSPACE_VALUE:
		switch (stSrc.eMemSpT)
		{
		case PDUMP_MEMSPACE_MEMORY:
			ui64Src = stSrc.ui64BlockAddr + stSrc.ui64Value;
			if(ui32WordSizeFlag == PDUMP_FLAGS_64BIT)
				ui32BinLiteType = TAL_PDUMPL_RDW64_MEM_64;
			else if (ui64Src >> 32 > 0)
				ui32BinLiteType = TAL_PDUMPL_RDW_MEM_64;
			else
				ui32BinLiteType = TAL_PDUMPL_RDW_MEM;
			break;
		case PDUMP_MEMSPACE_REGISTER:
			ui64Src = IMG_UINT64_TO_UINT32(psMemSpaceCBSrc->ui64BaseAddress) + stSrc.ui64Value;
			if(ui32WordSizeFlag == PDUMP_FLAGS_64BIT)
				ui32BinLiteType = TAL_PDUMPL_RDW64_REG_64;
			else if (ui64Src >> 32 > 0)
				ui32BinLiteType = TAL_PDUMPL_RDW_REG_64;
			else
				ui32BinLiteType = TAL_PDUMPL_RDW_REG;
			break;
			/* No other MemSpace Types Allowed for reading a Value*/
		default:
			printf("PDUMP_CMD ERROR : Incompatible Memspace %s Type in RDW Output\n", psMemSpaceCBDest->pszMemSpaceName);
			IMG_ASSERT(IMG_FALSE);
			break;
		}
		break;
	/* No other MemSpace Types Allowed for reading too*/
	default:
		printf("PDUMP_CMD ERROR : Incompatible Memspace %s Type in RDW Output\n", psMemSpaceCBDest->pszMemSpaceName);
		IMG_ASSERT(IMG_FALSE);
		break;
	}


	if (ui32WordSizeFlag & PDUMP_FLAGS_32BIT)
	{
		sReadCommand = "RDW";
	}
	else
	{
		sReadCommand = "RDW64";
	}
	if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP2))			/* Pdump2 */
	{
		pdmup_cmd_MakePdump2(caSrc, stSrc, IMG_FALSE);
		pdmup_cmd_MakePdump2(caDest, stDest, IMG_FALSE);
		if (stDest.eMemSpT == PDUMP_MEMSPACE_VALUE)
		{
			sprintf(cBuffer, "%s %s\n", sReadCommand, caSrc);
		}
		else
		{
			sprintf(cBuffer, "%s %s %s\n", sReadCommand, caDest, caSrc);
		}

pdumpAccessLock();
		IMG_FILEIO_WriteToFile(psPdumpContext->hOut2TxtFile, cBuffer, (IMG_UINT32)strlen(cBuffer));
pdumpAccessUnlock();
	}
	if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP1))			/* Pdump1 */
	{
		pdmup_cmd_MakePdump1(caSrc, stSrc);
		sprintf(cBuffer, "RDW %s\n", caSrc);

pdumpAccessLock();
		IMG_FILEIO_WriteToFile(gsPdump1Context.hOutTxtFile, cBuffer, (IMG_UINT32)strlen(cBuffer));
pdumpAccessUnlock();

		if (ui32WordSizeFlag & PDUMP_FLAGS_64BIT)
		{
			stSrc.ui64Value += 4;
			pdmup_cmd_MakePdump1(caSrc, stSrc);
			sprintf(cBuffer, "RDW %s\n", caSrc);

pdumpAccessLock();
			IMG_FILEIO_WriteToFile(gsPdump1Context.hOutTxtFile, cBuffer, (IMG_UINT32)strlen(cBuffer));
pdumpAccessUnlock();
		}
	}
	if (gbCapturePdumpLite)			/* Bin Lite */
	{
pdumpAccessLock();
		IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, ui32BinLiteType);
		IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, gui32SourceFileNo);
		IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, gui32SourceFileLineNo);
		switch(ui32BinLiteType)
		{
			case TAL_PDUMPL_INTREG_RDW64_MEM_64:
				IMG_FILEIO_Write64BitWordToFile(psPdumpContext->hOutLiteBinFile, ui64Src);
				psPdumpContext->ui32OutLiteBinFiletSizeInDWords ++;
				IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, IMG_UINT64_TO_UINT32(stDest.ui64Value));
				psPdumpContext->ui32OutLiteBinFiletSizeInDWords ++;
				break;
			case TAL_PDUMPL_RDW64_MEM_64:
			case TAL_PDUMPL_RDW64_REG_64:
				IMG_FILEIO_Write64BitWordToFile(psPdumpContext->hOutLiteBinFile, ui64Src);
				psPdumpContext->ui32OutLiteBinFiletSizeInDWords ++;
				break;
			default:
				if (ui64Src >> 32 > 0)
				{
					IMG_FILEIO_Write64BitWordToFile(psPdumpContext->hOutLiteBinFile, ui64Src);
					psPdumpContext->ui32OutLiteBinFiletSizeInDWords ++;
				}
				else
				{
					IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, IMG_UINT64_TO_UINT32(ui64Src));
				}
				if (stDest.eMemSpT == PDUMP_MEMSPACE_INT_VAR)
				{
					IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, IMG_UINT64_TO_UINT32(stDest.ui64Value));
					psPdumpContext->ui32OutLiteBinFiletSizeInDWords ++;
				}
				break;
		}
		psPdumpContext->ui32OutLiteBinFiletSizeInDWords += 4;
pdumpAccessUnlock();
	}
}


/*!
******************************************************************************

 @Function              PDUMP_CMD_ReadWord32

******************************************************************************/
IMG_VOID PDUMP_CMD_ReadWord32(
	PDUMP_CMD_sTarget		stSrc,
	PDUMP_CMD_sTarget		stDest,
	IMG_BOOL				bVerify
)
{
	pdump_cmd_ReadWord(stSrc, stDest, bVerify, PDUMP_FLAGS_32BIT);
}


/*!
******************************************************************************

 @Function              PDUMP_CMD_ReadWord64

******************************************************************************/
IMG_VOID PDUMP_CMD_ReadWord64(
	PDUMP_CMD_sTarget		stSrc,
	PDUMP_CMD_sTarget		stDest,
	IMG_BOOL				bVerify
)
{
	pdump_cmd_ReadWord(stSrc, stDest, bVerify, PDUMP_FLAGS_64BIT);
}

/*!
******************************************************************************

 @Function              pdump_cmd_WriteWord

******************************************************************************/
IMG_VOID pdump_cmd_WriteWord(
	PDUMP_CMD_sTarget		stSrc,
	PDUMP_CMD_sTarget		stDest,
	IMG_UINT64				ui64MangledVal,
	IMG_UINT32				ui32WordSizeFlag
)
{
  	PDUMP_CMDS_psMemSpaceCB		psSrcMemSpaceCB = (PDUMP_CMDS_psMemSpaceCB)stSrc.hMemSpace;
	PDUMP_CMDS_psMemSpaceCB		psDestMemSpaceCB = (PDUMP_CMDS_psMemSpaceCB)stDest.hMemSpace; 
	PDUMP_CMDS_psPdumpContext	psPdumpContext = pdump_cmd_CheckContext(psDestMemSpaceCB);
	IMG_CHAR					caDest[50], caSrc[100], cBuffer[150];
	IMG_UINT32					ui32BinLiteType = 0;
	IMG_UINT64					ui64Src = 0, ui64Dest = 0;
	IMG_BOOL					bUseSrcMemSpace = IMG_FALSE;
	IMG_CHAR *					sWriteCommand;
	PDUMP_CMD_sTarget			stPdump1Src;

	/* Check to see if we should be capturing this */
	if ( (gbCaptureEnabled != IMG_TRUE)
		|| (psDestMemSpaceCB->bCaptureEnabled != IMG_TRUE) )
		return;

	if (psSrcMemSpaceCB != NULL)
	{
		if (stSrc.eMemSpT == PDUMP_MEMSPACE_VALUE || stSrc.eMemSpT == PDUMP_MEMSPACE_MEMORY || stSrc.eMemSpT == PDUMP_MEMSPACE_INT_VAR)
		{
			bUseSrcMemSpace = IMG_TRUE;
		}
	}

	switch (stDest.eMemSpT)
	{
	/* Writing to an Internal Reg */
	case PDUMP_MEMSPACE_INT_VAR:
		if ((gui32DisableFlags & TAL_DISABLE_CAPTURE_INTERNAL_REG_CMDS) != 0)
			return;
		ui64Dest = stDest.ui64Value;
		/* Writing From ... */
		switch (stSrc.eMemSpT)
		{
		case PDUMP_MEMSPACE_VIRTUAL:
			// Not yet implemented
			IMG_ASSERT(IMG_FALSE);
			break;
		case PDUMP_MEMSPACE_MEMORY:
			psDestMemSpaceCB->bPdumpSoftReg[stDest.ui64Value] = IMG_FALSE;
			ui64Src = stSrc.ui64BlockAddr + stSrc.ui64Value;
			if (ui64Src >> 32 > 0)
				ui32BinLiteType = TAL_PDUMPL_INTREG_MOV_64;
			else
				ui32BinLiteType = TAL_PDUMPL_INTREG_MOV;
			break;
		case PDUMP_MEMSPACE_REGISTER:
			psDestMemSpaceCB->bPdumpSoftReg[stDest.ui64Value] = IMG_TRUE;
			ui64Src = IMG_UINT64_TO_UINT32(psSrcMemSpaceCB->ui64BaseAddress) + stSrc.ui64Value;
			if(ui32WordSizeFlag == PDUMP_FLAGS_64BIT)
				ui32BinLiteType = TAL_PDUMPL_INTREG_RDW64_REG_64;
			else if (ui64Src >> 32 > 0)
				ui32BinLiteType = TAL_PDUMPL_INTREG_RDW_REG_64;
			else
				ui32BinLiteType = TAL_PDUMPL_INTREG_RDW_REG;
			break;
		case PDUMP_MEMSPACE_VALUE:
			psDestMemSpaceCB->bPdumpSoftReg[stDest.ui64Value] = IMG_FALSE;
			ui64Src = stSrc.ui64Value;
			if (ui64Src >> 32 > 0)
				ui32BinLiteType = TAL_PDUMPL_INTREG_MOV_64;
			else
				ui32BinLiteType = TAL_PDUMPL_INTREG_MOV;
			break;
		default:
			printf("PDUMP_CMD ERROR : Incompatible Memspace %s Type in WRW Output\n", psSrcMemSpaceCB->pszMemSpaceName);
			IMG_ASSERT(IMG_FALSE);
			break;
		}
		break;
	/* Writing to Memory */
	case PDUMP_MEMSPACE_MEMORY:
		ui64Dest = stDest.ui64BlockAddr + stDest.ui64Value;
		switch (stSrc.eMemSpT)
		{
		/* Writing from Memory to Memory */
		case PDUMP_MEMSPACE_MEMORY:
			if ((gui32DisableFlags & TAL_DISABLE_CAPTURE_WRW_DEVMEM_DEVMEMREF) != 0)
				return;
			ui64Src = ui64MangledVal;
			if(ui32WordSizeFlag == PDUMP_FLAGS_64BIT)
				ui32BinLiteType = TAL_PDUMPL_WRW64_MEM_64;
			else if (ui64Src >> 32 > 0 || ui64Dest >> 32 > 0 )
				ui32BinLiteType = TAL_PDUMPL_WRW_MEM_64;
			else
				ui32BinLiteType = TAL_PDUMPL_WRW_MEM;
			break;
		/* Writing from Internal Register to Memory */
		case PDUMP_MEMSPACE_INT_VAR:
			if ((gui32DisableFlags & TAL_DISABLE_CAPTURE_INTERNAL_REG_CMDS) != 0)
				return;
			ui64Src = stSrc.ui64Value;
			if(ui32WordSizeFlag == PDUMP_FLAGS_64BIT)
				ui32BinLiteType = TAL_PDUMPL_INTREG_WRW64_MEM_64;
			else if (ui64Dest >> 32 > 0 )
				ui32BinLiteType = TAL_PDUMPL_INTREG_WRW_MEM_64;
			else
				ui32BinLiteType = TAL_PDUMPL_INTREG_WRW_MEM;
			break;
		/* Writing a Value to Memory */
		case PDUMP_MEMSPACE_VALUE:
			if (pdump_cmd_CheckCmdDisabled(psDestMemSpaceCB, IMG_UINT64_TO_UINT32(stDest.ui64Value), TAL_DISABLE_CAPTURE_WRW))
            {
				return;
            }
			ui64Src = stSrc.ui64Value;

			if(ui32WordSizeFlag == PDUMP_FLAGS_64BIT)
				ui32BinLiteType = TAL_PDUMPL_WRW64_MEM_64;
			else if (ui64Dest >> 32 > 0 )
				ui32BinLiteType = TAL_PDUMPL_WRW_MEM_64;
			else
				ui32BinLiteType = TAL_PDUMPL_WRW_MEM;
			break;
		default:
			printf("PDUMP_CMD ERROR : Incompatible Memspace %s Type in WRW Output\n", psSrcMemSpaceCB->pszMemSpaceName);
			IMG_ASSERT(IMG_FALSE);
			break;
		}
		break;
	/* Writing to Register */
	case PDUMP_MEMSPACE_REGISTER:
		ui64Dest = psDestMemSpaceCB->ui64BaseAddress + stDest.ui64Value;
		switch (stSrc.eMemSpT)
		{
		/* Writing from Memory to Register */
		case PDUMP_MEMSPACE_MEMORY:
			if ((gui32DisableFlags & TAL_DISABLE_CAPTURE_WRW_DEVMEMREF) != 0)
				return;
			ui64Src = ui64MangledVal;
			if(ui32WordSizeFlag == PDUMP_FLAGS_64BIT)
				ui32BinLiteType = TAL_PDUMPL_WRW64_REG_64;
			else if (ui64Dest >> 32 > 0 )
				ui32BinLiteType = TAL_PDUMPL_WRW_REG_64;
			else
				ui32BinLiteType = TAL_PDUMPL_WRW_REG;
			break;
		/* Writing from Internal Register to Register */
		case PDUMP_MEMSPACE_INT_VAR:
			if ((gui32DisableFlags & TAL_DISABLE_CAPTURE_INTERNAL_REG_CMDS) != 0)
				return;
			ui64Src = stSrc.ui64Value;
			if(ui32WordSizeFlag == PDUMP_FLAGS_64BIT)
				ui32BinLiteType = TAL_PDUMPL_INTREG_WRW64_REG_64;
			else if (ui64Dest >> 32 > 0 )
				ui32BinLiteType = TAL_PDUMPL_INTREG_WRW_REG_64;
			else
				ui32BinLiteType = TAL_PDUMPL_INTREG_WRW_REG;
			break;		
			break;
		/* Writing Value to Register */
		case PDUMP_MEMSPACE_VALUE:
			if (pdump_cmd_CheckCmdDisabled(psDestMemSpaceCB, IMG_UINT64_TO_UINT32(stDest.ui64Value), TAL_DISABLE_CAPTURE_WRW))
            {
				return;
            }
			ui64Src = stSrc.ui64Value;
			if(ui32WordSizeFlag == PDUMP_FLAGS_64BIT)
				ui32BinLiteType = TAL_PDUMPL_WRW64_REG_64;
			else if (ui64Dest >> 32 > 0 )
				ui32BinLiteType = TAL_PDUMPL_WRW_REG_64;
			else
				ui32BinLiteType = TAL_PDUMPL_WRW_REG;		
			break;
		default:
			printf("PDUMP_CMD ERROR : Incompatible Memspace %s Type in WRW Output\n", psSrcMemSpaceCB->pszMemSpaceName);
			IMG_ASSERT(IMG_FALSE);
			break;
		}
		break;
	default:
		printf("PDUMP_CMD ERROR : Incompatible Memspace %s Type in WRW Output\n", psDestMemSpaceCB->pszMemSpaceName);
		IMG_ASSERT(IMG_FALSE);
		break;
	}

	if(ui32WordSizeFlag == PDUMP_FLAGS_32BIT)
	{
		sWriteCommand = "WRW";
	}
	else
	{
		sWriteCommand = "WRW64";
	}

	if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP2))			/* Pdump2 */
	{
		pdmup_cmd_MakePdump2(caSrc, stSrc, IMG_FALSE);
		pdmup_cmd_MakePdump2(caDest, stDest, IMG_FALSE);
		sprintf(cBuffer, "%s %s %s\n", sWriteCommand, caDest, caSrc);

pdumpAccessLock();
		if (bUseSrcMemSpace)
		{
			IMG_FILEIO_WriteToFile(psSrcMemSpaceCB->psPdumpContext->hOut2TxtFile, cBuffer, (IMG_UINT32)strlen(cBuffer));
		}
		else
		{
			IMG_FILEIO_WriteToFile(psPdumpContext->hOut2TxtFile, cBuffer, (IMG_UINT32)strlen(cBuffer));
		}
pdumpAccessUnlock();
	}
	if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP1))			/* Pdump1 */
	{
		pdmup_cmd_MakePdump1(caDest, stDest);
		/* If this command writes to an internal register we can ignore it in pdump1 */
		if (stDest.eMemSpT != PDUMP_MEMSPACE_INT_VAR)
		{
			/* If the source is not an internal register
			  a value must be used */
			if (stSrc.eMemSpT == PDUMP_MEMSPACE_VALUE)
			{
				/* Write less significant bits first (little endian) */
				IMG_UINT64 ui64TempValue; /* Temp variable used to keep the original 64bit value */
				ui64TempValue = stSrc.ui64Value;
				stSrc.ui64Value &= 0xFFFFFFFFULL;
				pdmup_cmd_MakePdump1(caSrc, stSrc);
				stSrc.ui64Value = ui64TempValue;
			}
			else
			{
				/* Write less significant bits first (little endian) */
				stPdump1Src.hMemSpace = NULL;
				stPdump1Src.eMemSpT = PDUMP_MEMSPACE_VALUE;
				stPdump1Src.ui64BlockAddr = 0;
				stPdump1Src.ui32BlockId = TAL_NO_MEM_BLOCK;
				stPdump1Src.ui64Value = ui64MangledVal & 0xFFFFFFFFULL;
				pdmup_cmd_MakePdump1(caSrc, stPdump1Src);
			}
			sprintf(cBuffer, "WRW %s %s\n", caDest, caSrc);

pdumpAccessLock();
			IMG_FILEIO_WriteToFile(gsPdump1Context.hOutTxtFile, cBuffer, (IMG_UINT32)strlen(cBuffer));
pdumpAccessUnlock();

			if(ui32WordSizeFlag == PDUMP_FLAGS_64BIT)
			{
				/* Increase the offset in the destination memory address */
				stDest.ui64Value += 4;
				pdmup_cmd_MakePdump1(caDest, stDest);

				if (stSrc.eMemSpT == PDUMP_MEMSPACE_VALUE)
				{
					/* Write most significant bits */
					stSrc.ui64Value = stSrc.ui64Value >> 32;
					pdmup_cmd_MakePdump1(caSrc, stSrc);
				}
				else
				{
					/* Write most significant bits */
					stPdump1Src.ui64Value = ui64MangledVal >> 32;
					pdmup_cmd_MakePdump1(caSrc, stPdump1Src);
				}
				
				sprintf(cBuffer, "WRW %s %s\n", caDest, caSrc);
pdumpAccessLock();
				IMG_FILEIO_WriteToFile(gsPdump1Context.hOutTxtFile, cBuffer, (IMG_UINT32)strlen(cBuffer));
pdumpAccessUnlock();
			}
		}
		
	}
	if (gbCapturePdumpLite)			/* Bin Lite */
	{
		PDUMP_CMDS_psPdumpContext psBinContext;
		PDUMP_CMDS_psMemSpaceCB psInternalRegMemSpace = IMG_NULL;
		IMG_UINT32 ui32IntRegId = 0;
		IMG_UINT32 ui32NoOptimiseFlag = 0;

		// If the source and desination are in different contexts flag this as non-optimisable
		if (psSrcMemSpaceCB && psDestMemSpaceCB)
		{
			if (psSrcMemSpaceCB->psPdumpContext != psDestMemSpaceCB->psPdumpContext)
			{
				ui32NoOptimiseFlag = (IMG_UINT32)TAL_PDUMPL_NO_OPTIMISE;
			}
		}
		// Check which memory space we are pdumping too
		if (bUseSrcMemSpace)
		{
			psBinContext = psSrcMemSpaceCB->psPdumpContext;
		}
		else
		{
			psBinContext = psPdumpContext;
		}

		//Check to see if we are using an internal register
		if (stSrc.eMemSpT == PDUMP_MEMSPACE_INT_VAR)
		{
			psInternalRegMemSpace = psSrcMemSpaceCB;
			ui32IntRegId = IMG_UINT64_TO_UINT32(stSrc.ui64Value);
		}
		else if ( stDest.eMemSpT == PDUMP_MEMSPACE_INT_VAR )
		{
			psInternalRegMemSpace = psDestMemSpaceCB;
			ui32IntRegId = IMG_UINT64_TO_UINT32(stDest.ui64Value);
		}

		// Check to see if we can skip internal reg pdumping
		if (psInternalRegMemSpace == IMG_NULL || psInternalRegMemSpace->bPdumpSoftReg[ui32IntRegId])
		{	
pdumpAccessLock();
			IMG_FILEIO_WriteWordToFile(psBinContext->hOutLiteBinFile, ui32BinLiteType | ui32NoOptimiseFlag);
			IMG_FILEIO_WriteWordToFile(psBinContext->hOutLiteBinFile, gui32SourceFileNo);
			IMG_FILEIO_WriteWordToFile(psBinContext->hOutLiteBinFile, gui32SourceFileLineNo);
			if (ui32BinLiteType == TAL_PDUMPL_INTREG_MOV)
			{
				IMG_FILEIO_WriteWordToFile(psBinContext->hOutLiteBinFile, (IMG_UINT32)IMG_FALSE);
				psBinContext->ui32OutLiteBinFiletSizeInDWords ++;
			}
			
			switch (ui32BinLiteType)
			{
			case TAL_PDUMPL_INTREG_RDW_REG:
				IMG_FILEIO_WriteWordToFile(psBinContext->hOutLiteBinFile, IMG_UINT64_TO_UINT32(ui64Src));
				IMG_FILEIO_WriteWordToFile(psBinContext->hOutLiteBinFile, IMG_UINT64_TO_UINT32(ui64Dest));
				break;
			case TAL_PDUMPL_INTREG_RDW_REG_64:
				IMG_FILEIO_Write64BitWordToFile(psBinContext->hOutLiteBinFile, ui64Src);
				psBinContext->ui32OutLiteBinFiletSizeInDWords++;
				IMG_FILEIO_WriteWordToFile(psBinContext->hOutLiteBinFile, IMG_UINT64_TO_UINT32(ui64Dest));
				break;
			case TAL_PDUMPL_INTREG_RDW64_REG_64:
				IMG_FILEIO_Write64BitWordToFile(psBinContext->hOutLiteBinFile, ui64Src);
				psBinContext->ui32OutLiteBinFiletSizeInDWords++;
				IMG_FILEIO_Write64BitWordToFile(psBinContext->hOutLiteBinFile, ui64Dest);
				psBinContext->ui32OutLiteBinFiletSizeInDWords++;
				break;
			case TAL_PDUMPL_WRW64_MEM_64:
			case TAL_PDUMPL_INTREG_WRW64_MEM_64:
			case TAL_PDUMPL_WRW64_REG_64:
			case TAL_PDUMPL_INTREG_WRW64_REG_64:
				IMG_FILEIO_Write64BitWordToFile(psBinContext->hOutLiteBinFile, ui64Dest);
				psBinContext->ui32OutLiteBinFiletSizeInDWords ++;
				IMG_FILEIO_Write64BitWordToFile(psBinContext->hOutLiteBinFile, ui64Src);
				psBinContext->ui32OutLiteBinFiletSizeInDWords++;
				break;
			default:
				if (ui64Dest >> 32 > 0)
				{
					IMG_FILEIO_Write64BitWordToFile(psBinContext->hOutLiteBinFile, ui64Dest);
					psBinContext->ui32OutLiteBinFiletSizeInDWords ++;
				}
				else
				{
					IMG_FILEIO_WriteWordToFile(psBinContext->hOutLiteBinFile, IMG_UINT64_TO_UINT32(ui64Dest));
				}
				
				if(ui32WordSizeFlag == PDUMP_FLAGS_64BIT)
				{
					IMG_FILEIO_WriteWordToFile(psBinContext->hOutLiteBinFile, ui64Src & 0xFFFFFFFF);
					psBinContext->ui32OutLiteBinFiletSizeInDWords += 5;
					ui64Dest+= 4;
					ui64Src >>= 32;
					IMG_FILEIO_WriteWordToFile(psBinContext->hOutLiteBinFile, ui32BinLiteType | ui32NoOptimiseFlag);
					IMG_FILEIO_WriteWordToFile(psBinContext->hOutLiteBinFile, gui32SourceFileNo);
					IMG_FILEIO_WriteWordToFile(psBinContext->hOutLiteBinFile, gui32SourceFileLineNo);
					if (ui64Dest >> 32 > 0)
					{
						IMG_FILEIO_Write64BitWordToFile(psBinContext->hOutLiteBinFile, ui64Dest);
						psBinContext->ui32OutLiteBinFiletSizeInDWords ++;
					}
					else
					{
						IMG_FILEIO_WriteWordToFile(psBinContext->hOutLiteBinFile, IMG_UINT64_TO_UINT32(ui64Dest));
					}	
				}
				
				if (ui64Src >> 32 > 0)
				{
					IMG_FILEIO_Write64BitWordToFile(psBinContext->hOutLiteBinFile, ui64Src);
					psBinContext->ui32OutLiteBinFiletSizeInDWords++;
				}
				else
				{
					IMG_FILEIO_WriteWordToFile(psBinContext->hOutLiteBinFile, IMG_UINT64_TO_UINT32(ui64Src));
				}
				
				break;
			}
			psBinContext->ui32OutLiteBinFiletSizeInDWords += 5;
pdumpAccessUnlock();
		}
		else
		{
			// We can skip all but the writes from internal registers
			IMG_UINT32 ui32FixedBinLiteType = 0;
			switch (ui32BinLiteType)
			{
				case TAL_PDUMPL_INTREG_WRW_REG:
					ui32FixedBinLiteType = TAL_PDUMPL_WRW_REG;
					break;
				case TAL_PDUMPL_INTREG_WRW_MEM:
					ui32FixedBinLiteType = TAL_PDUMPL_WRW_MEM;
					break;
				case TAL_PDUMPL_INTREG_WRW_REG_64:
					ui32FixedBinLiteType = TAL_PDUMPL_WRW_REG_64;
					break;
				case TAL_PDUMPL_INTREG_WRW_MEM_64:
					ui32FixedBinLiteType = TAL_PDUMPL_WRW_MEM_64;
					break;
				case TAL_PDUMPL_INTREG_WRW64_REG_64:
					ui32FixedBinLiteType = TAL_PDUMPL_WRW64_REG_64;
					break;
				case TAL_PDUMPL_INTREG_WRW64_MEM_64:
					ui32FixedBinLiteType = TAL_PDUMPL_WRW64_MEM_64;
					break;
			}
pdumpAccessLock();	
			switch (ui32BinLiteType)
			{
			case TAL_PDUMPL_INTREG_WRW_REG:	
			case TAL_PDUMPL_INTREG_WRW_MEM:
				IMG_FILEIO_WriteWordToFile(psBinContext->hOutLiteBinFile, ui32FixedBinLiteType | ui32NoOptimiseFlag);
				IMG_FILEIO_WriteWordToFile(psBinContext->hOutLiteBinFile, gui32SourceFileNo);
				IMG_FILEIO_WriteWordToFile(psBinContext->hOutLiteBinFile, gui32SourceFileLineNo);
				IMG_FILEIO_WriteWordToFile(psBinContext->hOutLiteBinFile, IMG_UINT64_TO_UINT32(ui64Dest));
				IMG_FILEIO_WriteWordToFile(psBinContext->hOutLiteBinFile, IMG_UINT64_TO_UINT32(ui64MangledVal));
				psBinContext->ui32OutLiteBinFiletSizeInDWords += 5;

				break;
			case TAL_PDUMPL_INTREG_WRW_REG_64:
			case TAL_PDUMPL_INTREG_WRW_MEM_64:
				IMG_FILEIO_WriteWordToFile(psBinContext->hOutLiteBinFile, ui32FixedBinLiteType | ui32NoOptimiseFlag);
				IMG_FILEIO_WriteWordToFile(psBinContext->hOutLiteBinFile, gui32SourceFileNo);
				IMG_FILEIO_WriteWordToFile(psBinContext->hOutLiteBinFile, gui32SourceFileLineNo);
				IMG_FILEIO_Write64BitWordToFile(psBinContext->hOutLiteBinFile, ui64Dest);
				IMG_FILEIO_WriteWordToFile(psBinContext->hOutLiteBinFile, IMG_UINT64_TO_UINT32(ui64MangledVal));
				psBinContext->ui32OutLiteBinFiletSizeInDWords += 6;
				break;
			case TAL_PDUMPL_INTREG_WRW64_REG_64:
			case TAL_PDUMPL_INTREG_WRW64_MEM_64: 
				IMG_FILEIO_WriteWordToFile(psBinContext->hOutLiteBinFile, ui32FixedBinLiteType | ui32NoOptimiseFlag);
				IMG_FILEIO_WriteWordToFile(psBinContext->hOutLiteBinFile, gui32SourceFileNo);
				IMG_FILEIO_WriteWordToFile(psBinContext->hOutLiteBinFile, gui32SourceFileLineNo);
				IMG_FILEIO_Write64BitWordToFile(psBinContext->hOutLiteBinFile, ui64Dest);
				IMG_FILEIO_Write64BitWordToFile(psBinContext->hOutLiteBinFile, ui64MangledVal);
				psBinContext->ui32OutLiteBinFiletSizeInDWords += 7;
			default:
				break;
			}
pdumpAccessUnlock();
		}
	}
}

/*!
******************************************************************************

 @Function             PDUMP_CMD_WriteWord32

******************************************************************************/
IMG_VOID PDUMP_CMD_WriteWord32(
	PDUMP_CMD_sTarget		stSrc,
	PDUMP_CMD_sTarget		stDest,
	IMG_CHAR *				pszManglerFunc,
	IMG_UINT32				ui32BitMask,
	IMG_UINT64				ui64MangledVal
)
{
	(void)ui32BitMask;
	(void)pszManglerFunc;
	pdump_cmd_WriteWord(	stSrc, 
							stDest, 
							ui64MangledVal,
							PDUMP_FLAGS_32BIT);
}

/*!
******************************************************************************

 @Function             PDUMP_CMD_WriteWord64

******************************************************************************/
IMG_VOID PDUMP_CMD_WriteWord64(
	PDUMP_CMD_sTarget		stSrc,
	PDUMP_CMD_sTarget		stDest,
	IMG_CHAR *				pszManglerFunc,
	IMG_UINT32				ui32BitMask,
	IMG_UINT64				ui64MangledVal
)
{
	(void)ui32BitMask;
	(void)pszManglerFunc;
	pdump_cmd_WriteWord(	stSrc, 
							stDest, 
							ui64MangledVal,
							PDUMP_FLAGS_64BIT);
}


/*!
******************************************************************************

 @Function              PDUMP_CMD_LoadWords

******************************************************************************/
IMG_VOID PDUMP_CMD_LoadWords(
	PDUMP_CMD_sTarget	stDest,
	IMG_PUINT32			pui32Values,
	IMG_UINT32			ui32WordCount
)
{
	PDUMP_CMDS_psMemSpaceCB		psMemSpaceCB = (PDUMP_CMDS_psMemSpaceCB)stDest.hMemSpace;
	PDUMP_CMDS_psPdumpContext	psPdumpContext = pdump_cmd_CheckContext(psMemSpaceCB);
	IMG_CHAR					cBuffer[100], caDest[50];
	IMG_UINT32					ui32BinLiteType = 0, ui32Dest = 0, ui32Index = 0;

	/* Check to see if we should be capturing this */
	if ( (gbCaptureEnabled != IMG_TRUE)
		|| (psMemSpaceCB->bCaptureEnabled != IMG_TRUE)
		|| ((gui32DisableFlags & TAL_DISABLE_CAPTURE_LDW) != 0) )
		return;

	switch (stDest.eMemSpT)
	{
	case PDUMP_MEMSPACE_SLAVE_PORT:
		ui32BinLiteType = TAL_PDUMPL_LDW_SLV;
		ui32Dest = IMG_UINT64_TO_UINT32(psMemSpaceCB->ui64BaseAddress + stDest.ui64Value);
		break;
	case PDUMP_MEMSPACE_REGISTER:
		ui32BinLiteType = TAL_PDUMPL_LDW_REG;
		ui32Dest = IMG_UINT64_TO_UINT32(psMemSpaceCB->ui64BaseAddress + stDest.ui64Value);
		break;
	default:
		printf("PDUMP_CMD ERROR : Incompatible Memspace %s Type in LDW Output\n", psMemSpaceCB->pszMemSpaceName);
		IMG_ASSERT(IMG_FALSE);
		break;
	}

	if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP2))			/* Pdump2 */
    {
		/* Check if the prm file needs to be split...*/
        pdump_cmd_CheckSlitRequired(&psPdumpContext->psOut2PrmFileName, &psPdumpContext->hOut2PrmFile, &psPdumpContext->ui32Out2PrmFileSize, (ui32WordCount*sizeof(IMG_UINT32)));

        /* Create PDUMP and write command...*/
		pdmup_cmd_MakePdump2(caDest, stDest, IMG_FALSE);
        sprintf(cBuffer, "LDW %s 0x%X 0x%X %s\n", caDest, ui32WordCount,
				psPdumpContext->ui32Out2PrmFileSize, psPdumpContext->psOut2PrmFileName);
		
pdumpAccessLock();
        IMG_FILEIO_WriteToFile(psPdumpContext->hOut2TxtFile, cBuffer, (IMG_UINT32)strlen(cBuffer));
pdumpAccessUnlock();
    }
    if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP1))			/* Pdump1 */
    {
		/* Check if the file needs to be split...*/
        pdump_cmd_CheckSlitRequired(&gsPdump1Context.psOutPrmFileName, &gsPdump1Context.hOutPrmFile, &gsPdump1Context.ui32OutPrmFileSize, (ui32WordCount*4));

        /* Create PDUMP and write command...*/
		pdmup_cmd_MakePdump1(caDest, stDest);
        sprintf(cBuffer, "LDW %s %08X %08X %s\n",
				caDest, gsPdump1Context.ui32OutPrmFileSize,
				ui32WordCount, gsPdump1Context.psOutPrmFileName);

pdumpAccessLock();
        IMG_FILEIO_WriteToFile(gsPdump1Context.hOutTxtFile, cBuffer, (IMG_UINT32)strlen(cBuffer));
pdumpAccessUnlock();
    }
	if (gbCapturePdumpLite)			/* Bin Lite */
	{
pdumpAccessLock();
		IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, ui32BinLiteType);
		IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, gui32SourceFileNo);
		IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, gui32SourceFileLineNo);
		IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, ui32Dest); /* Write the abslute address, not just the offset */
		IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, ui32WordCount);
		psPdumpContext->ui32OutLiteBinFiletSizeInDWords += 5;
pdumpAccessUnlock();
	}
	
pdumpAccessLock();
	/* Output to PDUMP file ...Do as separate words to get consistant endian behaviour */
	for (ui32Index = 0; ui32Index < ui32WordCount  ; ui32Index++)
	{
		if (gbCapturePdumpLite)			/* Bin Lite */
		{
			IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, pui32Values[ui32Index]);
			psPdumpContext->ui32OutLiteBinFiletSizeInDWords++;
		}
		if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PRM))
		{
			if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP2))			/* Pdump2 */
			{
				IMG_FILEIO_WriteWordToFile(psPdumpContext->hOut2PrmFile, pui32Values[ui32Index]);
				psPdumpContext->ui32Out2PrmFileSize += 4;
			}
			if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP1))			/* Pdump1 */
			{
				IMG_FILEIO_WriteWordToFile(gsPdump1Context.hOutPrmFile, pui32Values[ui32Index]);
				gsPdump1Context.ui32OutPrmFileSize += 4;
			}
		}
		
	} // end for
pdumpAccessUnlock();
}

/*!
******************************************************************************

 @Function              PDUMP_CMD_SaveWords 

******************************************************************************/
IMG_VOID PDUMP_CMD_SaveWords(
	PDUMP_CMD_sTarget	stSrc,
	IMG_PUINT32			pui32Values,
	IMG_UINT32			ui32WordCount
	
)
{
  	PDUMP_CMDS_psMemSpaceCB		psMemSpaceCB = (PDUMP_CMDS_psMemSpaceCB)stSrc.hMemSpace;
	PDUMP_CMDS_psPdumpContext	psPdumpContext = pdump_cmd_CheckContext(psMemSpaceCB);
	IMG_CHAR					cBuffer[100], caSrc[50];

	/* Check to see if we should be capturing this */
	if ( (gbCaptureEnabled != IMG_TRUE)
		|| (psMemSpaceCB->bCaptureEnabled != IMG_TRUE)
		|| ((gui32DisableFlags & TAL_DISABLE_CAPTURE_SAW) != 0) )
		return;

	switch (stSrc.eMemSpT)
	{
	case PDUMP_MEMSPACE_SLAVE_PORT:
	case PDUMP_MEMSPACE_REGISTER:
		break;
	default:
		printf("PDUMP_CMD ERROR : Incompatible Memspace %s Type in LDW Output\n", psMemSpaceCB->pszMemSpaceName);
		IMG_ASSERT(IMG_FALSE);
		break;
	}
	if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP2))			/* Pdump2 */
    {
        /* Check if the file needs to be split...*/
        pdump_cmd_CheckSlitRequired(&psPdumpContext->psOut2ResFileName, &psPdumpContext->hOut2ResFile, &psPdumpContext->ui32Out2ResFileSize, ui32WordCount * sizeof(IMG_UINT32));

        /* Create PDUMP and write command...*/
		pdmup_cmd_MakePdump2(caSrc, stSrc, IMG_FALSE);
		sprintf(cBuffer, "SAW %s 0x%X 0x%X %s\n", 
				caSrc, ui32WordCount, psPdumpContext->ui32Out2ResFileSize, psPdumpContext->psOut2ResFileName);

pdumpAccessLock();
        IMG_FILEIO_WriteToFile(psPdumpContext->hOut2TxtFile, cBuffer, (IMG_UINT32)strlen(cBuffer));
pdumpAccessUnlock();
    }
    if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP1))			/* Pdump1 */
    {
        /* Check if the file needs to be split...*/
        pdump_cmd_CheckSlitRequired(&gsPdump1Context.psOutResFileName, &gsPdump1Context.hOutResFile, &gsPdump1Context.ui32OutResFileSize, ui32WordCount * sizeof(IMG_UINT32));

		pdmup_cmd_MakePdump1(caSrc, stSrc);
		sprintf(cBuffer, "SAW %s %08X %s -- offset 0x%08X\n", caSrc,
				ui32WordCount, gsPdump1Context.psOutResFileName, gsPdump1Context.ui32OutResFileSize);
		/* Output to PDUMP file ...*/

pdumpAccessLock();
		IMG_FILEIO_WriteToFile(gsPdump1Context.hOutTxtFile, cBuffer, (IMG_UINT32)strlen(cBuffer));
pdumpAccessUnlock();
    }
	/* Do not capture SAW in pdump lite */

	/* Output to PDUMP file ...*/
	if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_RES))
	{
pdumpAccessLock();
		if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP2))			/* Pdump2 */
		{
			IMG_FILEIO_WriteToFile(psPdumpContext->hOut2ResFile, (IMG_CHAR*)pui32Values, ui32WordCount * sizeof(IMG_UINT32));
			psPdumpContext->ui32Out2ResFileSize += ui32WordCount * sizeof(IMG_UINT32);
		}
		if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP1))			/* Pdump1 */
		{
			IMG_FILEIO_WriteToFile(gsPdump1Context.hOutResFile, (IMG_CHAR*)pui32Values, ui32WordCount * sizeof(IMG_UINT32));
			gsPdump1Context.ui32OutResFileSize += ui32WordCount * sizeof(IMG_UINT32);
		}
pdumpAccessUnlock();
	}
}

/*!
******************************************************************************

 @Function              PDUMP_CMD_LoadBytes

******************************************************************************/
IMG_VOID PDUMP_CMD_LoadBytes(
	PDUMP_CMD_sTarget	stDest,
	IMG_PUINT32			pui32Values,
	IMG_UINT32			ui32Size
)
{
  	PDUMP_CMDS_psMemSpaceCB		psMemSpaceCB = (PDUMP_CMDS_psMemSpaceCB)stDest.hMemSpace;
	PDUMP_CMDS_psPdumpContext	psPdumpContext = pdump_cmd_CheckContext(psMemSpaceCB);
	PDUMP_CMDS_psMemBlock		psMemBlock;
	IMG_CHAR					cBuffer[400], caDest[50];

	/* Check to see if we should be capturing this */
	if ( (gbCaptureEnabled != IMG_TRUE)
		|| (psMemSpaceCB->bCaptureEnabled != IMG_TRUE)
		|| ((gui32DisableFlags & TAL_DISABLE_CAPTURE_LDB) != 0) )
		return;

	switch (stDest.eMemSpT)
	{
	case PDUMP_MEMSPACE_MEMORY:
	case PDUMP_MEMSPACE_VIRTUAL:
		break;
	default:
		printf("PDUMP_CMD ERROR : Incompatible Memspace %s Type in SAB Output\n", psMemSpaceCB->pszMemSpaceName);
		IMG_ASSERT(IMG_FALSE);
		break;
	}

	if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP2))			/* Pdump2 */
    {
        /* Check if the file needs to be split...*/
        pdump_cmd_CheckSlitRequired(&psPdumpContext->psOut2PrmFileName, &psPdumpContext->hOut2PrmFile, &psPdumpContext->ui32Out2PrmFileSize, ui32Size);

        /* Create PDUMP and write command...*/
		psMemBlock = pdmup_cmd_getMemBlock(psMemSpaceCB, stDest.ui32BlockId);
		if (ui32Size == psMemBlock->ui32Size)
		{
			pdmup_cmd_MakePdump2(caDest, stDest, IMG_TRUE);
			sprintf(cBuffer, "LDB %s %d %s\n", 
				caDest, psPdumpContext->ui32Out2PrmFileSize, psPdumpContext->psOut2PrmFileName);
		}
		else
		{
			pdmup_cmd_MakePdump2(caDest, stDest, IMG_FALSE);
			sprintf(cBuffer, "LDB %s %d %d %s\n", 
				caDest, ui32Size, psPdumpContext->ui32Out2PrmFileSize, psPdumpContext->psOut2PrmFileName);
		}

pdumpAccessLock();
        IMG_FILEIO_WriteToFile(psPdumpContext->hOut2TxtFile, cBuffer, (IMG_UINT32)strlen(cBuffer));
pdumpAccessUnlock();
    }
    if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP1))			/* Pdump1 */
    {
   		/* Check if the file needs to be split...*/
        pdump_cmd_CheckSlitRequired(&gsPdump1Context.psOutPrmFileName, &gsPdump1Context.hOutPrmFile, &gsPdump1Context.ui32OutPrmFileSize, ui32Size);

		pdmup_cmd_MakePdump1(caDest, stDest);
		sprintf(cBuffer, "LDB %s %08X %08X %s\n", caDest,
				gsPdump1Context.ui32OutPrmFileSize, ui32Size, gsPdump1Context.psOutPrmFileName);
		/* Output to PDUMP file ...*/
pdumpAccessLock();
		IMG_FILEIO_WriteToFile(gsPdump1Context.hOutTxtFile, cBuffer, (IMG_UINT32)strlen(cBuffer));
pdumpAccessUnlock();
    }
	if (gbCapturePdumpLite)			/* Bin Lite */
	{
		IMG_UINT64 ui64Addr = stDest.ui64BlockAddr + stDest.ui64Value; // Use the abslute address, not just the offset

pdumpAccessLock();

		if (ui64Addr >> 32 > 0)
		{
			IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, TAL_PDUMPL_LDB_MEM_64);
		}
		else
		{
			IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, TAL_PDUMPL_LDB_MEM);
		}

		IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, gui32SourceFileNo);
		IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, gui32SourceFileLineNo);
		
		if (ui64Addr >> 32 > 0)
		{
			IMG_FILEIO_Write64BitWordToFile(psPdumpContext->hOutLiteBinFile, ui64Addr); 
			psPdumpContext->ui32OutLiteBinFiletSizeInDWords ++;
		}
		else
		{
			IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, IMG_UINT64_TO_UINT32(ui64Addr)); 
		}
		IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, ui32Size);
		psPdumpContext->ui32OutLiteBinFiletSizeInDWords += 5;
		
		/* Output the bytes to PDUMP Lite file ...*/
		IMG_FILEIO_WriteToFile(psPdumpContext->hOutLiteBinFile, (IMG_CHAR*)pui32Values, ui32Size);

		psPdumpContext->ui32OutLiteBinFiletSizeInDWords += (ui32Size/4);		/* Work out the size in rounded down dwords */

		/* If size is not 32 bit aligned, then need to write some padding bits so that the following command in the file sits at 32 bit aligned file offset */
		if (ui32Size & 0x00000003)
		{
			IMG_UINT32	ui32PaddingData = 0x00000000;
			IMG_FILEIO_WriteToFile(psPdumpContext->hOutLiteBinFile, (IMG_CHAR*)&ui32PaddingData, (4 - (ui32Size & 0x00000003)));

			psPdumpContext->ui32OutLiteBinFiletSizeInDWords++;	/* Add one for the last (padded) dword */
		}

pdumpAccessUnlock();
	}
	
	/* Output to PDUMP file ...*/
	if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PRM))
	{
pdumpAccessLock();
		if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP2))	/* Pdump2 */
		{
			IMG_FILEIO_WriteToFile(psPdumpContext->hOut2PrmFile, (IMG_CHAR*)pui32Values, ui32Size);
			psPdumpContext->ui32Out2PrmFileSize += ui32Size;
		}
		if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP1))	/* Pdump1 */
		{
			IMG_FILEIO_WriteToFile(gsPdump1Context.hOutPrmFile, (IMG_CHAR*)pui32Values, ui32Size);
			gsPdump1Context.ui32OutPrmFileSize += ui32Size;
		}
pdumpAccessUnlock();
	}
}

/*!
******************************************************************************

 @Function              PDUMP_CMD_SaveBytes

******************************************************************************/
IMG_VOID PDUMP_CMD_SaveBytes(
	PDUMP_CMD_sTarget	stSrc,
	IMG_PUINT8			pui8Values,
	IMG_UINT32			ui32Size
)	
{
	PDUMP_CMDS_psMemSpaceCB		psMemSpaceCB = (PDUMP_CMDS_psMemSpaceCB)stSrc.hMemSpace;

	/* Check to see if we should be capturing this */
	if ( (gbCaptureEnabled != IMG_TRUE)
		|| (psMemSpaceCB->bCaptureEnabled != IMG_TRUE)
		|| ((gui32DisableFlags & TAL_DISABLE_CAPTURE_SAB) != 0) 
        || ((psMemSpaceCB->ui32DisableFlags & TAL_DISABLE_CAPTURE_SAB) != 0)
        )
		return;

	pdump_cmd_SaveBytes(stSrc, pui8Values, ui32Size);
}


/*!
******************************************************************************

 @Function              pdump_cmd_CircBufPoll

******************************************************************************/
IMG_VOID pdump_cmd_CircBufPoll(
	PDUMP_CMD_sTarget	stSrc,
	IMG_UINT64			ui64WoffValue,
	IMG_UINT64			ui64PacketSize,
	IMG_UINT64			ui64BufferSize,
	IMG_UINT32			ui32PollCount,
	IMG_UINT32			ui32TimeOut,
	IMG_UINT32			ui32WordSizeFlag
)
{
  	PDUMP_CMDS_psMemSpaceCB		psMemSpaceCB = (PDUMP_CMDS_psMemSpaceCB)stSrc.hMemSpace;
	PDUMP_CMDS_psPdumpContext	psPdumpContext = pdump_cmd_CheckContext(psMemSpaceCB);
	IMG_CHAR					cBuffer[100], caSrc[50];
	IMG_UINT32					ui32BinLiteType = 0;
	IMG_UINT64					ui64ReadOffset = 0;
	
	/* Check to see if we should be capturing this */
	if ( (gbCaptureEnabled != IMG_TRUE)
		|| (psMemSpaceCB->bCaptureEnabled != IMG_TRUE)
		|| ((gui32DisableFlags & TAL_DISABLE_CAPTURE_CBP) != 0) )
		return;

	switch (stSrc.eMemSpT)
	{
	case PDUMP_MEMSPACE_REGISTER:
		ui32BinLiteType = TAL_PDUMPL_CBP_REG;
		ui64ReadOffset = IMG_UINT64_TO_UINT32(psMemSpaceCB->ui64BaseAddress) + stSrc.ui64Value;
		break;
	case PDUMP_MEMSPACE_MEMORY:
	case PDUMP_MEMSPACE_VIRTUAL:
		
		ui64ReadOffset = stSrc.ui64BlockAddr + stSrc.ui64Value;
		if (ui64ReadOffset >> 32 > 0)
			ui32BinLiteType = TAL_PDUMPL_CBP_MEM_64;
		else
			ui32BinLiteType = TAL_PDUMPL_CBP_MEM;
		break;
	default:
		printf("PDUMP_CMD ERROR : Incompatible Memspace %s in Circ Buffer Poll Output\n", psMemSpaceCB->pszMemSpaceName);
		IMG_ASSERT(IMG_FALSE);
		break;
	}

	if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP2))			/* Pdump2 */
    {
		pdmup_cmd_MakePdump2(caSrc, stSrc, IMG_FALSE);
		if (ui32WordSizeFlag & PDUMP_FLAGS_32BIT)
		{
			if (ui32PollCount > 0)
				sprintf(cBuffer, "CBP %s 0x%X 0x%X 0x%X 0x%X 0x%X\n",
						caSrc,	IMG_UINT64_TO_UINT32(ui64WoffValue), IMG_UINT64_TO_UINT32(ui64PacketSize), IMG_UINT64_TO_UINT32(ui64BufferSize), ui32PollCount, ui32TimeOut);
			else
				sprintf(cBuffer, "CBP %s 0x%X 0x%X 0x%X\n",
						caSrc,	IMG_UINT64_TO_UINT32(ui64WoffValue), IMG_UINT64_TO_UINT32(ui64PacketSize), IMG_UINT64_TO_UINT32(ui64BufferSize));
		}
		else if (ui32WordSizeFlag & PDUMP_FLAGS_64BIT)
		{
			if (ui32PollCount > 0)
				sprintf(cBuffer, "CBP64 %s 0x%" IMG_I64PR "X 0x%" IMG_I64PR "X 0x%" IMG_I64PR "X 0x%X 0x%X\n",
						caSrc, ui64WoffValue, ui64PacketSize, ui64BufferSize, ui32PollCount, ui32TimeOut);
			else
				sprintf(cBuffer, "CBP64 %s 0x%" IMG_I64PR "X 0x%" IMG_I64PR "X 0x%" IMG_I64PR "X\n",
						caSrc, ui64WoffValue, ui64PacketSize, ui64BufferSize);
		}
			/* Output to PDUMP file ...*/

pdumpAccessLock();
		IMG_FILEIO_WriteToFile(psPdumpContext->hOut2TxtFile, cBuffer, (IMG_UINT32)strlen(cBuffer));
pdumpAccessUnlock();
    }

	if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP1))			/* Pdump1 */
	{
		pdmup_cmd_MakePdump1(caSrc, stSrc);
		sprintf(cBuffer, "CBP %s %08X %08X %08X\n", caSrc, IMG_UINT64_TO_UINT32(ui64WoffValue), IMG_UINT64_TO_UINT32(ui64PacketSize),
					IMG_UINT64_TO_UINT32(ui64BufferSize));

pdumpAccessLock();
		IMG_FILEIO_WriteToFile(gsPdump1Context.hOutTxtFile, cBuffer, (IMG_UINT32)strlen(cBuffer));
pdumpAccessUnlock();
	}
	if (gbCapturePdumpLite)			/* Bin Lite */
	{
pdumpAccessLock();
		IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, ui32BinLiteType);
		IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, gui32SourceFileNo);
		IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, gui32SourceFileLineNo);
		if (ui64ReadOffset >> 32 > 0)
		{
			IMG_FILEIO_Write64BitWordToFile(psPdumpContext->hOutLiteBinFile, ui64ReadOffset);
			psPdumpContext->ui32OutLiteBinFiletSizeInDWords ++;
		}
		else
		{
			IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, IMG_UINT64_TO_UINT32(ui64ReadOffset));
		}
		if (ui32WordSizeFlag & PDUMP_FLAGS_32BIT)
		{
			IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, IMG_UINT64_TO_UINT32(ui64WoffValue));
			IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, IMG_UINT64_TO_UINT32(ui64PacketSize));
			IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, IMG_UINT64_TO_UINT32(ui64BufferSize));
		}
		else if (ui32WordSizeFlag & PDUMP_FLAGS_64BIT)
		{
			IMG_FILEIO_Write64BitWordToFile(psPdumpContext->hOutLiteBinFile, ui64WoffValue);
			IMG_FILEIO_Write64BitWordToFile(psPdumpContext->hOutLiteBinFile, ui64PacketSize);
			IMG_FILEIO_Write64BitWordToFile(psPdumpContext->hOutLiteBinFile, ui64BufferSize);
			psPdumpContext->ui32OutLiteBinFiletSizeInDWords += 3;
		}
		//IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, ui32PollCount);
		//IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, ui32TimeOut);

		psPdumpContext->ui32OutLiteBinFiletSizeInDWords += 7;
pdumpAccessUnlock();
	}
}



/*!
******************************************************************************

 @Function              PDUMP_CMD_LoopCommand

******************************************************************************/
IMG_VOID PDUMP_CMD_LoopCommand(
	PDUMP_CMD_sTarget		stSrc,
	IMG_UINT32				ui32CheckFuncIdExt,
	IMG_UINT32    			ui32RequValue,
	IMG_UINT32    			ui32Enable,
	IMG_UINT32				ui32LoopCount,
	PDUMP_CMD_eLoopCommand	eLoopCommand
)
{
  	PDUMP_CMDS_psMemSpaceCB		psMemSpaceCB = (PDUMP_CMDS_psMemSpaceCB)stSrc.hMemSpace;
	PDUMP_CMDS_psPdumpContext	psPdumpContext = pdump_cmd_CheckContext(psMemSpaceCB);
	IMG_CHAR					cBuffer[100], caSrc[50];
	
	(void)ui32LoopCount;

	/* Check to see if we should be capturing this */
	if ( (gbCaptureEnabled != IMG_TRUE)
		|| (psMemSpaceCB->bCaptureEnabled != IMG_TRUE)
		|| ((gui32DisableFlags & TAL_DISABLE_CAPTURE_CBP) != 0) )
		return;

	if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP2))			/* Pdump2 */
    {	
		switch(eLoopCommand)
		{
		case PDUMP_LOOP_COMMAND_WDO:
			if (gui32PdumpLoopLastCmd == PDUMP_LOOP_COMMAND_SDO)
			{
pdumpAccessLock();
				IMG_FILEIO_SeekFile(psPdumpContext->hOut2TxtFile, gui64PdumpFileLoopPos,SEEK_SET);
pdumpAccessUnlock();
			}
			pdmup_cmd_MakePdump2(caSrc, stSrc, IMG_FALSE);
			sprintf(cBuffer, "WDO %s 0x%X 0x%X 0x%X\n",
					caSrc,	ui32RequValue, ui32Enable, ui32CheckFuncIdExt);
			gui32PdumpLoopLastCmd = PDUMP_LOOP_COMMAND_WDO;
			break;
		case PDUMP_LOOP_COMMAND_DOW:
			pdmup_cmd_MakePdump2(caSrc, stSrc, IMG_FALSE);
			sprintf(cBuffer, "DOW %s 0x%X 0x%X 0x%X\n",
					caSrc,	ui32RequValue, ui32Enable, ui32CheckFuncIdExt);
			gui32PdumpLoopLastCmd = PDUMP_LOOP_COMMAND_DOW;
			break;
		case PDUMP_LOOP_COMMAND_EDO:
			sprintf(cBuffer, "EDO\n");
			gui32PdumpLoopLastCmd = PDUMP_LOOP_COMMAND_EDO;
			break;
		case PDUMP_LOOP_COMMAND_SDO:
			sprintf(cBuffer, "SDO\n");
			gui32PdumpLoopLastCmd = PDUMP_LOOP_COMMAND_SDO;

			pdumpAccessLock();
			IMG_FILEIO_Tell(psPdumpContext->hOut2TxtFile, &gui64PdumpFileLoopPos);
			pdumpAccessUnlock();
			break;
		default:
			IMG_ASSERT(IMG_FALSE);
		}

		pdumpAccessLock();
		IMG_FILEIO_WriteToFile(psPdumpContext->hOut2TxtFile, cBuffer, (IMG_UINT32)strlen(cBuffer));
		pdumpAccessUnlock();
    }

	if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP1))			/* Pdump1 */
	{
		pdmup_cmd_MakePdump1(caSrc, stSrc);
		
		switch(eLoopCommand)
		{
		case PDUMP_LOOP_COMMAND_WDO:
			sprintf(cBuffer, "WDO %s 0x%X 0x%X 0x%X\n",
					caSrc,	ui32RequValue, ui32Enable, ui32CheckFuncIdExt);
			break;
		case PDUMP_LOOP_COMMAND_DOW:
			sprintf(cBuffer, "DOW %s 0x%X 0x%X 0x%X\n",
					caSrc,	ui32RequValue, ui32Enable, ui32CheckFuncIdExt);
			break;
		case PDUMP_LOOP_COMMAND_EDO:
			sprintf(cBuffer, "EDO\n");
			break;
		case PDUMP_LOOP_COMMAND_SDO:
			sprintf(cBuffer, "SDO\n");
			break;
		default:
			IMG_ASSERT(IMG_FALSE);
		}

		pdumpAccessLock();
		IMG_FILEIO_WriteToFile(gsPdump1Context.hOutTxtFile, cBuffer, (IMG_UINT32)strlen(cBuffer));
		pdumpAccessUnlock();

	}
	if (gbCapturePdumpLite)			/* Bin Lite */
	{
		printf("PDUMP_CMD ERROR : Binary Lite not supported for loop commands\n");
		IMG_ASSERT(IMG_FALSE);
	}
}

/*!
******************************************************************************

 @Function              PDUMP_CMD_CircBufPoll32

******************************************************************************/
IMG_VOID PDUMP_CMD_CircBufPoll32(
	PDUMP_CMD_sTarget	stSrc,
	IMG_UINT32			ui32WoffValue,
	IMG_UINT32			ui32PacketSize,
	IMG_UINT32			ui32BufferSize,
	IMG_UINT32			ui32PollCount,
	IMG_UINT32			ui32TimeOut
)
{
	pdump_cmd_CircBufPoll(stSrc, ui32WoffValue, ui32PacketSize, ui32BufferSize, ui32PollCount, ui32TimeOut, PDUMP_FLAGS_32BIT);
}

/*!
******************************************************************************

 @Function              PDUMP_CMD_CircBufPoll64

******************************************************************************/
IMG_VOID PDUMP_CMD_CircBufPoll64(
	PDUMP_CMD_sTarget	stSrc,
	IMG_UINT64			ui64WoffValue,
	IMG_UINT64			ui64PacketSize,
	IMG_UINT64			ui64BufferSize,
	IMG_UINT32			ui32PollCount,
	IMG_UINT32			ui32TimeOut
)
{
	pdump_cmd_CircBufPoll(stSrc, ui64WoffValue, ui64PacketSize, ui64BufferSize, ui32PollCount, ui32TimeOut, PDUMP_FLAGS_64BIT);
}

/*!
******************************************************************************

 @Function              pdump_cmd_Poll

******************************************************************************/
IMG_VOID pdump_cmd_Poll(
	PDUMP_CMD_sTarget	stSrc,
	IMG_UINT64			ui64ReqVal,
	IMG_UINT64			ui64Enable,
	IMG_UINT32			ui32PollCount,
	IMG_UINT32			ui32Delay,
	IMG_UINT32			ui32Opid,
	IMG_CHAR *			pszCheckFunc,
	IMG_UINT32			ui32WordSizeFlag
)
{
  	PDUMP_CMDS_psMemSpaceCB		psMemSpaceCB = (PDUMP_CMDS_psMemSpaceCB)stSrc.hMemSpace;
	PDUMP_CMDS_psPdumpContext	psPdumpContext = pdump_cmd_CheckContext(psMemSpaceCB);
	IMG_CHAR					cBuffer[100], caSrc[50];
	IMG_UINT32					ui32BinLiteType = 0;
	IMG_UINT64					ui64ReadOffset = 0;
	IMG_CHAR *					sPollCommand;

	/* Check to see if we should be capturing this */
	if ( (gbCaptureEnabled != IMG_TRUE)
		|| (psMemSpaceCB->bCaptureEnabled != IMG_TRUE)
		|| ((gui32DisableFlags & TAL_DISABLE_CAPTURE_POL) != 0) )
		return;

    if ( ui32Delay >= TAL_PDUMP_MAX_POL_TIMEOUT )
    {
        // see TAL_PDUMP_MAX_POL_TIMEOUT
        printf("PDUMP_CMD WARNING: trying to POL for %u clock cycles - max recomended timeout is %u\n", ui32Delay, TAL_PDUMP_MAX_POL_TIMEOUT);
    }

	/* Check Memspace */
	switch (stSrc.eMemSpT)
	{
	case PDUMP_MEMSPACE_REGISTER:
		ui64ReadOffset = psMemSpaceCB->ui64BaseAddress + stSrc.ui64Value;
		if (ui32WordSizeFlag & PDUMP_FLAGS_64BIT)
			ui32BinLiteType = TAL_PDUMPL_POL64_REG_64;
		else if (ui64ReadOffset >> 32 > 0)
			ui32BinLiteType = TAL_PDUMPL_POL_REG_64;
		else
			ui32BinLiteType = TAL_PDUMPL_POL_REG;
		
		break;
	case PDUMP_MEMSPACE_MEMORY:
	case PDUMP_MEMSPACE_VIRTUAL:
		ui64ReadOffset = stSrc.ui64BlockAddr + stSrc.ui64Value;
		if (ui32WordSizeFlag & PDUMP_FLAGS_64BIT)
			ui32BinLiteType = TAL_PDUMPL_POL64_MEM_64;
		else if (ui64ReadOffset >> 32 > 0)
			ui32BinLiteType = TAL_PDUMPL_POL_MEM_64;
		else
			ui32BinLiteType = TAL_PDUMPL_POL_MEM;
		break;
	default:
		printf("PDUMP_CMD ERROR : Incompatible Memspace %s in Poll Output\n", psMemSpaceCB->pszMemSpaceName);
		IMG_ASSERT(IMG_FALSE);
		break;
	}

	if (ui32WordSizeFlag & PDUMP_FLAGS_64BIT)
		sPollCommand = "POL64";
	else
		sPollCommand = "POL";

	if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP2))			/* Pdump2 */
    {
		pdmup_cmd_MakePdump2(caSrc, stSrc, IMG_FALSE);
		if ( pszCheckFunc == IMG_NULL )
		{
			sprintf(cBuffer, "%s %s 0x%" IMG_I64PR "X 0x%" IMG_I64PR "X %d %d %d\n", sPollCommand, caSrc, ui64ReqVal, ui64Enable, ui32Opid, ui32PollCount, ui32Delay);
		}
		else
		{
			sprintf(cBuffer, "%s %s(%s 0x%" IMG_I64PR "X 0x%" IMG_I64PR "X) %d %d\n", sPollCommand, pszCheckFunc, caSrc, ui64ReqVal, ui64Enable, ui32PollCount, ui32Delay);
		}

pdumpAccessLock();
		IMG_FILEIO_WriteToFile(psPdumpContext->hOut2TxtFile, cBuffer, (IMG_UINT32)strlen(cBuffer));
pdumpAccessUnlock();
    }

	if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP1))			/* Pdump1 */
	{
		pdmup_cmd_MakePdump1(caSrc, stSrc);
		if ( pszCheckFunc == IMG_NULL )
		{
			if (ui32WordSizeFlag & PDUMP_FLAGS_64BIT)
			{
				PDUMP_CMD_sTarget	stSrc64 = stSrc;
				sprintf(cBuffer, "POL %s %08X %08X %d %d %d\n", caSrc, (IMG_UINT32)(ui64ReqVal & 0xFFFFFFFF), (IMG_UINT32)(ui64Enable & 0xFFFFFFFF), ui32Opid, ui32PollCount, ui32Delay);
				
pdumpAccessLock();
				IMG_FILEIO_WriteToFile(gsPdump1Context.hOutTxtFile, cBuffer, (IMG_UINT32)strlen(cBuffer));
pdumpAccessUnlock();

				stSrc64.ui64Value += 4;
				pdmup_cmd_MakePdump1(caSrc, stSrc64);
 				sprintf(cBuffer, "POL %s %08X %08X %d %d %d\n", caSrc, (IMG_UINT32)(ui64ReqVal >> 32), (IMG_UINT32)(ui64Enable >> 32), ui32Opid, ui32PollCount, ui32Delay);
			}
			else
			{
				sprintf(cBuffer, "POL %s %08X %08X %d %d %d\n", caSrc, IMG_UINT64_TO_UINT32(ui64ReqVal), IMG_UINT64_TO_UINT32(ui64Enable), ui32Opid, ui32PollCount, ui32Delay);
			}
			
		}
		else
		{
			sprintf(&cBuffer[0], "*** POL with ui32CheckFuncIdExt != SPECIAL not supported\n");
		}

pdumpAccessLock();
		IMG_FILEIO_WriteToFile(gsPdump1Context.hOutTxtFile, cBuffer, (IMG_UINT32)strlen(cBuffer));
pdumpAccessUnlock();

	}
	if (gbCapturePdumpLite)			/* Bin Lite */
	{
		IMG_ASSERT(pszCheckFunc == IMG_NULL);
		
pdumpAccessLock();

		IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, ui32BinLiteType);
		IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, gui32SourceFileNo);
		IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, gui32SourceFileLineNo);
		
		switch (ui32BinLiteType)
		{
			case TAL_PDUMPL_POL64_REG_64:
			case TAL_PDUMPL_POL64_MEM_64:
					IMG_FILEIO_Write64BitWordToFile(psPdumpContext->hOutLiteBinFile, ui64ReadOffset);
					IMG_FILEIO_Write64BitWordToFile(psPdumpContext->hOutLiteBinFile, ui64ReqVal);
					IMG_FILEIO_Write64BitWordToFile(psPdumpContext->hOutLiteBinFile, ui64Enable);
					psPdumpContext->ui32OutLiteBinFiletSizeInDWords += 3;
				break;
			default:
				if (ui64ReadOffset >> 32 > 0)
				{
					IMG_FILEIO_Write64BitWordToFile(psPdumpContext->hOutLiteBinFile, ui64ReadOffset);
					psPdumpContext->ui32OutLiteBinFiletSizeInDWords ++;
				}
				else
				{
					IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, IMG_UINT64_TO_UINT32(ui64ReadOffset));
				}
				IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, IMG_UINT64_TO_UINT32(ui64ReqVal));
				IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, IMG_UINT64_TO_UINT32(ui64Enable));
		}

		IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, ui32Opid);
		IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, ui32PollCount);
		IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, ui32Delay);
		psPdumpContext->ui32OutLiteBinFiletSizeInDWords += 9;

pdumpAccessUnlock();
	}
}

/*!
******************************************************************************

 @Function             PDUMP_CMD_Poll32

******************************************************************************/
IMG_VOID PDUMP_CMD_Poll32(
	PDUMP_CMD_sTarget	stSrc,
	IMG_UINT32			ui32ReqVal,
	IMG_UINT32			ui32Enable,
	IMG_UINT32			ui32PollCount,
	IMG_UINT32			ui32Delay,
	IMG_UINT32			ui32Opid,
	IMG_CHAR *			pszCheckFunc
)
{
	pdump_cmd_Poll(	stSrc, 
					ui32ReqVal, 
					ui32Enable,	
					ui32PollCount, 
					ui32Delay, 
					ui32Opid, 
					pszCheckFunc, 
					PDUMP_FLAGS_32BIT);
}


/*!
******************************************************************************

 @Function             PDUMP_CMD_Poll64

******************************************************************************/
IMG_VOID PDUMP_CMD_Poll64(
	PDUMP_CMD_sTarget	stSrc,
	IMG_UINT64			ui64ReqVal,
	IMG_UINT64			ui64Enable,
	IMG_UINT32			ui32PollCount,
	IMG_UINT32			ui32Delay,
	IMG_UINT32			ui32Opid,
	IMG_CHAR *			pszCheckFunc
)
{
	pdump_cmd_Poll(	stSrc, 
					ui64ReqVal, 
					ui64Enable,	
					ui32PollCount, 
					ui32Delay, 
					ui32Opid, 
					pszCheckFunc, 
					PDUMP_FLAGS_64BIT);
}
/*!
******************************************************************************

 @Function              PDUMP_CMD_Malloc     

******************************************************************************/
IMG_VOID PDUMP_CMD_Malloc(
	PDUMP_CMD_sTarget	stTarget,
	IMG_UINT32			ui32Size,
	IMG_UINT32			ui32Alignment,
	IMG_CHAR *			pszBlockName
)
{
  	PDUMP_CMDS_psMemSpaceCB		psMemSpaceCB = (PDUMP_CMDS_psMemSpaceCB)stTarget.hMemSpace;
	PDUMP_CMDS_psPdumpContext	psPdumpContext = pdump_cmd_CheckContext(psMemSpaceCB);
	IMG_CHAR					cBuffer[100], caDest[50];

	/* Check to see if we should be capturing this */
	if ( (gbCaptureEnabled != IMG_TRUE)
		|| (psMemSpaceCB->bCaptureEnabled != IMG_TRUE) )
		return;

    /* Check Memspace */
	switch (stTarget.eMemSpT)
	{
	case PDUMP_MEMSPACE_MEMORY:
	case PDUMP_MEMSPACE_MAPPED_MEMORY:
		break;
	default:
		printf("PDUMP_CMD ERROR : Incompatible Memspace %s in Malloc Output\n", psMemSpaceCB->pszMemSpaceName);
		IMG_ASSERT(IMG_FALSE);
		break;
	}

	pdmup_cmd_addMemBlock(psMemSpaceCB, stTarget.ui32BlockId, pszBlockName, ui32Size, stTarget.ui64BlockAddr);
	if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP2))			/* Pdump2 */
    {
		pdmup_cmd_MakePdump2(caDest,stTarget, IMG_TRUE);
		if (stTarget.eMemSpT == PDUMP_MEMSPACE_MAPPED_MEMORY)
		{
			/* Create PDUMP command...*/
			sprintf(&cBuffer[0], "MALLOC %s %d %d 0x%" IMG_I64PR "X\n", caDest, ui32Size, ui32Alignment, stTarget.ui64BlockAddr);
		}
		else
		{
			/* Create PDUMP command...*/
			sprintf(&cBuffer[0], "MALLOC %s %d %d\n", caDest, ui32Size, ui32Alignment);
		}

pdumpAccessLock();
		IMG_FILEIO_WriteToFile(psPdumpContext->hOut2TxtFile, cBuffer, (IMG_UINT32)strlen(cBuffer));
pdumpAccessUnlock();
    }
	/* No such command in Pdump1 or BinLite */
}

/*!
******************************************************************************

 @Function              PDUMP_CMD_Free

******************************************************************************/
IMG_VOID PDUMP_CMD_Free(
	PDUMP_CMD_sTarget	stTarget
)
{
  	PDUMP_CMDS_psMemSpaceCB		psMemSpaceCB = (PDUMP_CMDS_psMemSpaceCB)stTarget.hMemSpace;
	PDUMP_CMDS_psPdumpContext	psPdumpContext = pdump_cmd_CheckContext(psMemSpaceCB);
	IMG_CHAR					cBuffer[100], caDest[50];

	/* Check to see if we should be capturing this */
	if ( (gbCaptureEnabled != IMG_TRUE)
		|| (psMemSpaceCB->bCaptureEnabled != IMG_TRUE) )
		return;

    /* Check Memspace */
	switch (stTarget.eMemSpT)
	{
	case PDUMP_MEMSPACE_MEMORY:
		break;
	default:
		printf("PDUMP_CMD ERROR : Incompatible Memspace %s in Malloc Output\n", psMemSpaceCB->pszMemSpaceName);
		IMG_ASSERT(IMG_FALSE);
		break;
	}
	if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP2))			/* Pdump2 */
    {
		pdmup_cmd_MakePdump2(caDest, stTarget, IMG_TRUE);
		sprintf(cBuffer, "FREE %s\n", caDest);

pdumpAccessLock();
		IMG_FILEIO_WriteToFile(psPdumpContext->hOut2TxtFile, cBuffer, (IMG_UINT32)strlen(cBuffer));
pdumpAccessUnlock();
    }
	pdmup_cmd_removeMemBlock(psMemSpaceCB, stTarget.ui32BlockId);
	/* Not valid in Pdump1 or BinLite */
}

/*!
******************************************************************************

 @Function              PDUMP_CMD_CheckDumpImage     

******************************************************************************/
IMG_VOID PDUMP_CMD_CheckDumpImage(
	IMG_HANDLE			hMemSpace,
	IMG_UINT32			ui32Size
)
{
  	PDUMP_CMDS_psMemSpaceCB		psMemSpaceCB = (PDUMP_CMDS_psMemSpaceCB)hMemSpace;
	PDUMP_CMDS_psPdumpContext	psPdumpContext = pdump_cmd_CheckContext(psMemSpaceCB);

	if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP2))			/* Pdump2 */
    {
        /* Check if the file needs to be split...*/
        pdump_cmd_CheckSlitRequired(&psPdumpContext->psOut2ResFileName, &psPdumpContext->hOut2ResFile, &psPdumpContext->ui32Out2ResFileSize, ui32Size);
	}
	if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP1))			/* Pdump2 */
    {
        /* Check if the file needs to be split...*/
		pdump_cmd_CheckSlitRequired(&gsPdump1Context.psOutResFileName, &gsPdump1Context.hOutResFile, &gsPdump1Context.ui32OutResFileSize, ui32Size);
	}

}

/*!
******************************************************************************

 @Function              PDUMP_CMD_DumpImage      

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
)
{
    PDUMP_CMDS_psMemSpaceCB		psMemSpaceCB1 = (PDUMP_CMDS_psMemSpaceCB)siImg1.stTarget.hMemSpace;
	PDUMP_CMDS_psPdumpContext	psPdumpContext = pdump_cmd_CheckContext(psMemSpaceCB1);
	PDUMP_CMDS_psMemSpaceCB		psMemSpaceCB2 = (PDUMP_CMDS_psMemSpaceCB)siImg2.stTarget.hMemSpace;
	IMG_CHAR					cBuffer[300], cTempBuffer[64], caImg1[50], caImg2[50], caImg3[50];

	/* Check to see if we should be capturing this */
	if ( (gbCaptureEnabled != IMG_TRUE)
		|| (psMemSpaceCB1->bCaptureEnabled != IMG_TRUE) )
		return;

	IMG_ASSERT(siImg1.ui32Size != 0);

	/* Check Memspace */
	switch (siImg1.stTarget.eMemSpT)
	{
	case PDUMP_MEMSPACE_MEMORY:
	case PDUMP_MEMSPACE_VIRTUAL:
		break;
	default:
		printf("PDUMP_CMD ERROR : Incompatible Memspace %s in Dump Image Output\n", psMemSpaceCB1->pszMemSpaceName);
		IMG_ASSERT(IMG_FALSE);
		break;
	}

	if (siImg2.ui32Size != 0)
	{
		/* Check Memspace */
		switch (siImg2.stTarget.eMemSpT)
		{
		case PDUMP_MEMSPACE_MEMORY:
		case PDUMP_MEMSPACE_VIRTUAL:
			break;
		default:
			printf("PDUMP_CMD ERROR : Incompatible Memspace %s in Poll Output\n", psMemSpaceCB2->pszMemSpaceName);
			IMG_ASSERT(IMG_FALSE);
			break;
		}

		if (siImg3.ui32Size != 0)
		{
			/* Check Memspace */
			switch (siImg2.stTarget.eMemSpT)
			{
			case PDUMP_MEMSPACE_MEMORY:
			case PDUMP_MEMSPACE_VIRTUAL:
				break;
			default:
				printf("PDUMP_CMD ERROR : Incompatible Memspace %s in Poll Output\n", psMemSpaceCB2->pszMemSpaceName);
				IMG_ASSERT(IMG_FALSE);
				break;
			}
		}
	}
	else
	{
		/* If there is no image 2 there should be no image 3 */
		IMG_ASSERT(siImg3.ui32Size == 0);
	}
	

	if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP2))			/* Pdump2 */
    {
		IMG_UINT32 ui32Pdump2FileOffset = psPdumpContext->ui32Out2ResFileSize;
		pdmup_cmd_MakePdump2(caImg1, siImg1.stTarget, IMG_FALSE);
		sprintf(cBuffer, "SII %s %s %s 0x%X 0x%X ", pszImageSet, psPdumpContext->psOut2ResFileName, caImg1, siImg1.ui32Size, ui32Pdump2FileOffset);
		if (siImg2.ui32Size != 0)
		{
			pdmup_cmd_MakePdump2(caImg2, siImg2.stTarget, IMG_FALSE);
			ui32Pdump2FileOffset += siImg1.ui32Size;
			sprintf(cTempBuffer, "%s 0x%X 0x%X ", caImg2, siImg2.ui32Size, ui32Pdump2FileOffset);
			strcat(cBuffer, cTempBuffer);
			if (siImg3.ui32Size != 0)
			{
				pdmup_cmd_MakePdump2(caImg3, siImg3.stTarget, IMG_FALSE);
				ui32Pdump2FileOffset += siImg2.ui32Size;
				sprintf(cTempBuffer, "%s 0x%X 0x%X ", caImg3, siImg3.ui32Size, ui32Pdump2FileOffset);
				strcat(cBuffer, cTempBuffer);
			}
		}

		sprintf(cTempBuffer, "0x%X 0x%X 0x%X 0x%X 0x%X\n", ui32PixFmt, ui32Width, ui32Height, ui32Stride, ui32AddrMode);
		strcat (cBuffer, cTempBuffer);

pdumpAccessLock();
		IMG_FILEIO_WriteToFile(psPdumpContext->hOut2TxtFile, cBuffer, (IMG_UINT32)strlen(cBuffer));
pdumpAccessUnlock();
    }

	if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP1))			/* Pdump1 */
	{
		IMG_UINT32 ui32Pdump1FileOffset = gsPdump1Context.ui32OutResFileSize;
		
		/* Output Image 1 SAB */
		pdump_cmd_SaveBytes( siImg1.stTarget, siImg1.pui8ImageData, siImg1.ui32Size );

		sprintf(cBuffer, "SII %s %s %08X %08X ", pszImageSet, gsPdump1Context.psOutResFileName, siImg1.ui32Size, ui32Pdump1FileOffset);

		ui32Pdump1FileOffset += siImg2.ui32Size;
		if (siImg2.ui32Size != 0)
		{
            ui32Pdump1FileOffset += siImg2.ui32Size;
			/* Output Image 2 SAB */
			pdump_cmd_SaveBytes( siImg2.stTarget, siImg2.pui8ImageData, siImg2.ui32Size );
		}
		sprintf(cTempBuffer, "%08X %08X ", siImg2.ui32Size, ui32Pdump1FileOffset);
		strcat(cBuffer, cTempBuffer);
			
		ui32Pdump1FileOffset += siImg3.ui32Size;
		if (siImg3.ui32Size != 0)
		{
            ui32Pdump1FileOffset += siImg3.ui32Size;
			/* Output Image 3 SAB */
			pdump_cmd_SaveBytes( siImg3.stTarget, siImg3.pui8ImageData, siImg3.ui32Size );
		}
		ui32Pdump1FileOffset += siImg2.ui32Size;
		sprintf(cTempBuffer, "%08X %08X ", siImg3.ui32Size, ui32Pdump1FileOffset);
		strcat(cBuffer, cTempBuffer);

		sprintf(cTempBuffer, "%08X %08X %08X %08X %08X\n", ui32PixFmt, ui32Width, ui32Height, ui32Stride, ui32AddrMode);
		strcat (cBuffer, cTempBuffer);

pdumpAccessLock();
		IMG_FILEIO_WriteToFile(gsPdump1Context.hOutTxtFile, cBuffer, (IMG_UINT32)strlen(cBuffer));
pdumpAccessUnlock();
	}
	if (gbCapturePdumpLite)
	{
		/* Output Image 1 SAB */
		pdump_cmd_SaveBytes( siImg1.stTarget, siImg1.pui8ImageData, siImg1.ui32Size );

		if (siImg2.ui32Size != 0)
		{
			/* Output Image 2 SAB */
			pdump_cmd_SaveBytes( siImg2.stTarget, siImg2.pui8ImageData, siImg2.ui32Size );
		}

		if (siImg3.ui32Size != 0)
		{
			/* Output Image 3 SAB */
			pdump_cmd_SaveBytes( siImg3.stTarget, siImg3.pui8ImageData, siImg3.ui32Size );
		}
	}
	/* Output Data to RES file ...*/
	if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_RES) && GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP2))
	{

pdumpAccessLock();
		IMG_FILEIO_WriteToFile(psPdumpContext->hOut2ResFile, (IMG_CHAR*)siImg1.pui8ImageData, siImg1.ui32Size);
		IMG_FILEIO_WriteToFile(psPdumpContext->hOut2ResFile, (IMG_CHAR*)siImg2.pui8ImageData, siImg2.ui32Size);
		IMG_FILEIO_WriteToFile(psPdumpContext->hOut2ResFile, (IMG_CHAR*)siImg3.pui8ImageData, siImg3.ui32Size);
		psPdumpContext->ui32Out2ResFileSize += siImg1.ui32Size + siImg2.ui32Size + siImg3.ui32Size;
pdumpAccessUnlock();
	}
}

/*!
******************************************************************************

 @Function              PDUMP_CMD_CaptureGetFilename

******************************************************************************/
IMG_CHAR * PDUMP_CMD_CaptureGetFilename ( 
    IMG_HANDLE                      hMemSpace,
	TAL_eSetFilename		        eSetFilename
)
{
	PDUMP_CMDS_psMemSpaceCB		psMemSpaceCB = (PDUMP_CMDS_psMemSpaceCB)hMemSpace;
	PDUMP_CMDS_psPdumpContext psPdumpContext;
	/* Cannot call this function in kernel mode */
#ifdef IMG_KERNEL_MODULE
	IMG_ASSERT(IMG_FALSE);
	return IMG_NULL;
#else

	IMG_ASSERT(psMemSpaceCB != NULL);
	psPdumpContext = psMemSpaceCB->psPdumpContext;

	switch(eSetFilename)
	{
	case TAL_FILENAME_OUT_TXT:
		return gsPdump1Context.psOutTxtFileName;
	case TAL_FILENAME_OUT2_TXT:
		return psPdumpContext->psOut2TxtFileName;
	case TAL_FILENAME_OUT_RES:
		return gsPdump1Context.psOutResFileName;
	case TAL_FILENAME_OUT2_RES:
		return psPdumpContext->psOut2ResFileName;
	case TAL_FILENAME_OUT_PRM:
		return gsPdump1Context.psOutPrmFileName;
	case TAL_FILENAME_OUT2_PRM:
		return psPdumpContext->psOut2PrmFileName;
	case TAL_FILENAME_OUTL_BIN:
		return psPdumpContext->psOutLiteBinFileName;
	default:
		IMG_ASSERT(IMG_FALSE);
	}
	return IMG_NULL;
#endif
}

/*!
******************************************************************************

 @Function              PDUMP_CMD_RegisterCommand

******************************************************************************/
IMG_VOID PDUMP_CMD_RegisterCommand(
	IMG_UINT32			ui32CommandId,
	PDUMP_CMD_sTarget	stDest,
	PDUMP_CMD_sTarget	stOpreg1,
	PDUMP_CMD_sTarget	stOpreg2
)
{
  	PDUMP_CMDS_psMemSpaceCB		psMemSpaceCB = (PDUMP_CMDS_psMemSpaceCB)stDest.hMemSpace;
	PDUMP_CMDS_psPdumpContext	psPdumpContext = pdump_cmd_CheckContext(psMemSpaceCB);
	IMG_CHAR					cBuffer[100], caDest[50], caOpreg1[50], caOpreg2[50], cCmdBuffer[5];

	/* Check to see if we should be capturing this */
	if ( (gbCaptureEnabled != IMG_TRUE)
		|| (psMemSpaceCB->bCaptureEnabled != IMG_TRUE)
		|| ((gui32DisableFlags & TAL_DISABLE_CAPTURE_INTERNAL_REG_CMDS) != 0) )
		return;

	/* Check Memspace */
	switch (stDest.eMemSpT)
	{
	case PDUMP_MEMSPACE_INT_VAR:
		break;
	default:
		printf("PDUMP_CMD ERROR : Incompatible Memspace %s in Poll Output\n", psMemSpaceCB->pszMemSpaceName);
		IMG_ASSERT(IMG_FALSE);
		break;
	}
	/* Check Memspace */
	switch (stOpreg1.eMemSpT)
	{
	case PDUMP_MEMSPACE_INT_VAR:
	case PDUMP_MEMSPACE_VALUE:
		break;
	default:
		printf("PDUMP_CMD ERROR : Incompatible Memspace %s in Poll Output\n", psMemSpaceCB->pszMemSpaceName);
		IMG_ASSERT(IMG_FALSE);
		break;
	}
	/* Check Memspace */
	switch (stOpreg2.eMemSpT)
	{
	case PDUMP_MEMSPACE_INT_VAR:
	case PDUMP_MEMSPACE_VALUE:
		break;
	default:
		printf("PDUMP_CMD ERROR : Incompatible Memspace %s in Poll Output\n", psMemSpaceCB->pszMemSpaceName);
		IMG_ASSERT(IMG_FALSE);
		break;
	}

	switch (ui32CommandId)
	{
	case TAL_PDUMPL_INTREG_MOV:
		strcpy(cCmdBuffer, "MOV");
		break;
	case TAL_PDUMPL_INTREG_AND:
		strcpy(cCmdBuffer, "AND");
		break;
	case TAL_PDUMPL_INTREG_OR:
		strcpy(cCmdBuffer, "OR");
		break;
	case TAL_PDUMPL_INTREG_XOR:
		strcpy(cCmdBuffer, "XOR");
		break;
	case TAL_PDUMPL_INTREG_NOT:
		strcpy(cCmdBuffer, "NOT");
		break;
	case TAL_PDUMPL_INTREG_SHR:
		strcpy(cCmdBuffer, "SHR");
		break;
	case TAL_PDUMPL_INTREG_SHL:
		strcpy(cCmdBuffer, "SHL");
		break;
	case TAL_PDUMPL_INTREG_ADD:
		strcpy(cCmdBuffer, "ADD");
		break;
	case TAL_PDUMPL_INTREG_SUB:
		strcpy(cCmdBuffer, "SUB");
		break;
	case TAL_PDUMPL_INTREG_MUL:
		strcpy(cCmdBuffer, "MUL");
		break;
	case TAL_PDUMPL_INTREG_IMUL:
		strcpy(cCmdBuffer, "IMUL");
		break;
	case TAL_PDUMPL_INTREG_DIV:
		strcpy(cCmdBuffer, "DIV");
		break;
	case TAL_PDUMPL_INTREG_IDIV:
		strcpy(cCmdBuffer, "IDIV");
		break;
	case TAL_PDUMPL_INTREG_MOD:
		strcpy(cCmdBuffer, "MOD");
		break;
	case TAL_PDUMPL_INTREG_EQU:
		strcpy(cCmdBuffer, "EQU");
		break;
	case TAL_PDUMPL_INTREG_LT:
		strcpy(cCmdBuffer, "LT");
		break;
	case TAL_PDUMPL_INTREG_LTE:
		strcpy(cCmdBuffer, "LTE");
		break;
	case TAL_PDUMPL_INTREG_GT:
		strcpy(cCmdBuffer, "GT");
		break;
	case TAL_PDUMPL_INTREG_GTE:
		strcpy(cCmdBuffer, "GTE");
		break;
	case TAL_PDUMPL_INTREG_NEQ:
		strcpy(cCmdBuffer, "NEQ");
		break;
	default:
		printf("PDUMP_CMD ERROR : Unrecognised Internal Register Command %08X\n", ui32CommandId);
		IMG_ASSERT(IMG_FALSE);
	}

	/* Pdump2 Capture */
	if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP2))
	{
		pdmup_cmd_MakePdump2(caDest, stDest, IMG_TRUE);
		pdmup_cmd_MakePdump2(caOpreg1, stOpreg1, IMG_TRUE);
		pdmup_cmd_MakePdump2(caOpreg2, stOpreg2, IMG_TRUE);
		/* MOV and NOT only have two parameters */
		if (ui32CommandId == TAL_PDUMPL_INTREG_NOT)
		{
			sprintf(cBuffer, "%s %s %s\n", cCmdBuffer, caDest, caOpreg1);
		}
		else if (ui32CommandId == TAL_PDUMPL_INTREG_MOV)
		{
			sprintf(cBuffer, "%s %s %s\n", cCmdBuffer, caDest, caOpreg2);
		}
		else
		{
			pdmup_cmd_MakePdump2(caOpreg2, stOpreg2, IMG_TRUE);
			sprintf(cBuffer, "%s %s %s %s\n", cCmdBuffer, caDest, caOpreg1, caOpreg2);
		}

pdumpAccessLock();
		IMG_FILEIO_WriteToFile(psPdumpContext->hOut2TxtFile, cBuffer, (IMG_UINT32)strlen(cBuffer));
pdumpAccessUnlock();
	}
	/* Pdump1 Capture */
	if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP1))
    {
		//Pdump1 currently ignores all internal reg commands
		IMG_ASSERT (psMemSpaceCB->bPdumpSoftReg[stDest.ui64Value] == IMG_FALSE);
		/*pdmup_cmd_MakePdump1(caDest, stDest);
		pdmup_cmd_MakePdump1(caOpreg1, stOpreg1);
		// MOV and NOT only have two parameters
		if (ui32CommandId == TAL_PDUMPL_INTREG_MOV || ui32CommandId == TAL_PDUMPL_INTREG_NOT)
		{
			sprintf(cBuffer, "%s %s %s\n", cCmdBuffer, caDest, caOpreg1);
		}
		else
		{
			pdmup_cmd_MakePdump1(caOpreg2, stOpreg2);
			sprintf(cBuffer, "%s %s %s %s\n", cCmdBuffer, caDest, caOpreg1, caOpreg2);

		}
		IMG_FILEIO_WriteToFile(gsPdump1Context.hOutTxtFile, &cBuffer[0], (IMG_UINT32)strlen(&cBuffer[0]));*/
    }
	/* If capturing PDUMP Lite */
	if (gbCapturePdumpLite)
	{
		if (psMemSpaceCB->bPdumpSoftReg[stDest.ui64Value])
		{
pdumpAccessLock();
			IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, ui32CommandId);
			IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, gui32SourceFileNo);
			IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, gui32SourceFileLineNo);
			if (ui32CommandId == TAL_PDUMPL_INTREG_MOV || ui32CommandId == TAL_PDUMPL_INTREG_NOT)
			{
				if (ui32CommandId == TAL_PDUMPL_INTREG_MOV)
				{
					if (stOpreg1.eMemSpT == PDUMP_MEMSPACE_INT_VAR)
					{
						IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, 1);
					}
					else
					{
						IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, 0);
					}
					psPdumpContext->ui32OutLiteBinFiletSizeInDWords ++;
				}
				IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, IMG_UINT64_TO_UINT32(stDest.ui64Value));
				IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, IMG_UINT64_TO_UINT32(stOpreg1.ui64Value));
				psPdumpContext->ui32OutLiteBinFiletSizeInDWords += 5;
			}
			else
			{
				if (stOpreg2.eMemSpT == PDUMP_MEMSPACE_INT_VAR)
				{
					IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, 1);
				}
				else
				{
					IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, 0);
				}
				IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, IMG_UINT64_TO_UINT32(stDest.ui64Value));
				IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, IMG_UINT64_TO_UINT32(stOpreg1.ui64Value));
				IMG_FILEIO_WriteWordToFile(psPdumpContext->hOutLiteBinFile, IMG_UINT64_TO_UINT32(stOpreg2.ui64Value));
				psPdumpContext->ui32OutLiteBinFiletSizeInDWords += 7;
			}
pdumpAccessUnlock();
		}
	}
}

/*!
******************************************************************************

 @Function              PDUMP_CMD_TiledRegion

******************************************************************************/
IMG_VOID PDUMP_CMD_TiledRegion(
	PDUMP_CMD_sTarget	stTarget,
	IMG_UINT32			ui32RegionId,
	IMG_UINT64			ui64Size,
	IMG_UINT32			ui32Stride
)
{
  	PDUMP_CMDS_psMemSpaceCB		psMemSpaceCB = (PDUMP_CMDS_psMemSpaceCB)stTarget.hMemSpace;
	PDUMP_CMDS_psPdumpContext	psPdumpContext = pdump_cmd_CheckContext(psMemSpaceCB);
	IMG_CHAR					cBuffer[100], caTarget[50];
	
	/* Check to see if we should be capturing this */
	if ( (gbCaptureEnabled != IMG_TRUE)
		|| (psMemSpaceCB->bCaptureEnabled != IMG_TRUE) )
		return;

	switch (stTarget.eMemSpT)
	{
	case PDUMP_MEMSPACE_VIRTUAL:
		break;
	default:
		printf("PDUMP_CMD ERROR : Incompatible Memspace %s in Poll Output\n", psMemSpaceCB->pszMemSpaceName);
		IMG_ASSERT(IMG_FALSE);
		break;
	}
    if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP2))
	{
		pdmup_cmd_MakePdump2(caTarget, stTarget, IMG_TRUE);
		if (ui64Size != 0)
        {
            /* Create PDUMP command...*/
            sprintf(&cBuffer[0], "TRG %s %d 0x%016" IMG_I64PR "X 0x%016" IMG_I64PR "X %d\n", caTarget, ui32RegionId, stTarget.ui64Value, ui64Size, ui32Stride);
        }
        else
        {
            /* Create PDUMP command...*/
            sprintf(&cBuffer[0], "TRG %s %d\n", caTarget, ui32RegionId);
        }
        /* Output to PDUMP file ...*/
pdumpAccessLock();
        IMG_FILEIO_WriteToFile(psPdumpContext->hOut2TxtFile, &cBuffer[0], (IMG_UINT32)strlen(&cBuffer[0]));
pdumpAccessUnlock();
    }
	/* No Pdump1 or BinLite Equivalent */
}

/*!
******************************************************************************

 @Function              PDUMP_CMD_SetMMUContext

******************************************************************************/
IMG_VOID PDUMP_CMD_SetMMUContext(
	PDUMP_CMD_sTarget	stTarget,
	PDUMP_CMD_sTarget	stPageTable,
	IMG_UINT32			ui32MMUType
)
{
  	PDUMP_CMDS_psMemSpaceCB		psMemSpaceCB = (PDUMP_CMDS_psMemSpaceCB)stTarget.hMemSpace;
	PDUMP_CMDS_psPdumpContext	psPdumpContext = pdump_cmd_CheckContext(psMemSpaceCB);
	IMG_CHAR					cBuffer[100], caTarget[50], caPageTable[50];
	
	/* Check to see if we should be capturing this */
	if ( (gbCaptureEnabled != IMG_TRUE)
		|| (psMemSpaceCB->bCaptureEnabled != IMG_TRUE) )
		return;

	switch (stTarget.eMemSpT)
	{
	case PDUMP_MEMSPACE_VIRTUAL:
		break;
	default:
		printf("PDUMP_CMD ERROR : Incompatible Memspace %s in Poll Output\n", psMemSpaceCB->pszMemSpaceName);
		IMG_ASSERT(IMG_FALSE);
		break;
	}
    if (GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP2))
	{
		pdmup_cmd_MakePdump2(caTarget, stTarget, IMG_TRUE);
		if (ui32MMUType == 0)
        {
            /* Create PDUMP command...*/
            sprintf(cBuffer, "MMU %s\n", caTarget);
        }
        else
        {
			pdmup_cmd_MakePdump2(caPageTable, stPageTable, IMG_TRUE);
            /* Create PDUMP command...*/
            sprintf(cBuffer, "MMU %s %d %s\n", caTarget, ui32MMUType, caPageTable);
        }
        /* Output to PDUMP file ...*/
pdumpAccessLock();
        IMG_FILEIO_WriteToFile(psPdumpContext->hOut2TxtFile, &cBuffer[0], (IMG_UINT32)strlen(&cBuffer[0]));
pdumpAccessUnlock();
    }
	/* No Pdump1 or BinLite Equivalent */
}




/*!
******************************************************************************

 @Function              pdump_cmd_SetResPrmFileInfo

******************************************************************************/
static IMG_RESULT pdump_cmd_SetResPrmFileInfo(
	IMG_CHAR* pszNewFileName,
	PDUMP_CMDS_sResPrmFileInfo* psCustomNames,
	IMG_CHAR** ppszWorkingFileName,
	IMG_UINT32* pui32WorkingFileLength,
	IMG_HANDLE* phWorkingFileHandle,
	IMG_CHAR* pszPrefix
)
{
	IMG_BOOL	bNewName;
	IMG_UINT32	ui32NextNameId;
	IMG_UINT32	ui32CurrentNameId;
	IMG_CHAR*	pszTempFullPath = NULL;
	IMG_CHAR*	pszPtrToFileName;
	
	ui32CurrentNameId = psCustomNames->ui32CurrentNameId;

	// Copy the statistics of the last file being used over to the current index
	strcpy( psCustomNames->apszNames[ ui32CurrentNameId ], *ppszWorkingFileName );	
	psCustomNames->hFiles[ ui32CurrentNameId ]			= *phWorkingFileHandle;
	psCustomNames->ui32FileSizes[ ui32CurrentNameId ]	= *pui32WorkingFileLength;
	

	/* Ensure that we don't prefix any directories within the new name.
	** Find the start location of just the filename by searching for last 
	** file separator.*/
	if( strrchr( pszNewFileName, '/' ) != 0 )
	{
		pszPtrToFileName = strrchr( pszNewFileName, '/' ) + 1;
	}
	else if( strrchr( pszNewFileName, '\\') != 0 )
	{
		pszPtrToFileName = strrchr( pszNewFileName, '\\' ) + 1;
	}
	else
	{
		pszPtrToFileName = pszNewFileName;
	}


	// concatenate prefix and file name
	if( pszPrefix != NULL )
	{	
		pszTempFullPath 
			= (IMG_CHAR*)( calloc( ( 1 + strlen( pszPrefix ) + strlen( pszNewFileName )), sizeof( IMG_CHAR ) ));
		IMG_ASSERT( pszTempFullPath );

		strncpy( pszTempFullPath, pszNewFileName, pszPtrToFileName - pszNewFileName );
		strcat( pszTempFullPath, pszPrefix );
		strcat( pszTempFullPath, pszPtrToFileName );
	}
	else
	{
		// allocate new file name (enough for prefix, name, and null character
		pszTempFullPath 
			= (IMG_CHAR*)( calloc( ( 1 + strlen( pszNewFileName )), sizeof( IMG_CHAR )) );
		IMG_ASSERT( pszTempFullPath );

		strcpy( pszTempFullPath, pszPtrToFileName );
	}

	/* Change the current file to a user defined one */
	
	// Search through any previous files used - this will generate the correct nextname Id
	for( bNewName = IMG_TRUE, ui32NextNameId = 0; 
		ui32NextNameId < psCustomNames->ui32NameNum; 
		ui32NextNameId++ )
	{
		if( strcmp( pszTempFullPath, psCustomNames->apszNames[ui32NextNameId] ) == 0 )
		{
			bNewName = IMG_FALSE;
			break;
		}
	}

	// Check that we haven't however, tried to use too many names
	IMG_ASSERT( ui32NextNameId < MAX_CUSTOM_RES_PRM_NAME_NUM);

	if( bNewName == IMG_TRUE )
	{
pdumpAccessLock();
		// Create space for name in next location
		psCustomNames->apszNames[ui32NextNameId] = IMG_STRDUP( pszTempFullPath );

		/* 
		New file name: open a new file, reset all the working file information
		*/
		IMG_FILEIO_OpenFile( pszTempFullPath, "wb", phWorkingFileHandle, GET_ENABLE_COMPRESSION);
		*ppszWorkingFileName = psCustomNames->apszNames[ui32NextNameId];
		*pui32WorkingFileLength = 0;
		
		// increment number of files we are using
		psCustomNames->ui32NameNum++;
pdumpAccessUnlock();
	}
	else
	{
		// Copy information for the next file from structure
		*phWorkingFileHandle					= psCustomNames->hFiles[ui32NextNameId];
		*pui32WorkingFileLength					= psCustomNames->ui32FileSizes[ui32NextNameId];
		*ppszWorkingFileName					= psCustomNames->apszNames[ ui32NextNameId ];
	}

	// Assign new current ID
	psCustomNames->ui32CurrentNameId = ui32NextNameId;

	// clean up
	free( pszTempFullPath );

	return IMG_SUCCESS;
}



/*!
******************************************************************************

 @Function              PDUMP_CMD_SetResPrmFileName

******************************************************************************/
IMG_VOID PDUMP_CMD_SetResPrmFileName(
	IMG_HANDLE	hMemSpace,
	IMG_BOOL bNotResFile,
	IMG_CHAR* pszFileName,
	IMG_CHAR* pszOut2Prefix
)
{
	PDUMP_CMDS_psMemSpaceCB			psMemSpaceCB = (PDUMP_CMDS_psMemSpaceCB)hMemSpace;
	PDUMP_CMDS_sResPrmFileInfo*		psCustomNames;
	PDUMP_CMDS_sPdumpContext*		psPdump2Context;
	
	
	
	
	IMG_ASSERT( gbInitialised );


	if( GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP2) )
	{
		// check that a valid out2 prefix has been set
		IMG_ASSERT(pszOut2Prefix != NULL );
		
		if (psMemSpaceCB == NULL)
		{
			psPdump2Context = &gsPdumpContext;
		}
		else
		{
			psPdump2Context = psMemSpaceCB->psPdumpContext;
		}

		if( !bNotResFile )
		{
			psCustomNames = &psPdump2Context->asCustomResFiles;
			
			pdump_cmd_SetResPrmFileInfo(	pszFileName, 
											psCustomNames, 
											&psPdump2Context->psOut2ResFileName,
											&psPdump2Context->ui32Out2ResFileSize,
											&psPdump2Context->hOut2ResFile,
											pszOut2Prefix );
		}
		else
		{
			psCustomNames = &psPdump2Context->asCustomPrmFiles;

			pdump_cmd_SetResPrmFileInfo(	pszFileName, 
											psCustomNames, 
											&psPdump2Context->psOut2PrmFileName,
											&psPdump2Context->ui32Out2PrmFileSize,
											&psPdump2Context->hOut2PrmFile,
											pszOut2Prefix );
		}		
	}

	
	if( GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP1) )
	{
		if( !bNotResFile )
		{
			psCustomNames = &gsPdump1Context.asCustomResFiles;
			
			pdump_cmd_SetResPrmFileInfo(	pszFileName, 
											psCustomNames, 
											&gsPdump1Context.psOutResFileName,
											&gsPdump1Context.ui32OutResFileSize,
											&gsPdump1Context.hOutResFile,
											NULL );
		}
		else
		{
			psCustomNames = &gsPdump1Context.asCustomPrmFiles;

			pdump_cmd_SetResPrmFileInfo(	pszFileName, 
											psCustomNames, 
											&gsPdump1Context.psOutPrmFileName,
											&gsPdump1Context.ui32OutPrmFileSize,
											&gsPdump1Context.hOutPrmFile,
											NULL );
		}	
	}
}


/*!
******************************************************************************

 @Function              PDUMP_CMD_ResetResPrmFileName

******************************************************************************/
IMG_VOID PDUMP_CMD_ResetResPrmFileName(
	IMG_HANDLE	hMemSpace,
	IMG_BOOL bNotResFile
)
{
	PDUMP_CMDS_sResPrmFileInfo*		psCustomNames;
	PDUMP_CMDS_sPdumpContext*		psPdump2Context;
	PDUMP_CMDS_psMemSpaceCB			psMemSpaceCB = (PDUMP_CMDS_psMemSpaceCB)hMemSpace;
	
	IMG_ASSERT( gbInitialised );


	if( GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP2) )
	{
		if (hMemSpace == NULL)
		{
			psPdump2Context = &gsPdumpContext;
		}
		else
		{
			psPdump2Context = psMemSpaceCB->psPdumpContext;
		}

		if( !bNotResFile )
		{
			psCustomNames = &psPdump2Context->asCustomResFiles;
			
			pdump_cmd_SetResPrmFileInfo(	"out2.res", 
											psCustomNames, 
											&psPdump2Context->psOut2ResFileName,
											&psPdump2Context->ui32Out2ResFileSize,
											&psPdump2Context->hOut2ResFile,
											NULL );
		}
		else
		{
			psCustomNames = &psPdump2Context->asCustomPrmFiles;

			pdump_cmd_SetResPrmFileInfo(	"out2.prm",
											psCustomNames, 
											&psPdump2Context->psOut2PrmFileName,
											&psPdump2Context->ui32Out2PrmFileSize,
											&psPdump2Context->hOut2PrmFile,
											NULL );
		}		
	}

	
	if( GET_PDUMP_FLAG(TAL_PDUMP_FLAGS_PDUMP1) )
	{
		if( !bNotResFile )
		{
			psCustomNames = &gsPdump1Context.asCustomResFiles;
			
			pdump_cmd_SetResPrmFileInfo(	"out.res", 
											psCustomNames, 
											&gsPdump1Context.psOutResFileName,
											&gsPdump1Context.ui32OutResFileSize,
											&gsPdump1Context.hOutResFile,
											NULL );
		}
		else
		{
			psCustomNames = &gsPdump1Context.asCustomPrmFiles;

			pdump_cmd_SetResPrmFileInfo(	"out.prm", 
											psCustomNames, 
											&gsPdump1Context.psOutPrmFileName,
											&gsPdump1Context.ui32OutPrmFileSize,
											&gsPdump1Context.hOutPrmFile,
											NULL );
		}	
	}
}

/*!
******************************************************************************

 @Function				PDUMP_CMD_IsCaptureEnabled
 
******************************************************************************/
IMG_BOOL 
PDUMP_CMD_IsCaptureEnabled( void )
{
    return (gbCaptureEnabled && ((gFlags & TAL_PDUMP_FLAGS_PDUMP1) || (gFlags & TAL_PDUMP_FLAGS_PDUMP2)));
}


