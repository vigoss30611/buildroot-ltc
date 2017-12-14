/**
******************************************************************************
@file dg_debug.h

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

#include <img_types.h>
#include <img_defs.h>

#ifndef __DG_DEBUG__
#define __DG_DEBUG__

/**
 * @defgroup DG_KERNEL Data Generator: kernel-side driver
 * @{
 */
/*-------------------------------------------------------------------------
 * Following elements are in the DG_KERNEL documentation module
 *------------------------------------------------------------------------*/

/*
 * if this is not defined then the functions are not accessible
 */

#ifdef CI_DEBUGFS
// may be troublesome if CI_N_IMAGERS is not defined when this header is included...
extern IMG_UINT32 g_ui32NDGTriggered[CI_N_IMAGERS];
extern IMG_UINT32 g_ui32NDGSubmitted[CI_N_IMAGERS];
extern IMG_UINT32 g_ui32NDGInt[CI_N_IMAGERS];

extern IMG_UINT32 g_ui32NServicedDGHardInt;
extern IMG_UINT32 g_ui32NServicedDGThreadInt;
extern IMG_INT64 g_i64LongestDGHardIntUS;
extern IMG_INT64 g_i64LongestDGThreadIntUS;
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef FELIX_FAKE
#ifdef CI_DEBUG_FCT

/**
 * @brief Dumps the Data Generator registers to the hFile (File IO handle)
 */
IMG_RESULT KRN_DG_DebugDumpDataGenerator(IMG_HANDLE hFile, const IMG_CHAR *pszHeader, IMG_UINT32 ui32ContextNb, IMG_BOOL bDumpFields);

IMG_RESULT KRN_DG_DebugDumpRegisters(const IMG_CHAR *pszFilename, const IMG_CHAR *pszHeader, IMG_BOOL bDumpFields, IMG_BOOL bAppendFile);

IMG_RESULT KRN_DG_DebugRegistersTest(void);

#endif // CI_DEBUG_FCT

IMG_BOOL8 DG_SetUseTransif(IMG_BOOL8 bUseTransif);

void DG_WaitForMaster(int dg);

#endif

#ifdef __cplusplus
}
#endif



/**
 * @}
 */
/*-------------------------------------------------------------------------
 * End of the DG_KERNEL documentation module
 *------------------------------------------------------------------------*/

#endif // __DG_DEBUG__
