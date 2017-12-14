/**
******************************************************************************
@file ci_debug_test.cpp

@brief CI API internal tests

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
#include <gtest/gtest.h>

#include <cstdlib>
#include <cstdio>

#include <img_types.h>
#include <img_defs.h>
#include <img_errors.h>

#include "unit_tests.h"
#include "felix.h"

#include "ci_kernel/ci_debug.h"
#include "ci_kernel/ci_ioctrl.h"

#ifdef FELIX_HAS_DG
#include "dg_kernel/dg_camera.h"
#include "dg_kernel/dg_debug.h"
#endif

#include <tal.h>
#include <reg_io2.h>
#include <hw_struct/ctx_config.h>

#ifdef CI_DEBUG_FCT

TEST(RegDump, CIdefaults)
{
	Felix driver;
	driver.configure();

	EXPECT_EQ (IMG_SUCCESS, KRN_CI_DebugDumpRegisters("default_CI.txt", "default values (no fields)", IMG_FALSE, IMG_FALSE));
	EXPECT_EQ (IMG_SUCCESS, KRN_CI_DebugDumpRegisters("default_CI.txt", "default values (with fields)", IMG_TRUE, IMG_TRUE));

	// finalise drivers here
}

#ifdef FELIX_HAS_DG
TEST(RegDump, DGdefaults)
{
	Felix driver;
	KRN_DG_DRIVER sDGDriver;
	driver.configure();

    // default PLL and disabled MMU
	ASSERT_EQ (IMG_SUCCESS, KRN_DG_DriverCreate(&sDGDriver, 0, 0, 0));

	EXPECT_EQ (IMG_SUCCESS, KRN_DG_DebugDumpRegisters("default_DG.txt", "default values (no fields)", IMG_FALSE, IMG_FALSE));
	//EXPECT_EQ (IMG_SUCCESS, KRN_DG_DebugDumpRegisters("default_DG.txt", "default values (with fields)", IMG_TRUE, IMG_TRUE));

	EXPECT_EQ (IMG_SUCCESS, KRN_DG_DriverDestroy(&sDGDriver));
	// finalise drivers here
}
#endif

TEST(RegOverride, load)
{
	EXPECT_EQ(IMG_ERROR_NOT_INITIALISED, CI_DebugSetRegisterOverride(0, "testdata/regoverwrite.txt")) << "driver not initialised";

	Felix driver;
	driver.configure();

	EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, CI_DebugSetRegisterOverride(0, "donotexists.txt")) << "file does not exists";
	EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, CI_DebugSetRegisterOverride(5, "testdata/regoverwrite.txt")) << "HW does not support that many context";

	EXPECT_EQ(IMG_SUCCESS, CI_DebugSetRegisterOverride(0, "testdata/regoverwrite.txt"));
	EXPECT_EQ(IMG_SUCCESS, CI_DebugSetRegisterOverride(1, "testdata/regoverwrite.txt"));

    EXPECT_EQ(3, getRegovr_store(0)->ui32Elements);
	EXPECT_EQ(1, getRegovr_store(1)->ui32Elements);

	// finalise driver here
}

TEST(RegOverride, override)
{
	Felix driver;
	driver.configure();

	SYS_MEM sMemory;
	IMG_MEMSET(&sMemory, 0, sizeof(SYS_MEM));

	ASSERT_EQ(IMG_SUCCESS, SYS_MemAlloc(&sMemory, FELIX_LOAD_STRUCTURE_BYTE_SIZE, NULL, 0)) << "failed to allocate memstruct";
#ifndef CI_MEMSET_ALIGNMENT
	{
		// wasn't memset so we need to do it
		void *add = Platform_MemGetMemory(&sMemory);

		IMG_MEMSET(add, 0xAA, sMemory.uiAllocated);

		Platform_MemUpdateDevice(&sMemory);
	}
#endif

	EXPECT_EQ(IMG_SUCCESS, CI_DebugSetRegisterOverride(0, "testdata/regoverwrite.txt"));

	EXPECT_EQ(IMG_SUCCESS, KRN_CI_DebugRegisterOverride(0, &sMemory));

	// verify it was written
	{
		IMG_UINT32 ui32RegVal = 0;
		IMG_UINT32 *pMemory = (IMG_UINT32*)Platform_MemGetMemory(&sMemory);

		//TALMEM_ReadWord32(sMemory.hMem, FELIX_LOAD_STRUCTURE_LAT_CA_OFFSET_RED_OFFSET, &ui32RegVal);
		ui32RegVal = pMemory[FELIX_LOAD_STRUCTURE_LAT_CA_OFFSET_RED_OFFSET/4];

		EXPECT_EQ(0x12, REGIO_READ_FIELD(ui32RegVal, FELIX_LOAD_STRUCTURE, LAT_CA_OFFSET_RED, LAT_CA_OFFSET_Y_RED));
		EXPECT_EQ(0x13, REGIO_READ_FIELD(ui32RegVal, FELIX_LOAD_STRUCTURE, LAT_CA_OFFSET_RED, LAT_CA_OFFSET_X_RED));

		//TALMEM_ReadWord32(sMemory.hMem, REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE, TNM_GLOBAL_CURVE, 3), &ui32RegVal);
		ui32RegVal = pMemory[REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE, TNM_GLOBAL_CURVE, 3)/4];

		EXPECT_EQ(0xC0, REGIO_READ_FIELD(ui32RegVal, FELIX_LOAD_STRUCTURE, TNM_GLOBAL_CURVE, TNM_CURVE_POINT_1));
	}

	// clean memory
	{
		// wasn't memset so we need to do it
		void *add = Platform_MemGetMemory(&sMemory);

		IMG_MEMSET(add, 0xAA, sMemory.uiAllocated);

		Platform_MemUpdateDevice(&sMemory);
	}

	EXPECT_EQ(IMG_SUCCESS, CI_DebugSetRegisterOverride(1, "testdata/regoverwrite.txt"));

	EXPECT_EQ(IMG_SUCCESS, KRN_CI_DebugRegisterOverride(1, &sMemory));

	// verify it was written
	{
		IMG_UINT32 ui32RegVal = 0;
		IMG_UINT32 *pMemory = (IMG_UINT32*)Platform_MemGetMemory(&sMemory);

		//TALMEM_ReadWord32(sMemory.hMem, FELIX_LOAD_STRUCTURE_ENC_SCAL_H_PITCH_OFFSET, &ui32RegVal);
		ui32RegVal = pMemory[FELIX_LOAD_STRUCTURE_ENC_SCAL_H_PITCH_OFFSET/4];

		EXPECT_EQ(0x04, REGIO_READ_FIELD(ui32RegVal, FELIX_LOAD_STRUCTURE, ENC_SCAL_H_PITCH, ENC_SCAL_H_PITCH_INTEGER));
	}

	SYS_MemFree(&sMemory);
}

TEST(RegOverride, invalidOverride)
{
	Felix driver;
	driver.configure();

	SYS_MEM sMemory;
	IMG_MEMSET(&sMemory, 0, sizeof(SYS_MEM));

	ASSERT_EQ(IMG_SUCCESS, SYS_MemAlloc(&sMemory, FELIX_LOAD_STRUCTURE_BYTE_SIZE, NULL, 0)) << "failed to allocate memstruct";
#ifndef CI_MEMSET_ALIGNMENT
	{
		// wasn't memset so we need to do it
		void *add = Platform_MemGetMemory(&sMemory);

		IMG_MEMSET(add, 0xAA, sMemory.uiAllocated);

		Platform_MemUpdateDevice(&sMemory);
	}
#endif

	EXPECT_EQ(IMG_ERROR_FATAL, CI_DebugSetRegisterOverride(0, "testdata/regoverwrite_inv.txt"));
	EXPECT_EQ(IMG_ERROR_FATAL, CI_DebugSetRegisterOverride(1, "testdata/regoverwrite_inv.txt"));
	SYS_MemFree(&sMemory);
}

/**
 * @brief test that kernel side refuses access to banks that do not exists
 */
TEST(RegDebug, invalidBank)
{
    Felix driver;
    driver.configure();
    IMG_UINT32 res = 0;
    CI_CONNECTION *pConn = driver.getConnection();

    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS,
        CI_DriverDebugRegRead(pConn, CI_BANK_N, 0, &res));
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS,
        CI_DriverDebugRegWrite(pConn, CI_BANK_N, 0, res));

    if(pConn->sHWInfo.config_ui8NContexts < CI_N_CONTEXT)
    {
        EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS,
            CI_DriverDebugRegRead(pConn,
            CI_BANK_CTX + driver.getConnection()->sHWInfo.config_ui8NContexts,
            0, &res));
        EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS,
            CI_DriverDebugRegWrite(pConn,
            CI_BANK_CTX + pConn->sHWInfo.config_ui8NContexts,
            0, res));
    }

    if (pConn->sHWInfo.config_ui8NImagers < CI_N_IMAGERS)
    {
        EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS,
            CI_DriverDebugRegRead(pConn,
            CI_BANK_GASKET + pConn->sHWInfo.config_ui8NContexts,
            0, &res));
        EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS,
            CI_DriverDebugRegWrite(pConn,
            CI_BANK_GASKET + pConn->sHWInfo.config_ui8NContexts,
            0, res));
    }

    /* always tested because for the moment only HW with 1 IIFDG exist
     * and it's the last entry */
// if (driver.getConnection()->sHWInfo.config_ui8BitDepth < CI_N_IIF_DATAGEN)
    {
        EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS,
            CI_DriverDebugRegRead(pConn,
            CI_BANK_IIFDG
            + pConn->sHWInfo.config_ui8NIIFDataGenerator,
            0, &res));
        EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS,
            CI_DriverDebugRegWrite(driver.getConnection(),
            CI_BANK_IIFDG
            + pConn->sHWInfo.config_ui8NIIFDataGenerator,
            0, res));
    }
}

#endif // CI_DEBUG_FCT
