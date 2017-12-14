/**
*******************************************************************************
@file dg_debug.c

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

#include "dg_kernel/dg_debug.h"
#include "dg_kernel/dg_camera.h"  // access to g_pDGDriver

#include <registers/fields_ext_data_generator.h>
#include <registers/ext_data_generator.h>
#include <reg_io2.h>
#include <tal.h>

#ifdef CI_DEBUG_FCT
#include <gzip_fileio.h>

#ifndef FELIX_DUMP_COMPRESSED_REGISTERS
#define FELIX_DUMP_COMPRESSED_REGISTERS IMG_FALSE
#endif

static FELIX_TEST_DG_FIELDS  // felix_test_dg_fields
static IMG_SIZE gNElem = 0;

IMG_RESULT KRN_DG_DebugDumpDataGenerator(IMG_HANDLE hFile,
    const char *pszHeader, IMG_UINT32 ui32ContextNb, IMG_BOOL bDumpFields)
{
    IMG_RESULT ret;
    if (gNElem == 0)  // first time
    {
        gNElem = sizeof(felix_test_dg_fields) / sizeof(FieldDefnListEl);
        KRN_CI_DebugRegistersSort(felix_test_dg_fields, gNElem);
    }
    if (hFile == NULL || ui32ContextNb >= CI_N_EXT_DATAGEN)
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    if (g_pDGDriver == NULL)
    {
        return IMG_ERROR_UNEXPECTED_STATE;
    }
    if (pszHeader != NULL)
    {
        ret = IMG_FILEIO_WriteToFile(hFile, pszHeader, strlen(pszHeader));
        if (ret != IMG_SUCCESS)
        {
            CI_FATAL("failed to write header before dumping Data "\
                "Generator %d registers\n", ui32ContextNb);
            return IMG_ERROR_FATAL;
        }
    }
    ret = KRN_CI_DebugRegistersDump(hFile, felix_test_dg_fields, gNElem,
        g_pDGDriver->hRegFelixDG[ui32ContextNb], bDumpFields, IMG_FALSE);
    if (ret != IMG_SUCCESS)
    {
        CI_FATAL("failed to dump Data Generator %d registers\n",
            ui32ContextNb);
        return IMG_ERROR_FATAL;
    }
    return IMG_SUCCESS;
}
IMG_RESULT KRN_DG_DebugDumpRegisters(const char *pszFilename,
    const char *pszHeader, IMG_BOOL bDumpFields, IMG_BOOL bAppendFile)
{
    IMG_CHAR header[500];
    IMG_HANDLE hFile = NULL;
    IMG_BOOL bCompress = FELIX_DUMP_COMPRESSED_REGISTERS;
    IMG_UINT32 i;
    IMG_RESULT iRet = IMG_SUCCESS;
    if (pszFilename == NULL)
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    if (IMG_FILEIO_UsesZLib() == IMG_FALSE && bCompress == IMG_TRUE)
    {
        CI_WARNING("FileIO library does not have zlib - cannot write "\
            "compressed registers\n");
        bCompress = IMG_FALSE;
    }
    iRet = IMG_FILEIO_OpenFile(pszFilename,
        bAppendFile == IMG_TRUE ? "a" : "w", &hFile, bCompress);
    if (iRet != IMG_SUCCESS)
    {
        CI_FATAL("failed to open '%s'\n", pszFilename);
        return iRet;
    }
    for (i = 0; i < CI_N_EXT_DATAGEN; i++)
    {
        if (pszHeader != NULL)
        {
            snprintf(header, sizeof(header),
                "%s - data generator %u registers:\n",
                pszHeader, i);
        }
        else
        {
            snprintf(header, sizeof(header), "context %u registers:\n", i);
        }
        iRet = KRN_DG_DebugDumpDataGenerator(hFile, header, i, bDumpFields);
        if (iRet != IMG_SUCCESS)
        {
            CI_FATAL("failed to dump the data generator  %u registers\n", i);
            IMG_FILEIO_CloseFile(hFile);
            return iRet;
        }
    }
    IMG_FILEIO_CloseFile(hFile);
    return IMG_SUCCESS;
}
IMG_RESULT KRN_DG_DebugRegistersTest(void)
{
    IMG_UINT32 i;
    if (gNElem == 0)  // first time
    {
        gNElem = sizeof(felix_test_dg_fields) / sizeof(FieldDefnListEl);
        KRN_CI_DebugRegistersSort(felix_test_dg_fields, gNElem);
    }
    for (i = 0; i < g_pDGDriver->sHWInfo.config_ui8NDatagen; i++)
    {
        char buff[64];
        snprintf(buff, sizeof(buff), "read-write all datagen %d registers", i);
        CI_PDUMP_COMMENT(g_pDGDriver->hRegFelixDG[i], buff);
        KRN_CI_DebugReg(g_pDGDriver->hRegFelixDG[i], felix_test_dg_fields,
            gNElem, IMG_TRUE);
    }
    return IMG_SUCCESS;
}

#endif /* CI_DEBUG_FCT */

#if defined(FELIX_FAKE)

static IMG_BOOL8 g_bUseTransif = IMG_FALSE;

IMG_BOOL8 DG_SetUseTransif(IMG_BOOL8 bUseTransif)
{
    IMG_BOOL8 bprev = g_bUseTransif;
    g_bUseTransif = bUseTransif;
    return bprev;
}

void DG_WaitForMaster(int dg)
{
    if (g_bUseTransif)  // check if it has pending frames...
    {
        IMG_ASSERT(g_pDGDriver != NULL);
        IMG_ASSERT(dg < CI_N_EXT_DATAGEN);
        IMG_ASSERT(g_pDGDriver->aActiveCamera[dg] != NULL);

        //  should it be protected by sPendingLock???
        if (g_pDGDriver->aActiveCamera[dg]->sList_pending.ui32Elements > 0)
        {
            IMG_UINT32 tmpReg = ~0;
            printf("wait for master request (dg=%d)...\n", dg);
            do
            {
                // Let Master Requests (sent from simulator) to be handled...
                TAL_Wait(g_pDGDriver->hMemHandle, 1);
                TALREG_ReadWord32(g_pDGDriver->hRegFelixDG[dg],
                    FELIX_TEST_DG_DG_INTER_STATUS_OFFSET, &tmpReg);
            } while (!REGIO_READ_FIELD(tmpReg, FELIX_TEST_DG, DG_INTER_STATUS,
                DG_INT_END_OF_FRAME));
        }
    }
}

#endif /* FELIX_FAKE */
