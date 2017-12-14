/**
*******************************************************************************
@file driver_init.cpp

@brief ISPC help - driver init implementation

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
//
// fake driver common code
//
#ifdef FELIX_FAKE

#include <ci_kernel/ci_kernel.h>
#ifdef EXT_DATAGEN
#include <dg_kernel/dg_camera.h>
#endif
#include <ci_kernel/ci_debug.h>

#ifdef ISPCTEST_TRANSIF
#include "transif_adaptor.h"
#endif
static IMG_HANDLE gTransifAdaptor = NULL;
#endif
#include <sys/sys_userio_fake.h>

#include <cstdlib>
#include <ispctest/driver_init.h>

#define LOG_TAG "ISPC_HELP"
#include <felixcommon/userlog.h>

#ifdef FELIX_FAKE
static SYS_DEVICE sDevice;
#endif

int initializeFakeDriver(unsigned int useMMU, int gmaCurve,
    unsigned int pagesize, unsigned int tilingScheme,
    unsigned int tilingStride,
    unsigned char edgPLL_mult, unsigned char edgPLL_div,
    char *pszTransifLib, char *pszTransifSimParam)
{
#ifdef FELIX_FAKE
    static KRN_CI_DRIVER sCIDriver;  // resilient
#ifdef EXT_DATAGEN
    static KRN_DG_DRIVER sDGDriver;  // resilient
#endif
    IMG_RESULT ret;

    sDevice.pszDeviceName = "FELIX";
    sDevice.probeSuccess = NULL;
    SYS_DevRegister(&sDevice);

    if (setPageSize(pagesize) != 0)
    {
        LOG_ERROR("Failed to set CPU page size to %u\n", pagesize);
        return EXIT_FAILURE;
    }

    if (pszTransifLib != NULL && pszTransifSimParam != NULL)
    {
#ifdef ISPCTEST_TRANSIF
        // init transif here
        TransifAdaptorConfig sAdaptorConfig = { 0 };
        const char* ppszArgs[2] = { "-argfile", pszTransifSimParam };
        bUseTCP = IMG_FALSE;

        sAdaptorConfig.nArgCount = 2;
        sAdaptorConfig.ppszArgs = (char**)ppszArgs;
        sAdaptorConfig.pszLibName = pszTransifLib;
        sAdaptorConfig.pszDevifName = sDevice.pszDeviceName;
        sAdaptorConfig.bTimedModel = IMG_FALSE;
        sAdaptorConfig.bDebug = IMG_FALSE;

        sAdaptorConfig.eMemType = IMG_MEMORY_MODEL_DYNAMIC;
        sAdaptorConfig.bForceAlloc = IMG_FALSE;

        gTransifAdaptor = TransifAdaptor_Create(&sAdaptorConfig);
        if (gTransifAdaptor == NULL)
        {
            LOG_ERROR("Failed to create transif layer\n");
            return EXIT_FAILURE;
        }
#else
        LOG_ERROR("Transif layer not available\n");
        return EXIT_FAILURE;
#endif
    }

    if (gmaCurve < 0)
    {
        gmaCurve = CI_DEF_GMACURVE;
    }

    LOG_INFO("Fake insmod: mmu=%d tilingScheme=%d tilingStride=%d, "\
        "gmaCurve=%d PAGE_SIZE=%d\n",
        useMMU, tilingScheme, tilingStride, gmaCurve, pagesize);

    ret = KRN_CI_DriverCreate(&sCIDriver, useMMU, tilingScheme, tilingStride,
        gmaCurve, &sDevice);
    if (ret)
    {
        LOG_ERROR("Failed to create global CI driver\n");
        return EXIT_FAILURE;
    }

#ifdef EXT_DATAGEN
    ret = KRN_DG_DriverCreate(&sDGDriver, edgPLL_mult, edgPLL_div, useMMU);
    if (ret)
    {
        LOG_ERROR("Failed to create global DG driver\n");
        KRN_CI_DriverDestroy(&sCIDriver);
        return EXIT_FAILURE;
    }
#endif
#endif // FELIX_FAKE
    return EXIT_SUCCESS;
}

int finaliseFakeDriver(void)
{
    LOG_INFO("Fake rmmod\n");
#ifdef FELIX_FAKE
#ifdef EXT_DATAGEN
    KRN_DG_DriverDestroy(g_pDGDriver);
#endif
    KRN_CI_DriverDestroy(g_psCIDriver);

#ifdef ISPCTEST_TRANSIF
    if (gTransifAdaptor != NULL)
    {
        TransifAdaptor_Destroy(gTransifAdaptor);
        gTransifAdaptor = NULL;
    }
#endif
    SYS_DevDeregister(&sDevice);
#endif // FELIX_FAKE
    return EXIT_SUCCESS;
}
