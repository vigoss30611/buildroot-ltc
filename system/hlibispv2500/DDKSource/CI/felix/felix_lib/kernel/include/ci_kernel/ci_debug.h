/**
*******************************************************************************
@file ci_debug.h

@brief Debug functions

@copyright Imagination Technologies Ltd. All Rights Reserved. 

@license Strictly Confidential. 
   No part of this software, either material or conceptual may be copied or 
   distributed, transmitted, transcribed, stored in a retrieval system or  
   translated into any human or computer language in any form by any means, 
   electronic, mechanical, manual or other-wise, or disclosed to third  
   parties without the express written permission of  
   Imagination Technologies Limited,  
   Unit 8, HomePark Industrial Estate, 
   King's Langley, Hertfordshire, 
   WD4 8LZ, U.K. 

******************************************************************************/
#ifndef CI_DEBUG_H_
#define CI_DEBUG_H_

#include <img_types.h>
#include <img_defs.h>

#include "ci_kernel/ci_kernel.h"  // IMG_CI_BUFFER

#define FELIX_PDUMP_FOLDER "felix_pdump"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef FELIX_FAKE

/**
 * @brief to use TCP/IP or transif interface - modify before the call to
 * KRN_CI_DriverCreate. (defined in ci_internal.c)
 *
 * @note Only if Felix Fake
 */
extern IMG_BOOL8 bUseTCP;
/**
 * @brief When using TCP/IP is the IP address of the simulator
 *
 * @note Only if Felix Fake
 */
extern char *pszTCPSimIp;
/**
 * @brief When using TCP/IP is the port number used by the simulator
 *
 * @note Only if Felix Fake
 */
extern IMG_UINT32 ui32TCPPort;

/**
 * @brief If false will not initialise pdump
 *
 * @note Only if Felix Fake
 */
extern IMG_BOOL8 bEnablePdump;
/**
 * @brief When using Fake driver can disable generation of RDW pdump commands
 *
 * @note Only if Felix Fake
 */
extern IMG_BOOL8 bDisablePdumpRDW;

/**
 * @brief Can be used to create a pre-allocated block of virtual memory in
 * the device MMU to test HW MMU cache.
 *
 * @note Only if Felix Fake
 */
extern IMG_UINT32 aVirtualHeapsOffset[CI_N_HEAPS];

/**
 * @brief Start and configure the Pdump output
 *
 * @note Only if Felix Fake
 */
IMG_RESULT KRN_CI_DriverStartPdump(KRN_CI_DRIVER *pDriver,
    IMG_UINT8 enablePdump);

/**
 * @brief Update save structure from pipeline numbers... because there is
 * no HW to do that during unit tests
 *
 * @note Only if Felix Fake AND Unit test
 */
void KRN_CI_PipelineUpdateSave(KRN_CI_SHOT *pShot);

#endif

/*
 * if this is not defined then the functions are not accessible
 *
 * most of debug functions are only accessible when compiled in
 * FELIX_FAKE environment
 */
#ifdef CI_DEBUG_FCT

/**
 * @brief Forward declaration of the field information structure.
 *
 * This structure is defined in the generated fields_*.h register headers.
 */
 struct _FieldDefnListEl;

/*
 * shared register dump functions (with Data Generator)
 */

/**
 * @brief Get the TAL handle to use for a memory allocation
 *
 * @param psMem memory
 * @param uiOffset in memory
 * @param pOff2 offset to use in the given TAL handle (when using MMU offset
 * may change)
 *
 * @return The IMG_HANDLE to use with the TAL to access to that offset in
 * memory
 * @return NULL if wrong offset given
 */
IMG_HANDLE KRN_CI_DebugGetTalHandle(const SYS_MEM *psMem, IMG_SIZE uiOffset,
    IMG_SIZE *pOff2);

/**
 * @brief Sort a list of FieldDefnListEl by offset (reg offset then field
 * offset)
 *
 * @note Not a static function because it is used for data generator register
 * dumping as well
 */
IMG_RESULT KRN_CI_DebugRegistersSort(struct _FieldDefnListEl *pSortedBlock,
    IMG_SIZE nElem);

/**
 * @note Not a static function because it is used for data generator register
 * dumping as well
 */
IMG_RESULT KRN_CI_DebugRegisterPrint(IMG_HANDLE hFile,
    const struct _FieldDefnListEl *pField, IMG_HANDLE hTalRegBankHandle,
    IMG_BOOL bDumpFields, IMG_BOOL bIsMemstruct);

/**
 * @brief Prints the FieldDefnListEl to the hFile (File IO handle)
 *
 * @note Does nothing if register dumping is disabled
 */
IMG_RESULT KRN_CI_DebugRegistersDump(IMG_HANDLE hFile,
    struct _FieldDefnListEl *aSortedBlock, IMG_SIZE nElem,
    IMG_HANDLE hTalRegBankHandle, IMG_BOOL bDumpFields,
    IMG_BOOL bIsMemstruct);

void KRN_CI_DebugEnableRegisterDump(IMG_BOOL8 bEnable);

IMG_BOOL8 KRN_CI_DebugRegisterDumpEnabled(void);

/*
 * Felix specific register dump functions
 */

/**
 * @brief Dumps the Core registers to the hFile (File IO handle)
 *
 * Sorts its internal field structure if it hasn't been sorted yet.
 *
 * @note Does nothing if register dump is disabled
 */
IMG_RESULT KRN_CI_DebugDumpCore(IMG_HANDLE hFile, const IMG_CHAR *pszHeader,
    IMG_BOOL bDumpFields);

/**
 * @brief Dumps the Pointer structure (HW linked list) to the hFile
 * (File IO handle)
 *
 * Sorts its internal field structure if it hasn't been sorted yet.
 *
 * @note Does nothing if register dump is disabled
 */
IMG_RESULT KRN_CI_DebugDumpPointersStruct(IMG_HANDLE hTalHandle,
    IMG_HANDLE hFile, const IMG_CHAR *pszHeader, IMG_BOOL bDumpFields);

/**
 * @brief Dumps the Save strucutre to the hFile (File IO handle)
 *
 * Sorts its internal field structure if it hasn't been sorted yet.
 *
 * @note Does nothing if register dump is disabled
 */
IMG_RESULT KRN_CI_DebugDumpSaveStruct(IMG_HANDLE hTalHandle,
    IMG_HANDLE hFile, const IMG_CHAR *pszHeader, IMG_BOOL bDumpFields);

/**
 * @brief Dumps the Load structure to the hFile (File IO handle)
 *
 * Sorts its internal field structure if it hasn't been sorted yet.
 *
 * @note Does nothing if register dump is disabled
 */
IMG_RESULT KRN_CI_DebugDumpLoadStruct(IMG_HANDLE hTalHandle,
    IMG_HANDLE hFile, const IMG_CHAR *pszHeader, IMG_BOOL bDumpFields);

/**
 * @brief Dumps the Context ui32ContextNb to the hFile (File IO handle)
 *
 * Sorts its internal field structure if it hasn't been sorted yet.
 *
 * @note Does nothing if register dump is disabled
 */
IMG_RESULT KRN_CI_DebugDumpContext(IMG_HANDLE hFile,
    const IMG_CHAR *pszHeader, IMG_UINT32 ui32ContextNb,
    IMG_BOOL bDumpFields);

/**
 * @brief Dump all the registers to a file
 */
IMG_RESULT KRN_CI_DebugDumpRegisters(const IMG_CHAR *pszFilename,
    const IMG_CHAR *pszHeader, IMG_BOOL bDumpFields, IMG_BOOL bAppendFile);

/**
 * @brief Dump all associated memory of a given KRN_CI_SHOT
 */
IMG_RESULT KRN_CI_DebugDumpShot(const KRN_CI_SHOT *pShot,
    const IMG_CHAR *pszFilename, const IMG_CHAR *pszHeader,
    IMG_BOOL bDumpFields, IMG_BOOL bAppendFile);

/*
 * register override
 */

/**
 * @brief Access to register override store list - used for unit tests
 */
sLinkedList_T* getRegovr_store(int ctx);

/**
 * @brief Initialise the register override internal structures
 *
 * Called when driver is initialised if the FAKE interface is used
 */
IMG_RESULT KRN_CI_DebugRegisterOverrideInit(void);

/**
 * @brief Clean-up the register override internal structures
 *
 * Called when driver is finalised if the FAKE interface is used
 */
IMG_RESULT KRN_CI_DebugRegisterOverrideFinalise(void);

/**
 * @brief Load register overwrite values from file (and apply them for
 * every new frame until reset)
 *
 * Call this function from "user-side" test application.
 *
 * @warning To be applied the CI_PipelineUpdate must be called
 * (otherwise the correct update flags may not be set and HW will not
 * load the new configuration).
 *
 * @param ui32Context apply the overwrite when this context submits a new
 * frame
 * @param pszFilename filename to read - if NULL cleans the register overwrite
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_FATAL if the given file contains a invalid value for
 * a field
 */
IMG_RESULT CI_DebugSetRegisterOverride(IMG_UINT32 ui32Context,
    const IMG_CHAR *pszFilename);

/**
 * @brief Called from kernel-side just before submitting
 */
IMG_RESULT KRN_CI_DebugRegisterOverride(IMG_UINT32 ui32Context,
    SYS_MEM *pMemory);

/*
 * register testing
 */

void KRN_CI_DebugReg(IMG_HANDLE hTalHandle,
    struct _FieldDefnListEl *aSortedBlock, IMG_SIZE nElem, IMG_BOOL8 bWrite);

/**
 * @brief This funtion combines reading and writing to all registers
 *
 * @warning This should only be used for register testing
 *
 * @note The MMU is paused and not re-enabled during this test
 */
IMG_RESULT KRN_CI_DebugRegistersTest(void);

/**
 * @brief This function reads and polls all context registers
 *
 * @warning This is used to verify the load structure has been loaded properly
 */
IMG_RESULT CI_DebugLoadTest(IMG_UINT8 ctx);

IMG_RESULT CI_DebugGMALut(void);

IMG_RESULT KRN_CI_CRCCheck(KRN_CI_SHOT *pShot);

void CI_DebugTestIO(void);

#endif /* CI_DEBUG_FCT */

#ifdef CI_DEBUGFS

void KRN_CI_ResetDebugFS(void);

extern IMG_UINT32 g_ui32NCTXTriggered[CI_N_CONTEXT];
extern IMG_UINT32 g_ui32NCTXSubmitted[CI_N_CONTEXT];
extern IMG_UINT32 g_ui32NCTXInt[CI_N_CONTEXT];
extern IMG_UINT32 g_ui32NCTXDoneInt[CI_N_CONTEXT];
extern IMG_UINT32 g_ui32NCTXIgnoreInt[CI_N_CONTEXT];
extern IMG_UINT32 g_ui32NCTXStartInt[CI_N_CONTEXT];

extern IMG_UINT32 g_ui32NIntDGErrorInt[CI_N_IIF_DATAGEN];
extern IMG_UINT32 g_ui32NIntDGFrameEndInt[CI_N_IIF_DATAGEN];

extern IMG_UINT32 g_ui32NServicedHardInt;
extern IMG_UINT32 g_ui32NServicedThreadInt;
extern IMG_INT64 g_i64LongestHardIntUS;
extern IMG_INT64 g_i64LongestThreadIntUS;

// used to monitor device memory wartermark
#include <linux/mutex.h>
extern struct mutex g_DebugFSDevMemMutex;
extern IMG_UINT32 g_ui32DevMemUsed;
extern IMG_UINT32 g_ui32DevMemMaxUsed;

#ifdef INFOTM_ISP
// Extend Debug FS for Infotm
extern void ExtDebugFS_Create(struct dentry * pRoot);
#endif

#endif /* CI_DEBUGFS */

#ifdef __cplusplus
}
#endif

#endif /* CI_DEBUG_H_ */
