/**
 ******************************************************************************
 @file ci_debug.c

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
#include <img_errors.h>

#include "ci_kernel/ci_debug.h"

#ifdef CI_PDP
#include "ci_internal/ci_pdp.h"
#endif

#include <stdlib.h> // qsort

#include <tal.h>

#include "ci_kernel/ci_kernel.h" // access to g_psCIDriver
#include "ci_kernel/ci_hwstruct.h"
#include "felix_hw_info.h" // CI_FATAL
#include "ci/ci_version.h"

#include <linkedlist.h>

#include <registers/fields_core.h>
#include <registers/fields_context0.h>
#include <registers/context0.h> // to clear interrupts before load test
#include <registers/fields_gammalut.h>
#include <registers/test_io.h>
#include <hw_struct/fields_ctx_pointers.h>
#include <hw_struct/fields_save.h>
#include <hw_struct/fields_ctx_config.h>
#include <hw_struct/save.h>
#include <hw_struct/ctx_pointers.h>

#include <registers/core.h>
#include <reg_io2.h>

#ifndef FELIX_FAKE
#error "should not be compiled if FELIX_FAKE is not defined"
#endif

#ifdef FELIX_HAS_DG
#include <dg_kernel/dg_camera.h>
#include <dg_kernel/dg_debug.h>
#include <registers/ext_data_generator.h> // to check version when starting pdump
#endif

#ifdef CI_DEBUG_FCT

#include "gzip_fileio.h"

#ifndef FELIX_DUMP_COMPRESSED_REGISTERS
#define FELIX_DUMP_COMPRESSED_REGISTERS IMG_FALSE
#endif

#endif

IMG_BOOL8 bUseTCP = IMG_TRUE; // declared in ci_debug.h to know if we should use TCP/IP or Transif interface for TAL
char *pszTCPSimIp = "127.0.0.1"; // declared in ci_debug.h to know the sim IP - default to localhost
IMG_UINT32 ui32TCPPort = 2345; // declared in ci_debug.h to know the sim port
IMG_BOOL8 bEnablePdump = IMG_TRUE;
IMG_BOOL8 bDisablePdumpRDW = IMG_FALSE;
IMG_UINT32 aVirtualHeapsOffset[CI_N_HEAPS] = {0, 0, 0, 0, 0, 0, 0};

// normally implemented as an insmod parameter in ci_init_km.c
IMG_UINT32 ciLogLevel = CI_LOG_LEVEL;

/*
 * functions to sort and print the register fields - shared with DG
 */

static int IMG_CI_FieldDefnListEl_sort(IMG_CONST void* A, IMG_CONST void* B)
{
    FieldDefnListEl *pA = (FieldDefnListEl*)A,
                    *pB = (FieldDefnListEl*)B;
    int ret = pA->ui32RegOffset - pB->ui32RegOffset;
    // if same register - compare fields
    if ( ret == 0 )
    {
        ret = pA->ui32FieldStart - pB->ui32FieldStart;
    }
    return ret;
}

IMG_RESULT KRN_CI_DriverStartPdump(KRN_CI_DRIVER *pDriver, IMG_UINT8 enablePdump)
{
    // verify that the handles are not NULL
    // and setting up pdump setting for it
    IMG_HANDLE *pCurrent = (IMG_HANDLE*)&pDriver->sTalHandles; // fake array
    IMG_UINT32 i, regval;
    IMG_UINT32 nCtx = 0, nImgr = 0;
    IMG_CHAR pdumpname[32];

#ifdef FELIX_HAS_DG
    IMG_HANDLE dgHandle[CI_N_EXT_DATAGEN];
    IMG_HANDLE dgMMU;
    IMG_HANDLE testio;
    IMG_HANDLE phy;

    // we need to claim the DG registers otherwise they are not in the pdump
    // but I don't know if it is going to make use loose memory
    for ( i = 0 ; i < CI_N_EXT_DATAGEN ; i++ )
    {
        sprintf(pdumpname, "REG_TEST_DG_%d", i);
        dgHandle[i] = TAL_GetMemSpaceHandle(pdumpname);
    }
    dgMMU = TAL_GetMemSpaceHandle("REG_TEST_MMU");
    testio = TAL_GetMemSpaceHandle("REG_TEST_IO");
    phy = TAL_GetMemSpaceHandle("REG_GASK_PHY_0");
#endif

    {
        CI_HWINFO sInfo;
        // we load the information just to access nb of context and imagers
        // the driver one will be loaded later on
        HW_CI_DriverLoadInfo(pDriver, &sInfo);

        nCtx = sInfo.config_ui8NContexts;
        nImgr = sInfo.config_ui8NImagers;
    }

    // use pdump2, parameters and results
    TALPDUMP_SetFlags(TAL_PDUMP_FLAGS_PDUMP2
        | TAL_PDUMP_FLAGS_PRM | TAL_PDUMP_FLAGS_RES);

    if (enablePdump > 1 )
    {
        TALPDUMP_AddMemspaceToContext("felix_main", pDriver->sTalHandles.hRegFelixCore);
        TALPDUMP_AddMemspaceToContext("felix_main", pDriver->sTalHandles.hMemHandle);
        TALPDUMP_AddMemspaceToContext("felix_main", pDriver->sTalHandles.hRegGammaLut);
        TALPDUMP_AddMemspaceToContext("felix_main", pDriver->sMMU.hRegFelixMMU);

        // remove the read from the INTERRUPT_SOURCE register (CORE) to allow invisible waiting of the interrupt
#if FELIX_VERSION_MAJ == 2 && FELIX_VERSION_MIN >= 6
        TALPDUMP_DisableCmds(pDriver->sTalHandles.hRegFelixCore, FELIX_CORE_FELIX_INTERRUPT_STATUS_OFFSET, TAL_DISABLE_CAPTURE_RDW);
#else
        TALPDUMP_DisableCmds(pDriver->sTalHandles.hRegFelixCore, FELIX_CORE_FELIX_INTERRUPT_SOURCE_OFFSET, TAL_DISABLE_CAPTURE_RDW);
#endif
        TALPDUMP_DisableCmds(pDriver->sTalHandles.hRegFelixCore, FELIX_TEST_IO_FELIX_IRQ_STATUS_OFFSET, TAL_DISABLE_CAPTURE_RDW);

        // not using pDriver->sHWInfo.config_ui8NContexts because it was not read yet!
        for ( i = 0 ; i < CI_N_CONTEXT ; i++ )
        {
            sprintf(pdumpname, "felix_ctx%d", i);
            TALPDUMP_AddMemspaceToContext(pdumpname, pDriver->sTalHandles.hRegFelixContext[i]);
            if ( i > nCtx )
            {
                TALPDUMP_DisableContextCapture(pdumpname);
            }
        }

        for ( i = 0 ; i < CI_N_IMAGERS ; i++ )
        {
            TALPDUMP_AddMemspaceToContext("felix_main", pDriver->sTalHandles.hRegGaskets[i]);
            if ( i > nImgr )
            {
                TALPDUMP_DisableContextCapture(pdumpname);
            }
        }

#if FELIX_VERSION_MAJ > 1
        for ( i = 0 ; i < CI_N_IIF_DATAGEN ; i++ )
        {
            TALPDUMP_AddMemspaceToContext("felix_main", pDriver->sTalHandles.hRegIIFDataGen[i]);
        }
#endif

#ifdef FELIX_HAS_DG
        for ( i = 0 ; i < CI_N_EXT_DATAGEN ; i ++ )
        {
            sprintf(pdumpname, "datagen_ctx%d", i);
            TALPDUMP_AddMemspaceToContext(pdumpname, dgHandle[i]);
        }
        TALPDUMP_AddMemspaceToContext("ext_dg", phy);
        TALPDUMP_AddMemspaceToContext("ext_dg", dgMMU);
        TALPDUMP_AddMemspaceToContext("test_io", testio);
#endif

        for ( i = TAL_NO_SYNC_ID ; i < CI_SYNC_NOT_USED ; i++ )
        {
            TALPDUMP_AddBarrierToTDF(i);
        }

        {
            IMG_HANDLE sI2CHandle = TAL_GetMemSpaceHandle("REG_FELIX_SCB");
            TALPDUMP_AddMemspaceToContext("felix_extern", sI2CHandle);
            TALPDUMP_DisableContextCapture(sI2CHandle); // because we know we won't need it

#ifdef CI_PDP
            {
                IMG_HANDLE sBAR0 = TAL_GetMemSpaceHandle(BAR0_BANK); // actually BAR0 for PDP
                TALPDUMP_AddMemspaceToContext("felix_extern", sBAR0);
            }
#endif
        }
    }// if pdump > 1

    IMG_ASSERT(sizeof(struct KRN_CI_DRIVER_HANDLE)%sizeof(IMG_HANDLE)==0); // check that there is a whole number of handles in the structure
    for ( i = 0 ; i < sizeof(struct KRN_CI_DRIVER_HANDLE)/sizeof(IMG_HANDLE) ; i++ )
    {
        if ( *pCurrent == NULL )
        {
            CI_WARNING("Driver TAL handle %d/%d is NULL\n", i+1, sizeof(struct KRN_CI_DRIVER_HANDLE)/sizeof(IMG_HANDLE));
            //IMG_ASSERT(IMG_FALSE);
            //return IMG_ERROR_FATAL;
        }
        else
        {
            TALPDUMP_MemSpceCaptureEnable(*pCurrent, IMG_TRUE, NULL); // enable or disable pdump for tal handles
            if(bDisablePdumpRDW)
            {
                TALPDUMP_DisableCmds(*pCurrent, TAL_OFFSET_ALL, TAL_DISABLE_CAPTURE_RDW);
            }
        }
        pCurrent++;
    }

    TALPDUMP_MemSpceCaptureEnable(pDriver->sMMU.hRegFelixMMU, IMG_TRUE, NULL); // enable or disable pdump for tal handles
    if(bDisablePdumpRDW)
    {
        TALPDUMP_DisableCmds(pDriver->sMMU.hRegFelixMMU, TAL_OFFSET_ALL, TAL_DISABLE_CAPTURE_RDW);
    }

#ifdef FELIX_HAS_DG
    // we need to claim the DG registers otherwise they are not in the pdump
    // but I don't know if it is going to make use loose memory
    for ( i = 0 ; i < CI_N_EXT_DATAGEN ; i++ )
    {
        TALPDUMP_MemSpceCaptureEnable(dgHandle[i], IMG_TRUE, NULL); // enable or disable pdump for tal handles
        if(bDisablePdumpRDW)
        {
            TALPDUMP_DisableCmds(dgHandle[i], TAL_OFFSET_ALL, TAL_DISABLE_CAPTURE_RDW);
        }
    }
    TALPDUMP_MemSpceCaptureEnable(phy, IMG_TRUE, NULL);
    TALPDUMP_MemSpceCaptureEnable(dgMMU, IMG_TRUE, NULL);
    TALPDUMP_MemSpceCaptureEnable(testio, IMG_TRUE, NULL);

    if(bDisablePdumpRDW)
    {
        TALPDUMP_DisableCmds(phy, TAL_OFFSET_ALL, TAL_DISABLE_CAPTURE_RDW);
        TALPDUMP_DisableCmds(dgMMU, TAL_OFFSET_ALL, TAL_DISABLE_CAPTURE_RDW);
        TALPDUMP_DisableCmds(testio, TAL_OFFSET_ALL, TAL_DISABLE_CAPTURE_RDW);
    }
#endif

    // we don't care about I2C pdump
#ifdef CI_PDP
    {
        IMG_HANDLE sBAR0 = TAL_GetMemSpaceHandle(BAR0_BANK); // actually BAR0 for PDP
        TALPDUMP_MemSpceCaptureEnable(sBAR0, IMG_TRUE, NULL);
        if(bDisablePdumpRDW)
        {
            TALPDUMP_DisableCmds(sBAR0, TAL_OFFSET_ALL, TAL_DISABLE_CAPTURE_RDW);
        }
    }
#endif
    
    if(!bDisablePdumpRDW)
    {
        // remove the read from the INTERRUPT_SOURCE register (CORE) to allow 
        // invisible waiting of the interrupt but if bDisablePdumpRDW 
        // TAL_OFFSET_ALL was used on all handles so we don't need to do it
#if FELIX_VERSION_MAJ == 2 && FELIX_VERSION_MIN >= 6
        TALPDUMP_DisableCmds(pDriver->sTalHandles.hRegFelixCore, FELIX_CORE_FELIX_INTERRUPT_STATUS_OFFSET, TAL_DISABLE_CAPTURE_RDW);
#else
        TALPDUMP_DisableCmds(pDriver->sTalHandles.hRegFelixCore, FELIX_CORE_FELIX_INTERRUPT_SOURCE_OFFSET, TAL_DISABLE_CAPTURE_RDW);
#endif
    }

    if ( enablePdump >= 1 )
    {
        TALPDUMP_CaptureStart(FELIX_PDUMP_FOLDER);
    }

    sprintf(pdumpname, "DDK@%s", CI_CHANGELIST_STR);
    CI_PDUMP_COMMENT(pDriver->sTalHandles.hRegFelixContext, pdumpname);
    // these Polls are done for the HW to verify the version
    CI_PDUMP_COMMENT(pDriver->sTalHandles.hRegFelixCore, "Driver checks versions");

    regval=0;

    TALREG_ReadWord32(pDriver->sTalHandles.hRegFelixCore, FELIX_CORE_CORE_ID_OFFSET, &regval);  // reads the core ID register
    //nCtx = REGIO_READ_FIELD(regval, FELIX_CORE, CORE_ID, FELIX_NUM_CONTEXT)+1;
    TALREG_Poll32(pDriver->sTalHandles.hRegFelixCore, FELIX_CORE_CORE_ID_OFFSET, TAL_CHECKFUNC_ISEQUAL, regval, 0xffffffff, 1, CI_REGPOLL_TIMEOUT);

    TALREG_ReadWord32(pDriver->sTalHandles.hRegFelixCore, FELIX_CORE_CORE_REVISION_OFFSET, &regval);  // reads the core Rev register
    TALREG_Poll32(pDriver->sTalHandles.hRegFelixCore, FELIX_CORE_CORE_REVISION_OFFSET, TAL_CHECKFUNC_ISEQUAL, regval,
                  FELIX_CORE_CORE_REVISION_CORE_MAJOR_REV_MASK|FELIX_CORE_CORE_REVISION_CORE_MINOR_REV_MASK
                  , 1, CI_REGPOLL_TIMEOUT);

    TALREG_ReadWord32(pDriver->sTalHandles.hRegFelixCore, FELIX_CORE_DESIGNER_REV_FIELD1_OFFSET, &regval); // read the designer rev field 1
    TALREG_Poll32(pDriver->sTalHandles.hRegFelixCore, FELIX_CORE_DESIGNER_REV_FIELD1_OFFSET, TAL_CHECKFUNC_ISEQUAL, regval, 0xffffffff, 1, CI_REGPOLL_TIMEOUT);

    for ( i = 0 ; i < nCtx ; i++ )
    {
        TALREG_ReadWord32(pDriver->sTalHandles.hRegFelixContext[i], FELIX_CONTEXT0_CONTEXT_CONFIG_OFFSET, &regval); // configuration of the current context
        TALREG_Poll32(pDriver->sTalHandles.hRegFelixContext[i], FELIX_CONTEXT0_CONTEXT_CONFIG_OFFSET, TAL_CHECKFUNC_ISEQUAL, regval, 0xffffffff, 1, CI_REGPOLL_TIMEOUT);
    }

    CI_PDUMP_COMMENT(pDriver->sTalHandles.hRegFelixCore, "check gasket type and bitdepth");

    for ( i = 0 ; i < nImgr ; i++ )
    {
#if FELIX_VERSION_MAJ == 1
        TALREG_ReadWord32(pDriver->sTalHandles.hRegFelixCore, REGIO_TABLE_OFF(FELIX_CORE, DESIGNER_REV_FIELD4, i), &regval);
        TALREG_Poll32(pDriver->sTalHandles.hRegFelixCore, REGIO_TABLE_OFF(FELIX_CORE, DESIGNER_REV_FIELD4, i), TAL_CHECKFUNC_ISEQUAL, regval,
            FELIX_CORE_DESIGNER_REV_FIELD4_GASKET_TYPE_MASK|FELIX_CORE_DESIGNER_REV_FIELD4_GASKET_BIT_DEPTH_MASK
            , 1, CI_REGPOLL_TIMEOUT);
#else
        TALREG_ReadWord32(pDriver->sTalHandles.hRegFelixCore, REGIO_TABLE_OFF(FELIX_CORE, DESIGNER_REV_FIELD5, i), &regval);
        TALREG_Poll32(pDriver->sTalHandles.hRegFelixCore, REGIO_TABLE_OFF(FELIX_CORE, DESIGNER_REV_FIELD5, i), TAL_CHECKFUNC_ISEQUAL, regval,
            FELIX_CORE_DESIGNER_REV_FIELD5_GASKET_TYPE_MASK|FELIX_CORE_DESIGNER_REV_FIELD5_GASKET_BIT_DEPTH_MASK
            , 1, CI_REGPOLL_TIMEOUT);
#endif
    }


    CI_PDUMP_COMMENT(pDriver->sTalHandles.hRegFelixCore, "Driver checks versions - done");

#ifdef FELIX_HAS_DG
    CI_PDUMP_COMMENT(dgHandle[0], "check DG config");
    TALREG_ReadWord32(dgHandle[0], FELIX_TEST_DG_DG_CONFIG_OFFSET, &regval);
    TALREG_Poll32(dgHandle[0], FELIX_TEST_DG_DG_CONFIG_OFFSET, TAL_CHECKFUNC_ISEQUAL, regval, 0xffffffff, 1, CI_REGPOLL_TIMEOUT);
    CI_PDUMP_COMMENT(dgHandle[0], "check DG config - done");
#endif

    return IMG_SUCCESS;
}

void KRN_CI_PipelineUpdateSave(KRN_CI_SHOT *pShot)
{
#ifdef FELIX_UNIT_TESTS
    // update save structure as well... because there is no HW to do that during unit tests
    IMG_UINT32 reg = 0;
    REGIO_WRITE_FIELD(reg, FELIX_SAVE, CONTEXT_LOGS, SAVE_CONFIG_TAG, pShot->pPipeline->pipelineConfig.ui8PrivateValue);
    SYS_MemWriteWord(&(pShot->hwSave.sMemory), FELIX_SAVE_CONTEXT_LOGS_OFFSET, reg);
#endif
}
#ifdef CI_DEBUG_FCT

IMG_RESULT KRN_CI_DebugRegistersSort(FieldDefnListEl *pBlock, IMG_SIZE nElem)
{
    if ( pBlock == NULL || nElem == 0 )
        return IMG_ERROR_INVALID_PARAMETERS;
    qsort(pBlock, nElem, sizeof(FieldDefnListEl), &IMG_CI_FieldDefnListEl_sort);
    return IMG_SUCCESS;
}

IMG_RESULT KRN_CI_DebugRegisterPrint(IMG_HANDLE hFile, IMG_CONST FieldDefnListEl *pField, IMG_HANDLE hTalRegBankHandle, IMG_BOOL bDumpFields, IMG_BOOL bIsMemstruct)
{
    char buffer[512];
    IMG_UINT32 ui32RegVal;

    if ( hFile == NULL || pField == NULL )
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    if (bIsMemstruct==IMG_FALSE)
    {
        TALREG_ReadWord32(hTalRegBankHandle, pField->ui32RegOffset, &ui32RegVal);
    }
    else
    {
        TALMEM_ReadWord32(hTalRegBankHandle, pField->ui32RegOffset, &ui32RegVal);
    }

    if ( bDumpFields == IMG_FALSE )
    {
        sprintf(buffer, "[offs 0x%08X] %-43s = 0x%08X\n", pField->ui32RegOffset, pField->szRegName, ui32RegVal);
    }
    else
    {
        IMG_UINT32 ui32FieldVal = 0;
        // index starts at 0 -> needs +1 for the number of bits
        // then create the mask and left shift it to its right position in the word
        IMG_UINT32 ui32Mask = ((1<<(pField->ui32FieldEnd-pField->ui32FieldStart +1))-1)<<pField->ui32FieldStart;

        ui32FieldVal = (ui32RegVal&ui32Mask)>>pField->ui32FieldStart;

        sprintf(buffer, "[mask 0x%08X]   .%-40s = %u (0x%X)\n", ui32Mask, pField->szFieldName, ui32FieldVal, ui32FieldVal);
    }

    if ( IMG_FILEIO_WriteToFile(hFile, buffer, strlen(buffer)) != IMG_SUCCESS )
    {
        return IMG_ERROR_FATAL;
    }
    return IMG_SUCCESS;
}

IMG_RESULT KRN_CI_DebugRegistersDump(IMG_HANDLE hFile, FieldDefnListEl *aSortedBlock, IMG_SIZE nElem, IMG_HANDLE hTalRegBankHandle, IMG_BOOL bDumpFields, IMG_BOOL bIsMemstruct)
{
    IMG_UINT32 i;
    FieldDefnListEl *pPrevious = NULL, *pCurrent = NULL;

    if ( hFile == NULL || aSortedBlock == NULL || nElem == 0 )
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    for ( i = 0 ; i < nElem ; i++ )
    {
        pCurrent = &(aSortedBlock[i]);

        // 1st register or same register than previously written one
        // or
        // if same register than previously only dump if bDumpFields is IMG_TRUE
        if ( pPrevious == NULL ||
                pCurrent->ui32RegOffset != pPrevious->ui32RegOffset )
        {
            if ( KRN_CI_DebugRegisterPrint(hFile, pCurrent, hTalRegBankHandle, IMG_FALSE, bIsMemstruct) != IMG_SUCCESS )
            {
                return IMG_ERROR_FATAL;
            }
            if ( bDumpFields == IMG_TRUE )
            {
                // only writes the field
                if ( KRN_CI_DebugRegisterPrint(hFile, pCurrent, hTalRegBankHandle, IMG_TRUE, bIsMemstruct) != IMG_SUCCESS )
                {
                    return IMG_ERROR_FATAL;
                }
            }
            pPrevious = pCurrent;
        }
        else if ( bDumpFields == IMG_TRUE )
        {
            // only writes the field
            if ( KRN_CI_DebugRegisterPrint(hFile, pCurrent, hTalRegBankHandle, IMG_TRUE, bIsMemstruct) != IMG_SUCCESS )
            {
                return IMG_ERROR_FATAL;
            }
            pPrevious = pCurrent;
        }
    }

    return IMG_SUCCESS;
}

static IMG_BOOL8 g_bRegisterDumpEnabled = IMG_TRUE;

void KRN_CI_DebugEnableRegisterDump(IMG_BOOL8 bEnable)
{
    g_bRegisterDumpEnabled = bEnable;
}

IMG_BOOL8 KRN_CI_DebugRegisterDumpEnabled(void)
{
    return g_bRegisterDumpEnabled;
}

/*
 * Felix register dump
 */

/*
 * static globals to store the register fields
 */

static FELIX_LOAD_STRUCTURE_FIELDS // felix_load_structure_fields
static IMG_SIZE load_struct_nElem = 0;

static FELIX_LINK_LIST_FIELDS // felix_link_list_fields
static IMG_SIZE link_list_nElem = 0;

static FELIX_SAVE_FIELDS // felix_save_fields - no ; or it doesn't build for some reasons...
static IMG_SIZE save_nElem = 0;

static FELIX_CORE_FIELDS // felix_core_fields - no ; or it doesn't build for some reasons...
static IMG_SIZE core_nElem = 0;

static FELIX_CONTEXT0_FIELDS // felix_context0_fields
static IMG_SIZE context_nElem = 0;

static FELIX_GAMMA_LUT_FIELDS // felix_gamma_lut_fields
static IMG_SIZE gamma_nElem = 0;

IMG_RESULT KRN_CI_DebugDumpPointersStruct(IMG_HANDLE hTalHandle, IMG_HANDLE hFile, IMG_CONST IMG_CHAR *pszHeader, IMG_BOOL bDumpFields)
{
    if ( g_bRegisterDumpEnabled == IMG_FALSE )
    {
        return IMG_SUCCESS;
    }

    if ( link_list_nElem == 0 ) // first time
    {
        link_list_nElem = sizeof(felix_link_list_fields)/sizeof(FieldDefnListEl);
        KRN_CI_DebugRegistersSort(felix_link_list_fields, link_list_nElem);
    }

    if ( hFile == NULL )
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    if ( hTalHandle == NULL )
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    if ( pszHeader != NULL )
    {
        if ( IMG_FILEIO_WriteToFile(hFile, pszHeader, strlen(pszHeader)) != IMG_SUCCESS )
        {
            CI_FATAL("failed to write header before dumping Felix Pointers structure\n");
            return IMG_ERROR_FATAL;
        }
    }

    if ( KRN_CI_DebugRegistersDump(hFile, felix_link_list_fields, link_list_nElem, hTalHandle, bDumpFields, IMG_TRUE) != IMG_SUCCESS )
    {
        CI_FATAL("failed to dump Felix Pointers structure\n");
        return IMG_ERROR_FATAL;
    }

    return IMG_SUCCESS;
}

IMG_RESULT KRN_CI_DebugDumpSaveStruct(IMG_HANDLE hTalHandle, IMG_HANDLE hFile, IMG_CONST IMG_CHAR *pszHeader, IMG_BOOL bDumpFields)
{
    if ( g_bRegisterDumpEnabled == IMG_FALSE )
    {
        return IMG_SUCCESS;
    }

    if ( save_nElem == 0 ) // first time
    {
        save_nElem = sizeof(felix_save_fields)/sizeof(FieldDefnListEl);
        KRN_CI_DebugRegistersSort(felix_save_fields, save_nElem);
    }

    if ( hFile == NULL )
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    if ( hTalHandle == NULL )
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    if ( pszHeader != NULL )
    {
        if ( IMG_FILEIO_WriteToFile(hFile, pszHeader, strlen(pszHeader)) != IMG_SUCCESS )
        {
            CI_FATAL("failed to write header before dumping Felix Save structure\n");
            return IMG_ERROR_FATAL;
        }
    }

    if ( KRN_CI_DebugRegistersDump(hFile, felix_save_fields, save_nElem, hTalHandle, bDumpFields, IMG_TRUE) != IMG_SUCCESS )
    {
        CI_FATAL("failed to dump Felix Save structure\n");
        return IMG_ERROR_FATAL;
    }

    return IMG_SUCCESS;
}

IMG_RESULT KRN_CI_DebugDumpLoadStruct(IMG_HANDLE hTalHandle, IMG_HANDLE hFile, IMG_CONST IMG_CHAR *pszHeader, IMG_BOOL bDumpFields)
{
    if ( g_bRegisterDumpEnabled == IMG_FALSE )
    {
        return IMG_SUCCESS;
    }

    if ( load_struct_nElem == 0 ) // first time
    {
        load_struct_nElem = sizeof(felix_load_structure_fields)/sizeof(FieldDefnListEl);
        KRN_CI_DebugRegistersSort(felix_load_structure_fields, load_struct_nElem);
    }

    if ( hFile == NULL )
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    if ( hTalHandle == NULL )
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    if ( pszHeader != NULL )
    {
        if ( IMG_FILEIO_WriteToFile(hFile, pszHeader, strlen(pszHeader)) != IMG_SUCCESS )
        {
            CI_FATAL("failed to write header before dumping Felix Load structure\n");
            return IMG_ERROR_FATAL;
        }
    }

    if ( KRN_CI_DebugRegistersDump(hFile, felix_load_structure_fields, load_struct_nElem, hTalHandle, bDumpFields, IMG_TRUE) != IMG_SUCCESS )
    {
        CI_FATAL("failed to dump Felix Load structure\n");
        return IMG_ERROR_FATAL;
    }

    return IMG_SUCCESS;
}

IMG_RESULT KRN_CI_DebugDumpCore(IMG_HANDLE hFile, IMG_CONST IMG_CHAR *pszHeader, IMG_BOOL bDumpFields)
{
    if ( g_bRegisterDumpEnabled == IMG_FALSE )
    {
        return IMG_SUCCESS;
    }

    if ( core_nElem == 0 )// first time
    {
        core_nElem = sizeof(felix_core_fields)/sizeof(FieldDefnListEl);
        KRN_CI_DebugRegistersSort(felix_core_fields, core_nElem);
    }

    if ( hFile == NULL )
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    if ( g_psCIDriver == NULL )
    {
        CI_FATAL("driver not initialised\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hRegFelixCore, "register dump");

    if ( pszHeader != NULL )
    {
        if ( IMG_FILEIO_WriteToFile(hFile, pszHeader, strlen(pszHeader)) != IMG_SUCCESS )
        {
            CI_FATAL("failed to write header before dumping Felix Core registers\n");
            return IMG_ERROR_FATAL;
        }
    }

    if ( KRN_CI_DebugRegistersDump(hFile, felix_core_fields, core_nElem, g_psCIDriver->sTalHandles.hRegFelixCore, bDumpFields, IMG_FALSE) != IMG_SUCCESS )
    {
        CI_FATAL("failed to dump Felix Core registers\n");
        return IMG_ERROR_FATAL;
    }

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hRegFelixCore, "dump done");

    return IMG_SUCCESS;
}

IMG_RESULT KRN_CI_DebugDumpContext(IMG_HANDLE hFile, IMG_CONST IMG_CHAR *pszHeader, IMG_UINT32 ui32ContextNb, IMG_BOOL bDumpFields)
{
    if ( g_bRegisterDumpEnabled == IMG_FALSE )
    {
        return IMG_SUCCESS;
    }

    if ( context_nElem == 0 )// first time
    {
        context_nElem = sizeof(felix_context0_fields)/sizeof(FieldDefnListEl);
        KRN_CI_DebugRegistersSort(felix_context0_fields, context_nElem);
    }

    if ( hFile == NULL || ui32ContextNb >= CI_N_CONTEXT)
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    if ( g_psCIDriver == NULL )
    {
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hRegFelixContext[ui32ContextNb], "register dump");

    if ( pszHeader != NULL )
    {
        if ( IMG_FILEIO_WriteToFile(hFile, pszHeader, strlen(pszHeader)) != IMG_SUCCESS )
        {
            CI_FATAL("failed to write header before dumping Felix Context %d registers\n", ui32ContextNb);
            return IMG_ERROR_FATAL;
        }
    }

    if ( KRN_CI_DebugRegistersDump(hFile, felix_context0_fields, context_nElem, g_psCIDriver->sTalHandles.hRegFelixContext[ui32ContextNb], bDumpFields, IMG_FALSE) != IMG_SUCCESS )
    {
        CI_FATAL("failed to dump Felix Context %d registers\n", ui32ContextNb);
        return IMG_ERROR_FATAL;
    }

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hRegFelixContext[ui32ContextNb], "dump done");

    return IMG_SUCCESS;
}

IMG_RESULT KRN_CI_DebugDumpRegisters(IMG_CONST IMG_CHAR *pszFilename, IMG_CONST IMG_CHAR *pszHeader, IMG_BOOL bDumpFields, IMG_BOOL bAppendFile)
{
    IMG_CHAR header[500];
    IMG_HANDLE hFile = NULL;
    IMG_BOOL bCompress = FELIX_DUMP_COMPRESSED_REGISTERS;
    IMG_UINT32 i;
    IMG_RESULT iRet = IMG_SUCCESS;

    if ( pszFilename == NULL )
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    if (IMG_FILEIO_UsesZLib() == IMG_FALSE && bCompress == IMG_TRUE)
    {
        CI_WARNING("FileIO library does not have zlib - cannot write compressed registers\n");
        bCompress = IMG_FALSE;
    }

    if ( (iRet=IMG_FILEIO_OpenFile(pszFilename, bAppendFile == IMG_TRUE ? "a" : "w", &hFile, bCompress)) != IMG_SUCCESS )
    {
        CI_FATAL("failed to open '%s'\n", pszFilename);
        return iRet;
    }

    if ( pszHeader != NULL ) sprintf(header, "%s - core registers:\n", pszHeader);
    else sprintf(header, "core registers:\n");

    if ( (iRet=KRN_CI_DebugDumpCore(hFile, header, bDumpFields)) != IMG_SUCCESS )
    {
        CI_FATAL("failed to dump the core registers\n");
        IMG_FILEIO_CloseFile(hFile);
        return iRet;
    }

    for ( i = 0 ; i < g_psCIDriver->sHWInfo.config_ui8NContexts ; i++ )
    {
        if ( pszHeader != NULL ) sprintf(header, "%s - context %u registers:\n", pszHeader, i);
        else sprintf(header, "context %u registers:\n", i);

        if ( (iRet=KRN_CI_DebugDumpContext(hFile, header, i, bDumpFields)) != IMG_SUCCESS )
        {
            CI_FATAL("failed to dump the context %u registers\n", i);
            IMG_FILEIO_CloseFile(hFile);
            return iRet;
        }
    }

    IMG_FILEIO_CloseFile(hFile);
    return IMG_SUCCESS;
}

IMG_RESULT KRN_CI_DebugDumpShot(IMG_CONST KRN_CI_SHOT *pShot, IMG_CONST IMG_CHAR *pszFilename, IMG_CONST IMG_CHAR *pszHeader, IMG_BOOL bDumpFields, IMG_BOOL bAppendFile)
{
    IMG_CHAR header[500];
    IMG_HANDLE hFile = NULL;
    IMG_BOOL bCompress = FELIX_DUMP_COMPRESSED_REGISTERS;
    IMG_RESULT iRet = IMG_SUCCESS;
    IMG_SIZE off;

    if ( pShot == NULL )
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    if ( pszFilename == NULL )
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    if ( g_psCIDriver->sMMU.apDirectory[0] != NULL )
    {
        CI_WARNING("cannot dump shots with the MMU enabled\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    if (IMG_FILEIO_UsesZLib() == IMG_FALSE && bCompress == IMG_TRUE)
    {
        CI_WARNING("FileIO library does not have zlib - cannot write compressed registers\n");
        bCompress = IMG_FALSE;
    }

    if ( (iRet=IMG_FILEIO_OpenFile(pszFilename, bAppendFile == IMG_TRUE ? "a" : "w", &hFile, bCompress)) != IMG_SUCCESS )
    {
        CI_FATAL("failed to open '%s'\n", pszFilename);
        return iRet;
    }

    if ( pszHeader != NULL ) sprintf(header, "%s - shot structures:\n", pszHeader);
    else sprintf(header, "shot structures:\n");

    if ( (iRet = IMG_FILEIO_WriteToFile(hFile, header, IMG_STRLEN(header))) != IMG_SUCCESS )
    {
        CI_FATAL("failed to write header before dumping shot structures\n");
        IMG_FILEIO_CloseFile(hFile);
        return iRet;
    }

    sprintf(header, "pointers struct (0x%08"IMG_PTRDPR"X)\n", Platform_MemGetDevMem( &(pShot->hwLinkedList), 0 ));
    if ( (iRet = KRN_CI_DebugDumpPointersStruct(Platform_MemGetTalHandle(&(pShot->hwLinkedList), 0, &off), hFile, header, bDumpFields)) != IMG_SUCCESS )
    {
        CI_FATAL("failed to write the pointers struct\n");
        IMG_FILEIO_CloseFile(hFile);
        return iRet;
    }

    sprintf(header, "save struct (0x%08"IMG_PTRDPR"X)\n", Platform_MemGetDevMem( &(pShot->hwSave.sMemory), 0 ));
    if ( (iRet = KRN_CI_DebugDumpSaveStruct(Platform_MemGetTalHandle(&(pShot->hwSave.sMemory), 0, &off), hFile, header, bDumpFields)) != IMG_SUCCESS )
    {
        CI_FATAL("failed to write the save struct\n");
        IMG_FILEIO_CloseFile(hFile);
        return iRet;
    }

    sprintf(header, "config struct (0x%08"IMG_PTRDPR"X)\n", Platform_MemGetDevMem( &(pShot->hwLoadStructure.sMemory), 0 ));
    if ( (iRet = KRN_CI_DebugDumpLoadStruct(Platform_MemGetTalHandle(&(pShot->hwLoadStructure.sMemory), 0, &off), hFile, header, bDumpFields)) != IMG_SUCCESS )
    {
        CI_FATAL("failed to write the load struct\n");
        IMG_FILEIO_CloseFile(hFile);
        return iRet;
    }

    IMG_FILEIO_CloseFile(hFile);
    return IMG_SUCCESS;
}

/*
 * register override
 */

struct RegOvr {
    IMG_UINT32 ui32FieldId; // id in the field array after being sorted
    IMG_UINT32 ui32Value; // shifted and masked value - ready to be written in register
    IMG_UINT32 ui32FieldMask; // mask of the shifted value for the field
};

static sLinkedList_T aRegovr_store[CI_N_CONTEXT];
static IMG_BOOL8 bRegover_init = IMG_FALSE;

sLinkedList_T* getRegovr_store(int ctx)
{
    if ( ctx < CI_N_CONTEXT )
        return &(aRegovr_store[ctx]);
    return NULL;
}

static void IMG_CI_DebugFreeRegovr(void *listElem)
{
    struct RegOvr *pReg = (struct RegOvr*)listElem;

    IMG_FREE(pReg);
}

IMG_RESULT KRN_CI_DebugRegisterOverrideInit(void)
{
    IMG_UINT32 i;

    for ( i = 0 ; i < CI_N_CONTEXT ; i++ )
    {
        List_init(&(aRegovr_store[i]));
    }
    bRegover_init = IMG_TRUE;

    return IMG_SUCCESS;
}

IMG_RESULT KRN_CI_DebugRegisterOverrideFinalise(void)
{
    IMG_UINT32 i;

    if ( bRegover_init == IMG_FALSE )
    {
        return IMG_SUCCESS; // the lists are not ready!
    }

    for ( i = 0 ; i < CI_N_CONTEXT ; i++ )
    {
        List_clearObjects(&(aRegovr_store[i]), &IMG_CI_DebugFreeRegovr);
    }

    return IMG_SUCCESS;
}

#define BUFFER_N 1024
#define INT_N 64
IMG_RESULT CI_DebugSetRegisterOverride(IMG_UINT32 ui32Context, IMG_CONST IMG_CHAR *pszFilename)
{
    sLinkedList_T *pList = &(aRegovr_store[ui32Context]);
    FILE *file = NULL;
    IMG_CHAR buffer[BUFFER_N];
    IMG_CHAR bankNameDB[INT_N];
    IMG_CHAR bankName[INT_N], fieldName[INT_N];
    IMG_UINT32 ui32Value;
    IMG_UINT32 i;

    if ( load_struct_nElem == 0 ) // first time
    {
        load_struct_nElem = sizeof(felix_load_structure_fields)/sizeof(FieldDefnListEl);
        KRN_CI_DebugRegistersSort(felix_load_structure_fields, load_struct_nElem);
    }

    if ( g_psCIDriver == NULL )
    {
        CI_FATAL("driver not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    if ( ui32Context > g_psCIDriver->sHWInfo.config_ui8NContexts )
    {
        CI_FATAL("HW supports only %d context - try to overwrite context %d\n",
                g_psCIDriver->sHWInfo.config_ui8NContexts, ui32Context);
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    List_clearObjects(pList, &IMG_CI_DebugFreeRegovr); // clear the list of current objects

    if ( pszFilename == NULL )
    {
        return IMG_SUCCESS;
    }

    if ( (file = fopen(pszFilename, "r")) == NULL )
    {
        CI_FATAL("Failed to open register overwrite file '%s'\n", pszFilename);
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    sprintf(bankNameDB, "REG_FELIX_CONTEXT_%d", ui32Context);

    while (!feof(file))
    {
        if ( fgets(buffer, BUFFER_N, file) == NULL )
        {
            break; // could not read - it's the end
        }

        if ( sscanf(buffer, "%s %s 0x%x\n", bankName, fieldName, &ui32Value) != 3 )
        {
            continue; // next line
        }

        if ( IMG_STRCMP(bankName, bankNameDB) == 0 ) // correct bank
        {
            struct RegOvr *neo = (struct RegOvr *)IMG_CALLOC(1, sizeof(struct RegOvr));

            neo->ui32FieldId = load_struct_nElem; // not possible

            // slow search
            for ( i = 0 ; i < load_struct_nElem && neo->ui32FieldId == load_struct_nElem ; i++ )
            {
                if ( strcmp(felix_load_structure_fields[i].szFieldName, fieldName) == 0 ) // field found
                {
                    neo->ui32FieldId = i;

                    neo->ui32FieldMask = felix_load_structure_fields[i].ui32FieldEnd - felix_load_structure_fields[i].ui32FieldStart + 1; // size of the field in bits
                    neo->ui32FieldMask = (0xFFFFFFFF>>(32-neo->ui32FieldMask)<<(felix_load_structure_fields[i].ui32FieldStart));

                    neo->ui32Value = (ui32Value << (felix_load_structure_fields[i].ui32FieldStart));

                    if ( neo->ui32Value != 0 && ( (neo->ui32Value)&(~neo->ui32FieldMask)) != 0 )
                    {
                        CI_FATAL("invalid register override value 0x%x for field %s\n", ui32Value, felix_load_structure_fields[i].szFieldName);
                        IMG_FREE(neo);
                       fclose(file);
                        return IMG_ERROR_FATAL;
                    }
                }
            }

            if ( neo->ui32FieldId == load_struct_nElem ) // not found
            {
                CI_WARNING("not found: %s %s 0x%x\n", bankName, fieldName, ui32Value);
                IMG_FREE(neo);
            }
            else
            {
                List_pushBackObject(pList, neo);
            }
        }
        else
        {
            CI_WARNING("not used: %s %s 0x%x\n", bankName, fieldName, ui32Value);
        }
    }

    fclose(file);
    return IMG_SUCCESS;
}

static IMG_BOOL8 IMG_VisitorApplyOverride(void *listElem, void *elem)
{
    SYS_MEM *pMemory = (SYS_MEM *)elem;
    struct RegOvr *pReg = (struct RegOvr *)listElem;
    FieldDefnListEl *fieldInfo = &(felix_load_structure_fields[pReg->ui32FieldId]);
    IMG_UINT32 ui32Val = 0;

    SYS_MemReadWord(pMemory, fieldInfo->ui32RegOffset, &ui32Val);

#ifdef FELIX_FAKE
    {
        char mess[64];
        IMG_MEMSET(mess, 0, 64*sizeof(char));
        sprintf(mess, "read value 0x%x=0x%x", fieldInfo->ui32RegOffset, ui32Val);
        CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, mess);
    }
#endif

    ui32Val = ((ui32Val) & ~(pReg->ui32FieldMask)) | (pReg->ui32Value);

    SYS_MemWriteWord(pMemory, fieldInfo->ui32RegOffset, ui32Val);

    return IMG_TRUE; // continue
}

IMG_RESULT KRN_CI_DebugRegisterOverride(IMG_UINT32 ui32Context, SYS_MEM *pMemory)
{
    char message[64];

    if ( load_struct_nElem == 0 ) // first time
    {
        // if it was not initialised then no register override was done from user-space!
        load_struct_nElem = sizeof(felix_load_structure_fields)/sizeof(FieldDefnListEl);
        KRN_CI_DebugRegistersSort(felix_load_structure_fields, load_struct_nElem);
    }

    if ( aRegovr_store[ui32Context].ui32Elements > 0 )
    {
        sprintf(message, "register overwrite ctx %d", ui32Context);
        CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, message);

        //Platform_MemUpdateHost(pMemory); // done in caller
        List_visitor(&(aRegovr_store[ui32Context]), pMemory, &IMG_VisitorApplyOverride);
        //Platform_MemUpdateDevice(pMemory); // done after call

        sprintf(message, "register overwrite ctx %d - done", ui32Context);
        CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, message);

        return IMG_SUCCESS;
    }

    return IMG_ERROR_CANCELLED; // no update done
}

/*
 * register testing
 */

void KRN_CI_DebugReg(IMG_HANDLE hTalHandle, FieldDefnListEl *aSortedBlock, IMG_SIZE nElem, IMG_BOOL8 bWrite)
{
    IMG_UINT32 i;
    FieldDefnListEl *pPrevious = NULL, *pCurrent = NULL;
    IMG_UINT32 val;

    IMG_ASSERT(aSortedBlock != NULL);
    IMG_ASSERT(nElem > 1);

    pPrevious = &(aSortedBlock[nElem-1]); // cheat on previously written one to trigger the first write
    for ( i = 0 ; i < nElem ; i++ )
    {
        pCurrent = &(aSortedBlock[i]);

        // different register than previously written one
        if ( pCurrent->ui32RegOffset != pPrevious->ui32RegOffset )
        {
            size_t len = strlen(pCurrent->szRegName);

            if ( strncmp(&(pCurrent->szRegName[len-5]), "_ADDR", 5) == 0 )
            {
                char mess[256];
                sprintf(mess, "register %s is an address", pCurrent->szRegName);
                CI_PDUMP_COMMENT(hTalHandle, mess);
            }
            else
            {
                TALREG_ReadWord32(hTalHandle, pCurrent->ui32RegOffset, &val);
                TALREG_Poll32(hTalHandle, pCurrent->ui32RegOffset, TAL_CHECKFUNC_ISEQUAL, val, ~0, CI_REGPOLL_TRIES, CI_REGPOLL_TIMEOUT);
                if ( bWrite )
                {
                    TALREG_WriteWord32(hTalHandle, pCurrent->ui32RegOffset, 0xAA);
                }
            }
            pPrevious = pCurrent;
        }
    }
}

IMG_RESULT KRN_CI_DebugRegistersTest(void)
{
    unsigned int i;

    IMG_ASSERT(g_psCIDriver != NULL);

    TALPDUMP_CaptureStop(); // remove the registers dumping that was done before
    KRN_CI_DriverStartPdump(g_psCIDriver, 1); // single pdump

    KRN_CI_MMUPause(&(g_psCIDriver->sMMU), IMG_TRUE);

    if ( core_nElem == 0 )// first time
    {
        core_nElem = sizeof(felix_core_fields)/sizeof(FieldDefnListEl);
        KRN_CI_DebugRegistersSort(felix_core_fields, core_nElem);
    }

    if ( context_nElem == 0 )// first time
    {
        context_nElem = sizeof(felix_context0_fields)/sizeof(FieldDefnListEl);
        KRN_CI_DebugRegistersSort(felix_context0_fields, context_nElem);
    }

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hRegFelixCore, "read-write all core registers");
    KRN_CI_DebugReg(g_psCIDriver->sTalHandles.hRegFelixCore, felix_core_fields, core_nElem, IMG_TRUE);

    for ( i = 0 ; i < g_psCIDriver->sHWInfo.config_ui8NContexts ; i++ )
    {
        char buff[64];
        sprintf(buff, "read-write all context %d registers", i);
        CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hRegFelixContext[i], buff);
        KRN_CI_DebugReg(g_psCIDriver->sTalHandles.hRegFelixContext[i], felix_context0_fields, context_nElem, IMG_TRUE);
    }

#ifdef FELIX_HAS_DG
    KRN_DG_DebugRegistersTest();
#endif

    return IMG_SUCCESS;
}

IMG_RESULT CI_DebugLoadTest(IMG_UINT8 ctx)
{
    char buff[64];

    IMG_ASSERT(g_psCIDriver != NULL);

    if ( context_nElem == 0 )// first time
    {
        context_nElem = sizeof(felix_context0_fields)/sizeof(FieldDefnListEl);
        KRN_CI_DebugRegistersSort(felix_context0_fields, context_nElem);
    }

    if ( ctx >= g_psCIDriver->sHWInfo.config_ui8NContexts )
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    sprintf(buff, "verify context %d registers", ctx);
    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hRegFelixContext[ctx], buff);
    TALREG_WriteWord32(g_psCIDriver->sTalHandles.hRegFelixContext[ctx], FELIX_CONTEXT0_INTERRUPT_CLEAR_OFFSET, ~0);
    KRN_CI_DebugReg(g_psCIDriver->sTalHandles.hRegFelixContext[ctx], felix_context0_fields, context_nElem, IMG_FALSE);
    sprintf(buff, "verify context %d registers - done", ctx);
    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hRegFelixContext[ctx], buff);

    return IMG_SUCCESS;
}

IMG_RESULT CI_DebugGMALut()
{
    IMG_ASSERT(g_psCIDriver != NULL);

    if ( gamma_nElem == 0 )// first time
    {
        gamma_nElem = sizeof(felix_gamma_lut_fields)/sizeof(FieldDefnListEl);
        KRN_CI_DebugRegistersSort(felix_gamma_lut_fields, gamma_nElem);
    }

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hRegGammaLut, "verify GMA Lut registers");
    KRN_CI_DebugReg(g_psCIDriver->sTalHandles.hRegGammaLut, felix_gamma_lut_fields, gamma_nElem, IMG_FALSE);
    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hRegGammaLut, "verify GMA Lut registers done");

    return IMG_SUCCESS;
}

IMG_RESULT KRN_CI_CRCCheck(KRN_CI_SHOT *pShot)
{
    IMG_UINT32 off = SAVE_STRUCT_CRC_FIRST;
    const IMG_UINT32 end = SAVE_STRUCT_CRC_LAST;
    IMG_UINT32 val = 0;
    IMG_RESULT ret = IMG_SUCCESS;
    IMG_HANDLE hMemHandle = NULL;
    IMG_SIZE uiOffset;

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "check CRC");

    SYS_MemReadWord(&(pShot->hwLinkedList), FELIX_LINK_LIST_SAVE_CONFIG_FLAGS_OFFSET, &val);
    if ( REGIO_READ_FIELD(val, FELIX_LINK_LIST, SAVE_CONFIG_FLAGS, CRC_ENABLE) == 0 )
    {
        CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "   CRC disabled - expecting 0");
    }

    IMG_ASSERT(off < end);
    IMG_ASSERT(end < pShot->hwSave.sMemory.uiAllocated);
    while ( off <= end )
    {
        if ( (hMemHandle=Platform_MemGetTalHandle(&(pShot->hwSave.sMemory), off, &uiOffset)) == NULL )
        {
            ret = IMG_ERROR_FATAL;
            CI_FATAL("failed to get memory handle for offset %uB in Save structure\n", off);
            break;
        }

        TALMEM_ReadWord32(hMemHandle, uiOffset, &val);
        TALMEM_Poll32(hMemHandle, uiOffset, TAL_CHECKFUNC_ISEQUAL, val, ~0, CI_REGPOLL_TRIES, CI_REGPOLL_TIMEOUT);

        off += 4;
    }

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "check CRC done");
    return ret;
}

void CI_DebugTestIO(void)
{
#ifdef FELIX_HAS_DG
    IMG_UINT32 currentReg;
    IMG_ASSERT(g_pDGDriver != NULL);

    CI_PDUMP_COMMENT(g_pDGDriver->hRegTestIO, "start");
    // TEST IO registers are only available if the external dg is there too
    currentReg = 0;
    TALREG_ReadWord32(g_pDGDriver->hRegTestIO, FELIX_TEST_IO_ENC_FRAME_COUNTER_OFFSET, &currentReg);
    TALREG_Poll32(g_pDGDriver->hRegTestIO, FELIX_TEST_IO_ENC_FRAME_COUNTER_OFFSET, TAL_CHECKFUNC_ISEQUAL, currentReg, ~0, CI_REGPOLL_TRIES, CI_REGPOLL_TIMEOUT);

    currentReg = 0;
    TALREG_ReadWord32(g_pDGDriver->hRegTestIO, FELIX_TEST_IO_ENC_LINE_COUNTER_OFFSET, &currentReg);
    TALREG_Poll32(g_pDGDriver->hRegTestIO, FELIX_TEST_IO_ENC_LINE_COUNTER_OFFSET, TAL_CHECKFUNC_ISEQUAL, currentReg, ~0, CI_REGPOLL_TRIES, CI_REGPOLL_TIMEOUT);

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hRegFelixCore, "done");
#endif
}

#endif // CI_DEBUG_FCT
