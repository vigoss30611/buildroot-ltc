/**
*******************************************************************************
@file ci_internal.c

@brief Implementation of the kernel-side Driver object

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

#include <tal.h>
#include <target.h>
#include <reg_io2.h>

#ifdef INFOTM_ISP
#include <mach/imap-iomap.h>
#include <mach/imap-isp.h>
#endif //INFOTM_ISP
#include "ci/ci_api_structs.h"
#include "ci_target.h"
#include "felix_hw_info.h"

#include "ci_kernel/ci_kernel_structs.h"
#include "ci_kernel/ci_kernel.h"
#include "ci_kernel/ci_debug.h"
#include "ci_kernel/ci_connection.h" // for IO functions
#include "ci_kernel/ci_hwstruct.h" // to load GMA LUT

#include "ci_internal/sys_device.h"

#include "linkedlist.h"

#ifdef FELIX_UNIT_TESTS
#include "unit_tests.h" // gfinittal
#endif

#include <mmulib/mmu.h>

#ifdef FELIX_FAKE // CI_MMU_TAL_ALLOC

#include <talalloc/talalloc.h>

#endif // FELIX_FAKE

KRN_CI_DRIVER *g_psCIDriver = NULL;

#ifdef FELIX_FAKE
#include <registers/test_io.h>
#endif

#include <stdarg.h>

/// @brief size of the tmp message
#define CI_LOG_TMP 512

#ifndef CI_LOG_LEVEL
#define CI_LOG_LEVEL CI_LOG_LEVEL_FATALS
#endif
#if CI_LOG_LEVEL < CI_LOG_LEVEL_QUIET
#error "CI_LOG_LEVEL too low"
#elif CI_LOG_LEVEL > CI_LOG_LEVEL_DBG
#error "CI_LOG_LEVEL too high"
#endif

void LOG_CI_Fatal(const char *function, IMG_UINT32 line,
    const char* format, ...)
{
    if (ciLogLevel >= CI_LOG_LEVEL_FATALS)
    {
        char _message_[CI_LOG_TMP];
        va_list args;

        va_start(args, format);

        vsprintf(_message_, format, args);

        va_end(args);

#ifndef IMG_KERNEL_MODULE
        fprintf(stderr, "ERROR [CI_KRN]: %s:%u %s", function, line, _message_);
#else
        printk(KERN_ERR "%s:%s:%u %s", TAL_TARGET_NAME, function, line,
            _message_);
#endif // FELIX_FAKE
    }
}

void LOG_CI_Warning(const char *function, IMG_UINT32 line, const char* format,
    ...)
{
    if (ciLogLevel >= CI_LOG_LEVEL_ERRORS)
    {
        char _message_[CI_LOG_TMP];
        va_list args;
        (void)line; // unused

        va_start(args, format);

        vsprintf(_message_, format, args);

        va_end(args);

#ifndef IMG_KERNEL_MODULE
        fprintf(stdout, "WARNING [CI_KRN]: %s %s", function, _message_);
#else
        printk(KERN_WARNING "%s:%s %s", TAL_TARGET_NAME, function, _message_);
#endif // FELIX_FAKE
    }
}

void LOG_CI_Info(const char *function, IMG_UINT32 line, const char* format,
    ...)
{
    if (ciLogLevel >= CI_LOG_LEVEL_ALL)
    {
        char _message_[CI_LOG_TMP];
        va_list args;
        (void)line; // unused
        (void)function; // unused

        va_start(args, format);

        vsprintf(_message_, format, args);

        va_end(args);

#ifndef IMG_KERNEL_MODULE
        fprintf(stdout, "INFO [CI_KRN]: %s", _message_);
#else
        printk(KERN_INFO "%s %s", TAL_TARGET_NAME, _message_);
#endif // FELIX_FAKE
    }
}

void LOG_CI_Debug(const char *function, IMG_UINT32 line, const char* format,
    ...)
{
    if (ciLogLevel >= CI_LOG_LEVEL_DBG)
    {
        char _message_[CI_LOG_TMP];
        va_list args;
        (void)line; // unused

        va_start(args, format);

        vsprintf(_message_, format, args);

        va_end(args);

#ifndef IMG_KERNEL_MODULE
        fprintf(stdout, "DEBUG [CI_KRN]: %s %s", function, _message_);
#else
        printk(KERN_WARNING "%s DBG:%s %s", TAL_TARGET_NAME, function,
            _message_);
#endif // FELIX_FAKE
    }
}

int LOG_CI_LogLevel(void)
{
    return CI_LOG_LEVEL;
}

void LOG_CI_PdumpComment(const char *function, IMG_HANDLE talHandle,
    const char* message)
{
#ifdef FELIX_FAKE
    IMG_CHAR _message_[CI_LOG_TMP];

    sprintf(_message_, "%s - %s", function, message);

    TALPDUMP_Comment(talHandle, _message_);
#else
    (void)talHandle;
    LOG_CI_Debug(function, 0, message);
#endif
}

#ifndef __maybe_unused
#define __maybe_unused
#endif

/**
 * @warning assumes that g_psCIDriver is not NULL
 */
static void IMG_CI_DriverDefaultLinestore(KRN_CI_DRIVER *pDriver)
{
    IMG_INT32 i;
    IMG_UINT32 total = 0;

    CI_HWINFO *pInfo = &(pDriver->sHWInfo);

    // start from the last one
    for (i = pInfo->config_ui8NContexts - 1; i >= 0; i--)
    {
        IMG_UINT32 size = 0;
        pDriver->sLinestoreStatus.aStart[i] =
            i*(pInfo->ui32MaxLineStore / pInfo->config_ui8NContexts);

        // if there is a following context
        if (i + 1 < pInfo->config_ui8NContexts)
        {
            size = pDriver->sLinestoreStatus.aStart[i + 1]
                - pDriver->sLinestoreStatus.aStart[i];
        }
        else
        {
            size = pInfo->ui32MaxLineStore
                - pDriver->sLinestoreStatus.aStart[i];
        }

        // if too big shift to the right
        if (size > pInfo->context_aMaxWidthMult[i])
        {
            pDriver->sLinestoreStatus.aStart[i] +=
                size - pInfo->context_aMaxWidthMult[i];
            size = pInfo->context_aMaxWidthMult[i];
        }
        pDriver->sLinestoreStatus.aSize[i] = size;

        total += size;
    }

    IMG_ASSERT(total <= pInfo->ui32MaxLineStore);
}

/**
 * @brief Computes the remaining size for this context before the next active
 * context
 *
 * @warning assumes that sActiveContextLock AND sConnectionLock are already
 * locked!
 *
 * @warning assumes that given context is not active
 */
static IMG_SIZE IMG_CI_DriveComputeLinstoreSize(IMG_UINT8 ui8Context)
{
    IMG_UINT32 i;
    /* assumes context uses all it can because this computes the maximum size
     * the linestore can ever have */
    // checks for multiple context is done when starting
    IMG_INT32 size = g_psCIDriver->sHWInfo.context_aMaxWidthSingle[ui8Context];

    // recompute the based on the other contexts
    for (i = 0; i < g_psCIDriver->sHWInfo.config_ui8NContexts; i++)
    {
        if (i != ui8Context)
        {
            if (g_psCIDriver->sLinestoreStatus.aStart[i] >
                g_psCIDriver->sLinestoreStatus.aStart[ui8Context])
            {
                /* take the minimum: if context[i] is too far for context to
                 * reach it size is unchanged (but there is a gap)*/
                size = IMG_MIN_INT(size,
                    g_psCIDriver->sLinestoreStatus.aStart[i]
                    - g_psCIDriver->sLinestoreStatus.aStart[ui8Context]);
            }
        }
    }

    return size;
}

IMG_RESULT KRN_CI_DriverDefaultsGammaLUT(CI_MODULE_GMA_LUT *pGamma,
    IMG_UINT32 uiCurve)
{
    /* BT.709 according to reasearch computations (std curve + tweaking
     * to minimise error)*/
    // uses the same values for R/G/B channels
    // uiCurve == 0
    const static IMG_UINT16 BT709_curve[GMA_N_POINTS] =
    { 0, 72, 144, 180, 213, 243, 270, 320, 365, 406,
    444, 512, 573, 629, 680, 728, 773, 816, 857, 896,
    933, 969, 1003, 1069, 1131, 1189, 1245, 1298, 1349, 1398,
    1445, 1490, 1535, 1577, 1619, 1659, 1699, 1737, 1775, 1811,
    1847, 1882, 1917, 1950, 1984, 2016, 2048, 2079, 2110, 2141,
    2171, 2200, 2229, 2258, 2286, 2314, 2341, 2368, 2395, 2421,
    2447, 2473, 2499 };

    /* sRGB according to research computaions (std curve + tweaking
     * to minimise error)*/
    // uses the same values for R/G/B channels
    // uiCurve == 1

	const static IMG_UINT16 sRGB_curve1[GMA_N_POINTS] =      //[level 0]for lanchou project or imagination default
    { 0, 173, 269, 307, 340, 370, 397, 447, 491, 531,
    568, 634, 693, 747, 796, 841, 884, 925, 963, 999,
    1034, 1068, 1100, 1161, 1218, 1272, 1323, 1372, 1418, 1463,
    1506, 1547, 1587, 1626, 1664, 1700, 1736, 1770, 1804, 1837,
    1869, 1900, 1931, 1961, 1991, 2020, 2048, 2076, 2103, 2130,
    2157, 2183, 2208, 2234, 2259, 2283, 2307, 2331, 2355, 2378,
    2401, 2423, 2446 };

	const static IMG_UINT16 sRGB_curve2[GMA_N_POINTS] =	// [level 3]laow area will more brighter, middle area will keep gamma curve 2.2, hight area will over gamma curve 2.2
	{
		0,275,367,403,435,464,490,538,581,619,655,718,775,826,874,918,959,998,1035,1070,1103,1135,1166,1225,1280,1332,1381,1428,1473,
		1516,1557,1597,1635,1672,1709,1744,1778,1811,1843,1875,1906,1936,1966,1995,2023,2051,2078,2105,2131,2157,2183,2208,2232,2257,
		2281,2304,2328,2350,2373,2395,2417,2439,2461
	};
	const static IMG_UINT16 sRGB_curve3[GMA_N_POINTS] =   //level 1
	{
		0,
		242,
		327,
		360,
		390,
		417,
		441,
		486,
		527,
		563,
		597,
		658,
		712,
		761,
		807,
		849,
		889,
		926,
		962,
		996,
		1029,
		1060,
		1090,
		1148,
		1201,
		1252,
		1300,
		1346,
		1391,
		1433,
		1474,
		1513,
		1551,
		1588,
		1624,
		1659,
		1693,
		1726,
		1758,
		1789,
		1820,
		1850,
		1880,
		1909,
		1937,
		1965,
		1992,
		2019,
		2045,
		2071,
		2097,
		2122,
		2147,
		2171,
		2195,
		2219,
		2242,
		2265,
		2288,
		2310,
		2332,
		2354,
		2376
	};

	const static IMG_UINT16 sRGB_curve4[GMA_N_POINTS] = //level 2
	{
		0,
		261,
		349,
		383,
		413,
		440,
		465,
		511,
		551,
		588,
		621,
		682,
		736,
		784,
		829,
		871,
		910,
		947,
		982,
		1015,
		1047,
		1078,
		1107,
		1163,
		1215,
		1264,
		1311,
		1355,
		1398,
		1438,
		1478,
		1515,
		1552,
		1587,
		1622,
		1655,
		1687,
		1719,
		1750,
		1780,
		1809,
		1838,
		1866,
		1893,
		1920,
		1946,
		1972,
		1998,
		2023,
		2047,
		2072,
		2095,
		2119,
		2142,
		2165,
		2187,
		2209,
		2231,
		2252,
		2273,
		2294,
		2315,
		2335,
	};

	const static IMG_UINT16 sRGB_curve5[GMA_N_POINTS] = { //Gamma 	2.4
		0,	271,	362,	397,	429,	457,	483,	530,	572,	610,
		645,	708,	764,	814,	861,	904,	945,	983,	1020,	1054,
		1087,	1119,	1149,	1207,	1261,	1312,	1361,	1407,	1451,	1494,
		1534,	1574,	1611,	1648,	1684,	1718,	1752,	1785,	1817,	1848,
		1878,	1908,	1937,	1966,	1994,	2021,	2048,	2048,	2048,	2048,
		2048,	2048,	2048,	2048,	2048,	2048,	2048,	2048,	2048,	2048,
		2048,	2048,	2048
	};

    IMG_ASSERT(pGamma != NULL);

    if (uiCurve == 0)
    {
        CI_DEBUG("Using BT709 Gamma curve\n");
        IMG_MEMCPY(pGamma->aRedPoints, BT709_curve,
            GMA_N_POINTS*sizeof(IMG_UINT16));
        IMG_MEMCPY(pGamma->aGreenPoints, BT709_curve,
            GMA_N_POINTS*sizeof(IMG_UINT16));
        IMG_MEMCPY(pGamma->aBluePoints, BT709_curve,
            GMA_N_POINTS*sizeof(IMG_UINT16));
    }
    else if (uiCurve == 1)
    {
        CI_DEBUG("Using sRGB Gamma curve 1\n");
        IMG_MEMCPY(pGamma->aRedPoints, sRGB_curve1,
            GMA_N_POINTS*sizeof(IMG_UINT16));
        IMG_MEMCPY(pGamma->aGreenPoints, sRGB_curve1,
            GMA_N_POINTS*sizeof(IMG_UINT16));
        IMG_MEMCPY(pGamma->aBluePoints, sRGB_curve1,
            GMA_N_POINTS*sizeof(IMG_UINT16));
    }
    else if (2 == uiCurve)
    {
    	CI_DEBUG("Using sRGB Gamma curve 2\n");
    	IMG_MEMCPY(pGamma->aRedPoints, sRGB_curve2,
			GMA_N_POINTS*sizeof(IMG_UINT16));
		IMG_MEMCPY(pGamma->aGreenPoints, sRGB_curve2,
			GMA_N_POINTS*sizeof(IMG_UINT16));
		IMG_MEMCPY(pGamma->aBluePoints, sRGB_curve2,
			GMA_N_POINTS*sizeof(IMG_UINT16));
    }
    else if (3 == uiCurve)
    {
		CI_DEBUG("Using sRGB Gamma curve 3\n");
		IMG_MEMCPY(pGamma->aRedPoints, sRGB_curve3,
			GMA_N_POINTS*sizeof(IMG_UINT16));
		IMG_MEMCPY(pGamma->aGreenPoints, sRGB_curve3,
			GMA_N_POINTS*sizeof(IMG_UINT16));
		IMG_MEMCPY(pGamma->aBluePoints, sRGB_curve3,
			GMA_N_POINTS*sizeof(IMG_UINT16));
    }
    else if (4 == uiCurve)
	{
		CI_DEBUG("Using sRGB Gamma curve 4\n");
		IMG_MEMCPY(pGamma->aRedPoints, sRGB_curve4,
			GMA_N_POINTS*sizeof(IMG_UINT16));
		IMG_MEMCPY(pGamma->aGreenPoints, sRGB_curve4,
			GMA_N_POINTS*sizeof(IMG_UINT16));
		IMG_MEMCPY(pGamma->aBluePoints, sRGB_curve4,
			GMA_N_POINTS*sizeof(IMG_UINT16));
	}
    else if (5 == uiCurve)
	{
		CI_DEBUG("Using sRGB Gamma curve 5\n");
		IMG_MEMCPY(pGamma->aRedPoints, sRGB_curve5,
			GMA_N_POINTS*sizeof(IMG_UINT16));
		IMG_MEMCPY(pGamma->aGreenPoints, sRGB_curve5,
			GMA_N_POINTS*sizeof(IMG_UINT16));
		IMG_MEMCPY(pGamma->aBluePoints, sRGB_curve5,
			GMA_N_POINTS*sizeof(IMG_UINT16));
	}
    else
    {
        CI_FATAL("Given Gamma curve %u does not exists!\n", uiCurve);
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    return IMG_SUCCESS;
}

IMG_RESULT KRN_CI_DriverAcquireDPFBuffer(IMG_UINT32 prev, IMG_UINT32 wanted)
{
    IMG_RESULT ret = IMG_ERROR_NOT_SUPPORTED;

    SYS_LockAcquire(&(g_psCIDriver->sConnectionLock));
    {
        if (wanted < g_psCIDriver->ui32DPFInternalBuff + prev)
        {
            g_psCIDriver->ui32DPFInternalBuff += prev;
            g_psCIDriver->ui32DPFInternalBuff -= wanted;
            ret = IMG_SUCCESS;
        }
        else
        {
            CI_FATAL("prev=%d wanted=%d available=%d\n", prev, wanted,
                g_psCIDriver->ui32DPFInternalBuff);
        }
    }
    SYS_LockRelease(&(g_psCIDriver->sConnectionLock));
    IMG_ASSERT(g_psCIDriver->ui32DPFInternalBuff
        <= g_psCIDriver->sHWInfo.config_ui32DPFInternalSize);

    return ret;
}

IMG_STATIC_ASSERT(CI_N_HEAPS == 7, CI_N_HEAPS_CHANGED);

IMG_RESULT KRN_CI_DriverCreate(KRN_CI_DRIVER *pKrnDriver,
    IMG_UINT8 ui8MMUEnabled, IMG_UINT32 ui32TilingScheme,
    IMG_UINT32 ui32GammaCurveDefault, SYS_DEVICE *pDevice)
{
    IMG_RESULT ret;
    IMG_UINT32 i;

    // has already been initialised so cannot change parameters
    if (g_psCIDriver)
    {
        CI_FATAL("Driver already initialised\n");
        return IMG_ERROR_MEMORY_IN_USE;
    }
    if (!pDevice)
    {
        CI_FATAL("Driver needs a SYS_DEVICE\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

#ifdef FELIX_FAKE
    /* when running against a simulator we can choose the page size at
     * run-time, real system should define PAGE_SIZE as the correct value
     * as a global-accessible macro*/
    ret = IMGMMU_SetCPUPageSize(PAGE_SIZE);
    if (IMG_SUCCESS != ret)
    {
        return IMG_ERROR_FATAL;
    }
#endif

    IMG_MEMSET(pKrnDriver, 0, sizeof(KRN_CI_DRIVER));

    pKrnDriver->pDevice = pDevice;
    pKrnDriver->uiSemWait = 1000; // 1s timeout by default
    //pKrnDriver->pDevice->pszDeviceName = TAL_TARGET_NAME;

    CI_INFO("driver initialisation...\n");

    pKrnDriver->pDevice->irqHardHandle = &HW_CI_DriverHardHandleInterrupt;
    pKrnDriver->pDevice->irqThreadHandle = &HW_CI_DriverThreadHandleInterrupt;
    pKrnDriver->pDevice->handlerParam = pKrnDriver->pDevice;

    // owner is set in the insmod init function
    pKrnDriver->pDevice->sFileOps.open = &DEV_CI_Open;
    pKrnDriver->pDevice->sFileOps.release = &DEV_CI_Close;
    pKrnDriver->pDevice->sFileOps.unlocked_ioctl = &DEV_CI_Ioctl;
    pKrnDriver->pDevice->sFileOps.mmap = &DEV_CI_Mmap;

    pKrnDriver->pDevice->suspend = &DEV_CI_Suspend;
    pKrnDriver->pDevice->resume = &DEV_CI_Resume;

    ret = List_init(&(pKrnDriver->sWorkqueue));
    if (ret)  // unlikely
    {
        CI_FATAL("failed to create the workqueue\n");
        return ret;
    }
    ret = List_init(&(pKrnDriver->sList_connection));
    if (ret)  // unlikely
    {
        CI_FATAL("failed to create the connection list\n");
        return ret;
    }

    // Target config file is the one command line option
    {
        static TAL_sDeviceInfo sDevInfo;

#ifdef FELIX_FAKE // using TAL Normal
        // not using info from device discovery because it did not use much
        setDeviceInfo_IP(&sDevInfo, ui32TCPPort, pszTCPSimIp);
        if (IMG_FALSE == bUseTCP)
        {
            sDevInfo.sDevIfDeviceCB.sDevIfSetup.sWrapCtrlInfo.eDevifType =
                DEVIF_TYPE_DIRECT; // transif operates througth direct
    }

#ifdef FELIX_UNIT_TESTS
        sDevInfo.sDevIfDeviceCB.sDevIfSetup.sWrapCtrlInfo.eDevifType =
            DEVIF_TYPE_NULL;
#endif

#else // we did real probing
        sDevInfo.ui64DeviceBaseAddr =
            pKrnDriver->pDevice->uiRegisterCPUVirtual;
        sDevInfo.ui64MemBaseAddr = pKrnDriver->pDevice->uiMemoryCPUVirtual;
#endif

        TARGET_SetMemSpaceInfo(gasMemSpaceInfo, TAL_MEM_SPACE_ARRAY_SIZE,
            &sDevInfo);
}

    // Setup TAL and Pdump
    if (TALSETUP_Initialise() != IMG_SUCCESS)
    {
        CI_FATAL("failed to initialise the Target Abstraction Layer\n");
        TALSETUP_Deinitialise();
        // nothing in the connection list, nothing to destroy
        return IMG_ERROR_FATAL;
    }

#ifdef FELIX_FAKE
    // use pdump2 and pdump1, parameters and results
    TALPDUMP_SetFlags(TAL_PDUMP_FLAGS_PDUMP2
        | TAL_PDUMP_FLAGS_PDUMP1 | TAL_PDUMP_FLAGS_PRM | TAL_PDUMP_FLAGS_RES);
    // TAL_PDUMP_FLAGS_GZIP could be used too
#endif

    ret = SYS_SpinlockInit(&(pKrnDriver->sWorkSpinlock));
    if (ret)  // unlikely
    {
        CI_FATAL("failed to create the connection lock\n");
        return ret;
    }

    ret = SYS_LockInit(&(pKrnDriver->sConnectionLock));
    if (ret)  // unlikely
    {
        CI_FATAL("failed to create the connection lock\n");
        return ret;
    }
#ifndef INFOTM_USE_SPIN_LOCK // added by linyun.xiong @2016-03-22
    ret = SYS_LockInit(&(pKrnDriver->sActiveContextLock));
#else
    ret = SYS_SpinlockInit(&(pKrnDriver->sActiveContextLock));
#endif
    if (ret)  // unlikely
    {
        CI_FATAL("Failed to create active context lock\n", CI_N_CONTEXT);
        return ret;
    }
	
    /* not using g_psCIDriver->sHWInfo.config_ui8NContexts because
     *creating the objects*/
    for (i = 0; i < CI_N_CONTEXT && ret == IMG_SUCCESS; i++)
    {
        ret = SYS_SemInit(&(pKrnDriver->aListQueue[i]), 0);
    }
    if (IMG_SUCCESS != ret)
    {
        CI_FATAL("Failed to create context semaphore %d/%d\n", i,
            CI_N_CONTEXT);

        while (i >= 1)
        {
            i--;
            SYS_SemDestroy(&(pKrnDriver->aListQueue[i]));
        }
#ifndef INFOTM_USE_SPIN_LOCK // added by linyun.xiong @2016-03-22
        SYS_LockDestroy(&(pKrnDriver->sActiveContextLock));
#else
		SYS_SpinlockDestroy(&(pKrnDriver->sActiveContextLock));
#endif
        SYS_SpinlockDestroy(&(pKrnDriver->sWorkSpinlock));

        return ret;
    }

    /*
     * in order to allow the setup of the default register when using the NULL
     * interface
     */
#ifdef FELIX_UNIT_TESTS
    if (gpfnInitTal != NULL) gpfnInitTal();
#endif // FELIX_UNIT_TESTS

    // get memspace handles used to access the device
    pKrnDriver->sTalHandles.hRegFelixCore =
        TAL_GetMemSpaceHandle("REG_FELIX_CORE");

    /* not using g_psCIDriver->sHWInfo.config_ui8NContexts because
     * creating objects*/
    /* if the context do not exists the TAL handle will be an invalid address
     *- but it should not be used */
    for ( i = 0 ; i < CI_N_CONTEXT ; i++ )
    {
        char name[32];
        sprintf(name, "REG_FELIX_CONTEXT_%d", i);
        pKrnDriver->sTalHandles.hRegFelixContext[i] =
            TAL_GetMemSpaceHandle(name);
    }

#if FELIX_VERSION_MAJ == 1 // compiled into the register info

    pKrnDriver->sTalHandles.hRegGaskets[0] =
        TAL_GetMemSpaceHandle("REG_FELIX_GASKET");
    for (i = 1; i < CI_N_IMAGERS; i++)
    {
        /* to fool the Pdump and tal handle verification when running
         * fake driver */
        pKrnDriver->sTalHandles.hRegGaskets[i] =
            pKrnDriver->sTalHandles.hRegGaskets[0];
    }

    for (i = 0; i < CI_N_IIF_DATAGEN; i++)
    {
        pKrnDriver->sTalHandles.hRegIIFDataGen[i] = NULL;
    }
#else
    for (i = 0; i < CI_N_IMAGERS; i++)
    {
        char name[32];
        sprintf(name, "REG_FELIX_GASKET_%d", i);
        pKrnDriver->sTalHandles.hRegGaskets[i] = TAL_GetMemSpaceHandle(name);
    }

    for (i = 0; i < CI_N_IIF_DATAGEN; i++)
    {
        char name[32];
        sprintf(name, "REG_FELIX_DG_IIF_%d", i);
        pKrnDriver->sTalHandles.hRegIIFDataGen[i] =
            TAL_GetMemSpaceHandle(name);
    }
#endif

    pKrnDriver->sTalHandles.hRegGammaLut =
        TAL_GetMemSpaceHandle("REG_FELIX_GMA_LUT");
    pKrnDriver->sTalHandles.hMemHandle = TAL_GetMemSpaceHandle("SYSMEM");

    //pKrnDriver->sTalHandles.hRegFelixI2C =
    //    TAL_GetMemSpaceHandle("REG_FELIX_SCB");
    pKrnDriver->sMMU.hRegFelixMMU = TAL_GetMemSpaceHandle("REG_FELIX_MMU");

    // pdump generation is only when running against the csim
#if defined(FELIX_FAKE) && !defined(FELIX_UNIT_TESTS)
    if (bEnablePdump)
    {
        ret = KRN_CI_DriverStartPdump(pKrnDriver, 1);
        if (IMG_SUCCESS != ret)
        {
            CI_FATAL("failed to start pdump\n");
            return IMG_ERROR_FATAL;
        }
    }
#endif

    // reset the HW when 1st connection
#ifdef FELIX_FAKE
    CI_PDUMP_COMMENT(pKrnDriver->sTalHandles.hRegFelixCore, "HW power ON");
#else
    CI_DEBUG("HW power ON");
#endif
    // HW is started to read information - this will not print in pdump...
    SYS_DevPowerControl(pDevice, IMG_TRUE);

    /*
     * load HW information (e.g. number of imagers)
     */
    ret = HW_CI_DriverLoadInfo(pKrnDriver, &(pKrnDriver->sHWInfo));
    if (IMG_SUCCESS != ret)
    {
        CI_FATAL("could not load HW information - wrong device discovered!\n");
        KRN_CI_DriverDestroy(pKrnDriver);
        return IMG_ERROR_NOT_SUPPORTED;
    }

    // line not used and disable it in HW to be sure
    pKrnDriver->ui8EncOutContext = CI_N_CONTEXT;
    pKrnDriver->ui32DPFInternalBuff =
        pKrnDriver->sHWInfo.config_ui32DPFInternalSize;
    pKrnDriver->sHWInfo.uiTiledScheme = ui32TilingScheme;
    pKrnDriver->sHWInfo.uiMMUEnabled = ui8MMUEnabled == 0 ?
        0 : ui8MMUEnabled == 1 ? 32 : 40;

    if (pKrnDriver->sHWInfo.config_ui8NContexts > CI_N_CONTEXT
        || 0 == pKrnDriver->sHWInfo.config_ui8NContexts)
    {
        CI_FATAL("driver compiled with %d context support, HW has %d\n",
            CI_N_CONTEXT, pKrnDriver->sHWInfo.config_ui8NContexts);
        /* put it to the max to avoid seg-fault when destroying "dynamic"
         * objects*/
        pKrnDriver->sHWInfo.config_ui8NContexts = CI_N_CONTEXT;
        // almost started... better call destroy
        KRN_CI_DriverDestroy(pKrnDriver);
        return IMG_ERROR_NOT_SUPPORTED;
    }

    if (pKrnDriver->sHWInfo.config_ui8NImagers > CI_N_IMAGERS
        || 0 == pKrnDriver->sHWInfo.config_ui8NImagers)
    {
        CI_FATAL("driver compiled with %d imager support, HW has %d\n",
            CI_N_IMAGERS, pKrnDriver->sHWInfo.config_ui8NImagers);
        /* put it to the max to avoid seg-fault when destroying "dynamic"
         * objects*/
        pKrnDriver->sHWInfo.config_ui8NImagers = CI_N_IMAGERS;
        // almost started... better destroy
        KRN_CI_DriverDestroy(pKrnDriver);
        return IMG_ERROR_NOT_SUPPORTED;
    }

    if (pKrnDriver->sHWInfo.scaler_ui8EncHLumaTaps != ESC_H_LUMA_TAPS
        || pKrnDriver->sHWInfo.scaler_ui8EncVLumaTaps != ESC_V_LUMA_TAPS
        /*|| pKrnDriver->sHWInfo.scaler_ui8EncHChromaTaps != ESC_H_CHROMA_TAPS*/
        || pKrnDriver->sHWInfo.scaler_ui8EncVChromaTaps != ESC_V_CHROMA_TAPS
        || pKrnDriver->sHWInfo.scaler_ui8DispHLumaTaps != DSC_H_LUMA_TAPS
        || pKrnDriver->sHWInfo.scaler_ui8DispVLumaTaps != DSC_V_LUMA_TAPS
        /* ||pKrnDriver->sHWInfo.scaler_ui8DispHChromaTaps != DSC_H_CHROMA_TAPS
        || pKrnDriver->sHWInfo.scaler_ui8DispVChromaTaps != DSC_V_CHROMA_TAPS */
        )
    {
        CI_FATAL("driver compiled with different taps size than available "\
            "(reg vs compiled): ESC H luma %u-%u, V luma %u-%u, V chroma "\
            "%u-%u ; DSC H luma %u-%u, V luma %u-%u\n",
            pKrnDriver->sHWInfo.scaler_ui8EncHLumaTaps, ESC_H_LUMA_TAPS,
            pKrnDriver->sHWInfo.scaler_ui8EncVLumaTaps, ESC_V_LUMA_TAPS,
            pKrnDriver->sHWInfo.scaler_ui8EncVChromaTaps, ESC_V_CHROMA_TAPS,
            pKrnDriver->sHWInfo.scaler_ui8DispHLumaTaps, DSC_H_LUMA_TAPS,
            pKrnDriver->sHWInfo.scaler_ui8DispVLumaTaps, DSC_V_LUMA_TAPS
            );
        // almost started... better destroy
        KRN_CI_DriverDestroy(pKrnDriver);
        return IMG_ERROR_NOT_SUPPORTED;
    }

    // if FELIX_VERSION_MAJ == 2 we support 2.1, 2.3 and 2.4
    if (pKrnDriver->sHWInfo.rev_ui8Designer != FELIX_DESIGNER ||
        pKrnDriver->sHWInfo.rev_ui8Major != FELIX_VERSION_MAJ ||
        (pKrnDriver->sHWInfo.rev_ui8Minor != FELIX_VERSION_MIN &&
        !(FELIX_VERSION_MAJ == 2 && pKrnDriver->sHWInfo.rev_ui8Major == 2
        && (pKrnDriver->sHWInfo.rev_ui8Minor == 3
        || pKrnDriver->sHWInfo.rev_ui8Minor == 4)))
        )
    {
        CI_FATAL("unsupported HW found! Driver supports design %d %d.%d - "\
            "found design %d %d.%d\n",
            FELIX_DESIGNER, FELIX_VERSION_MAJ, FELIX_VERSION_MIN,
            pKrnDriver->sHWInfo.rev_ui8Designer,
            pKrnDriver->sHWInfo.rev_ui8Major,
            pKrnDriver->sHWInfo.rev_ui8Minor);
        // almost started... better destroy
        KRN_CI_DriverDestroy(pKrnDriver);
        return IMG_ERROR_NOT_SUPPORTED;
    }

    if (IMGMMU_GetPageSize() != pKrnDriver->sHWInfo.ui32MMUPageSize)
    {
        CI_FATAL("unsupported HW found! Driver supports device MMU page "\
            "size %d but HW reports %d\n",
            IMGMMU_GetPageSize(), pKrnDriver->sHWInfo.ui32MMUPageSize);
        // almost started... better destroy
        KRN_CI_DriverDestroy(pKrnDriver);
        return IMG_ERROR_NOT_SUPPORTED;
    }

    // default linestore computation
    IMG_CI_DriverDefaultLinestore(pKrnDriver);

    // default Gamma LUT setup
    ret = KRN_CI_DriverDefaultsGammaLUT(&(pKrnDriver->sGammaLUT),
        ui32GammaCurveDefault);
    if (IMG_SUCCESS != ret)
    {
        CI_FATAL("Default Gamma curve %u setup failed!\n",
            ui32GammaCurveDefault);
        // almost started... better destroy
        KRN_CI_DriverDestroy(pKrnDriver);
        return IMG_ERROR_FATAL;
    }

    /*
     * load dynamic HW information
     */

    // read again in DEV_CI_Resume but here to make sure HW has correct default
    HW_CI_ReadCurrentDEPoint(pKrnDriver,
        &(pKrnDriver->eCurrentDataExtraction));

    if (pKrnDriver->eCurrentDataExtraction >= CI_INOUT_NONE)
    {
        CI_FATAL("Unkown state for data extraction field!\n");
        // almost started... better destroy
        KRN_CI_DriverDestroy(pKrnDriver);
        return IMG_ERROR_FATAL;
    }

    // device was found and initialised
    /* done here because MMU configuration will use it to allocate the
     * directory page*/
    g_psCIDriver = pKrnDriver;

    /*
     * MMU configuration
     */
    pKrnDriver->sMMU.uiTiledScheme = ui32TilingScheme;
    IMG_MEMSET(pKrnDriver->sMMU.aHeapSize, 0, CI_N_HEAPS*sizeof(IMG_UINT32));
    IMG_MEMSET(pKrnDriver->sMMU.aHeapStart, 0, CI_N_HEAPS*sizeof(IMG_UINT32));

    if (0 != ui8MMUEnabled)
    {
        IMG_UINT64 max = 0x100000000ULL;
        IMG_INT32 i = 0;
        IMG_UINT32 tiledHeapSize = 0;
        IMG_UINT64 total = 0;
        IMG_UINT32 ui32MMUPageSize = IMGMMU_GetPageSize();

        if (256 != pKrnDriver->sMMU.uiTiledScheme
            && 512 != pKrnDriver->sMMU.uiTiledScheme)
        {
            CI_FATAL("Unsuported tiling scheme of %dB (supports 256 = "\
                "256Bx16 and 512 = 512Bx8)\n",
                pKrnDriver->sMMU.uiTiledScheme);
            // almost started... better destroy
            KRN_CI_DriverDestroy(pKrnDriver);
            return IMG_ERROR_FATAL;
        }

        // starts at page size to stkip virtual address 0
        total = ui32MMUPageSize;

        // virtual heap 0: DATA (first 256 MB)
        pKrnDriver->sMMU.uiAllocAtom = ui32MMUPageSize;
        pKrnDriver->sMMU.aHeapStart[CI_DATA_HEAP] = (IMG_UINT32)total;
        // 256MB of virtual memory (1/16 of virtual memory)
        pKrnDriver->sMMU.aHeapSize[CI_DATA_HEAP] = (1 << (8 + 20))
            - ui32MMUPageSize;
        total += (IMG_UINT64)pKrnDriver->sMMU.aHeapSize[CI_DATA_HEAP];
        /* allocation in the data heap are: LSH grid, DPF read map and per
         * buffer: load structure, save structure, link list and DPF
         * write map*/
#ifndef NDEBUG
        {
            IMG_UINT64 needed = 0;
            IMG_UINT8 nBuff = IMG_MAX_INT(
                pKrnDriver->sHWInfo.context_aPendingQueue[0], CI_MAX_N_BUFF);
            // verify than the 1/16 of memory is enough

            /* LSH - normally is subsampled but let's assume it is not
             * - rounded up to page size*/
            needed += (pKrnDriver->sHWInfo.context_aMaxWidthSingle[0]
                * pKrnDriver->sHWInfo.context_aMaxHeight[0] + ui32MMUPageSize + 1)
                / ui32MMUPageSize *ui32MMUPageSize;
            /* DPF read should be smaller normally (DPF_MAP_MAX_PER_LINE is
             * max expected output) - rounded up to page size */
            needed += (DPF_MAP_MAX_PER_LINE
                *pKrnDriver->sHWInfo.context_aMaxHeight[0] + ui32MMUPageSize + 1)
                / ui32MMUPageSize *ui32MMUPageSize;
            // per buffer information
            needed += nBuff*((HW_CI_LoadStructureSize() + ui32MMUPageSize - 1)
                / ui32MMUPageSize *ui32MMUPageSize);
            needed += nBuff*((HW_CI_SaveStructureSize() + ui32MMUPageSize - 1)
                / ui32MMUPageSize *ui32MMUPageSize);
            needed += nBuff*((HW_CI_LinkedListSize() + ui32MMUPageSize - 1)
                / ui32MMUPageSize *ui32MMUPageSize);
            needed += nBuff*((DPF_MAP_MAX_PER_LINE
                *pKrnDriver->sHWInfo.context_aMaxHeight[0] + ui32MMUPageSize + 1)
                / ui32MMUPageSize *ui32MMUPageSize);

            if (needed > pKrnDriver->sMMU.aHeapSize[CI_DATA_HEAP])
            {
                CI_FATAL("Not enought memory reserved in data heap!\n");
                // almost started... better destroy
                KRN_CI_DriverDestroy(pKrnDriver);
                return IMG_ERROR_FATAL;
            }
        }
#endif

        // virtual heap 1: internal DG (last 256 MB)
        pKrnDriver->sMMU.uiAllocAtom = ui32MMUPageSize;
        // 256MB of virtual memory (1/16 of virtual memory)
        pKrnDriver->sMMU.aHeapSize[CI_INTDG_HEAP] = (1 << (8 + 20))
            - ui32MMUPageSize;
        pKrnDriver->sMMU.aHeapStart[CI_INTDG_HEAP] = (IMG_UINT32)(max
            - ui32MMUPageSize - pKrnDriver->sMMU.aHeapSize[CI_INTDG_HEAP]);
        total += (IMG_UINT64)pKrnDriver->sMMU.aHeapSize[CI_INTDG_HEAP];

        // virtual heap 2: images
        pKrnDriver->sMMU.aHeapStart[CI_IMAGE_HEAP] =
            pKrnDriver->sMMU.aHeapStart[CI_DATA_HEAP]
            + pKrnDriver->sMMU.aHeapSize[CI_DATA_HEAP];
        /* compute the remaining virtual memory - remove last page because
         * it should not be used*/
        pKrnDriver->sMMU.aHeapSize[CI_IMAGE_HEAP] = (IMG_UINT32)(max
            - total - ui32MMUPageSize);
        // divide the remaining of memory in half and round up to page size
        pKrnDriver->sMMU.aHeapSize[CI_IMAGE_HEAP] =
            ((pKrnDriver->sMMU.aHeapSize[CI_IMAGE_HEAP] / 2)
            / ui32MMUPageSize)*ui32MMUPageSize;;
        total += (IMG_UINT64)pKrnDriver->sMMU.aHeapSize[CI_IMAGE_HEAP];

        // virtual heap 3 to 7: tiled images
        /*tiledHeapSize = (pKrnDriver->sMMU.aHeapSize[CI_IMAGE_HEAP])
            /pKrnDriver->sHWInfo.config_ui8NContexts;*/
        tiledHeapSize = (IMG_UINT32)(max - total - ui32MMUPageSize);
        tiledHeapSize = ((tiledHeapSize
            / pKrnDriver->sHWInfo.config_ui8NContexts - 1)
            / ui32MMUPageSize)*ui32MMUPageSize;

        pKrnDriver->sMMU.aHeapStart[CI_TILED_IMAGE_HEAP0] =
            (pKrnDriver->sMMU.aHeapStart[CI_IMAGE_HEAP]
            + pKrnDriver->sMMU.aHeapSize[CI_IMAGE_HEAP]);
        pKrnDriver->sMMU.aHeapSize[CI_TILED_IMAGE_HEAP0] = tiledHeapSize;
        total += (IMG_UINT64)pKrnDriver->sMMU.aHeapSize[CI_TILED_IMAGE_HEAP0];

        for (i = 1; i < pKrnDriver->sHWInfo.config_ui8NContexts; i++)
        {
            pKrnDriver->sMMU.aHeapStart[CI_TILED_IMAGE_HEAP0 + i] =
                pKrnDriver->sMMU.aHeapStart[CI_TILED_IMAGE_HEAP0 + (i - 1)]
                + tiledHeapSize;
            pKrnDriver->sMMU.aHeapSize[CI_TILED_IMAGE_HEAP0 + i] =
                tiledHeapSize;
            total += (IMG_UINT64)
                pKrnDriver->sMMU.aHeapSize[CI_TILED_IMAGE_HEAP0 + i];
        }

        total += ui32MMUPageSize; // add last page

        if (total > max)
        {
            CI_FATAL("Reserved more virtual memory than possible!\n");
            // almost started... better destroy
            KRN_CI_DriverDestroy(pKrnDriver);
            return IMG_ERROR_FATAL;
        }

        for (i = 0; i < pKrnDriver->sHWInfo.config_ui8NContexts; i++)
        {
            pKrnDriver->sMMU.apDirectory[i] = KRN_CI_MMUDirCreate(pKrnDriver,
                &ret);
            if (!pKrnDriver->sMMU.apDirectory[i])
            {
                i--;
                while (i >= 0)
                {
                    KRN_CI_MMUDirDestroy(pKrnDriver->sMMU.apDirectory[i]);
                    i--;
                }
                // almost started... better destroy
                KRN_CI_DriverDestroy(pKrnDriver);
                return ret;
            }
            // configure 1 requestor per MMU directory
            pKrnDriver->sMMU.aRequestors[i] = i;
        }

        pKrnDriver->sMMU.bEnableExtAddress = ui8MMUEnabled > 1;

        // MMU is configured when 1st connection is made
        CI_INFO("Main MMU pre-configured (ext address range %d)\n",
            pKrnDriver->sMMU.bEnableExtAddress);
    } // mmu config

    /*
     * Interrupt management is done when starting the capture
     */

#ifdef FELIX_FAKE
    CI_PDUMP_COMMENT(pKrnDriver->sTalHandles.hRegFelixCore, "HW power OFF");
#else
    CI_DEBUG("HW power OFF");
#endif
    // HW is stopped until 1st connection
    SYS_DevPowerControl(pDevice, IMG_FALSE);

#if defined(CI_DEBUG_FCT) && defined(FELIX_FAKE)
    KRN_CI_DebugEnableRegisterDump(IMG_FALSE);

    ret = KRN_CI_DebugRegisterOverrideInit();
    if (IMG_SUCCESS != ret)
    {
        CI_FATAL("Failed to initialise the register overwrite\n");
        // almost is started... better call destroy
        KRN_CI_DriverDestroy(pKrnDriver);
        return ret;
    }
#endif

    return IMG_SUCCESS;
}

IMG_RESULT KRN_CI_DriverDestroy(KRN_CI_DRIVER *pDriver)
{
    IMG_UINT32 i;
    sCell_T *pCurrent = NULL;

    CI_INFO("shutting down HW\n");

    // if it is not it means it failed at creation time!
    if (g_psCIDriver == pDriver)
    {
        //LOCK driver
        g_psCIDriver = NULL; // so no one else can do things
        //UNLOCK driver
    }

    /* kernel module should not have connections lefts (rmmod is not
    * possible if applications are still using the module)*/
    IMG_ASSERT(pDriver->sList_connection.ui32Elements == 0);
    // no need to lock as nothing else can add to the driver
    //SYS_LockAcquire(&(pDriver->sConnectionLock)); 
    {
        /* the mmu is not stopped but there should be no connections
         * left when doing rmmod*/
        pCurrent = List_popFront(&(pDriver->sList_connection));
        while (pCurrent)
        {
            KRN_CI_ConnectionDestroy((KRN_CI_CONNECTION*)pCurrent->object);
            IMG_FREE(pCurrent);
            pCurrent = List_popFront(&(pDriver->sList_connection));
        }
    }
    //SYS_LockRelease(&(pDriver->sConnectionLock));

    IMG_ASSERT(pDriver->sWorkqueue.ui32Elements == 0);

    SYS_SpinlockDestroy(&(pDriver->sWorkSpinlock));
    SYS_LockDestroy(&(pDriver->sConnectionLock));
#ifndef INFOTM_USE_SPIN_LOCK // added by linyun.xiong @2016-03-22
    SYS_LockDestroy(&(pDriver->sActiveContextLock));
#else
    SYS_SpinlockDestroy(&(pDriver->sActiveContextLock));
#endif

    /* not using g_psCIDriver->sHWInfo.config_ui8NContexts because created
     * using CI_N_CONTEXT*/
    for (i = 0; i < CI_N_CONTEXT; i++)
    {
        SYS_SemDestroy(&(pDriver->aListQueue[i]));
    }

    for (i = 0; i < pDriver->sHWInfo.config_ui8NContexts; i++)
    {
        KRN_CI_MMUDirDestroy(pDriver->sMMU.apDirectory[i]);
        pDriver->sMMU.apDirectory[i] = NULL;
    }

    // Shutdown TAL
#ifdef FELIX_FAKE
    TALPDUMP_CaptureStop();
#endif
    TALSETUP_Deinitialise();

#if defined(CI_DEBUG_FCT) && defined(FELIX_FAKE)
    KRN_CI_DebugRegisterOverrideFinalise();
#endif

    pDriver->pDevice = NULL;

#ifdef INFOTM_ISP
    CI_INFO("driver initialisation success !!!\n");
#endif //INFOTM_ISP	
    return IMG_SUCCESS;
}

IMG_RESULT KRN_CI_DriverAcquireContext(KRN_CI_PIPELINE *pCapture)
{
    IMG_UINT8 ui8Context = 0;
    IMG_RESULT ret = IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;

    if (!pCapture)
    {
        CI_FATAL("pCapture is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    if (!g_psCIDriver)
    {
        CI_FATAL("Driver not initialised\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    // should have been checked already in IMG_CI_ caller
    ui8Context = pCapture->userPipeline.ui8Context;

    SYS_LockAcquire(&(g_psCIDriver->sConnectionLock));
#ifndef INFOTM_USE_SPIN_LOCK // added by linyun.xiong @2016-03-22
    SYS_LockAcquire(&(g_psCIDriver->sActiveContextLock));
#else
	SYS_SpinlockAcquire(&(g_psCIDriver->sActiveContextLock));
#endif
    {
        ret = IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
        // verify that it is available
        if (g_psCIDriver->aActiveCapture[ui8Context] == NULL)
        {
            IMG_UINT32 ui32Width =
                (pCapture->userPipeline.sImagerInterface.ui16ImagerSize[0] + 1)
                * 2;
            IMG_UINT32 ui32MaxSize =
                g_psCIDriver->sHWInfo.context_aMaxWidthSingle[ui8Context];

            if (g_psCIDriver->ui32NActiveCapture > 0)
            {
                IMG_UINT8 i;
                ui32MaxSize =
                    g_psCIDriver->sHWInfo.context_aMaxWidthMult[ui8Context];
                for (i = 0; i < g_psCIDriver->sHWInfo.config_ui8NContexts;
                    i++)
                {
                    if (g_psCIDriver->aActiveCapture[i])
                    {
                        if (g_psCIDriver->sLinestoreStatus.aSize[i]
                >g_psCIDriver->sHWInfo.context_aMaxWidthMult[i])
                        {
                            ui32MaxSize = 0;
                            /* cannot run at all because one context is
                             * using more than its multiple context size */
                        }
                    }
                }
            }

            if (g_psCIDriver->sLinestoreStatus.aSize[ui8Context] < ui32Width
                || ui32Width>ui32MaxSize)
            {
                /* does not fit in the reserved linestore
                 * or the required size cannot be met (either using too much
                 * or one active context is running above its multiple
                 * limit) */
                ret = IMG_ERROR_MINIMUM_LIMIT_NOT_MET;
            }
            else
            {
                ret = IMG_SUCCESS;
                if (pCapture->userPipeline.bUseEncOutLine)
                {
                    // assume not available
                    ret = IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
                    if (CI_N_CONTEXT == g_psCIDriver->ui8EncOutContext)
                    {
                        // it is currently not used so we take it
                        g_psCIDriver->ui8EncOutContext = ui8Context;

                        HW_CI_DriverWriteEncOut(ui8Context,
                            pCapture->userPipeline.ui8EncOutPulse);
                        ret = IMG_SUCCESS;
                    }
                }

                if (IMG_SUCCESS == ret)
                {
                    IMG_INT8 i8IntDGCountSource = -1; // means no
                    int g = 0;
                    // set capture to pCapture for the connection
                    g_psCIDriver->aActiveCapture[ui8Context] = pCapture;
                    IMG_ASSERT(g_psCIDriver->ui32NActiveCapture >= 0);
                    g_psCIDriver->ui32NActiveCapture++;

                    // only 1 DG in HW but code is almost ready for several
                    /*for ( g = 0 ; g
                        < g_psCIDriver->sHWInfo.config_ui8NIIFDataGenerator ;
                        g++ )*/
                    {
                        if (g_psCIDriver->pActiveDatagen)
                        {
                            if (g_psCIDriver->pActiveDatagen->userDG.ui8Gasket
                                == pCapture->userPipeline.sImagerInterface.\
                                ui8Imager)
                            {
                                i8IntDGCountSource = g;
                            }
                        }
                    }

                    if (i8IntDGCountSource < 0)
                    {
                        IMG_UINT32 imager =
                            pCapture->userPipeline.sImagerInterface.ui8Imager;
                        g_psCIDriver->aLastGasketCount[ui8Context] =
                            HW_CI_GasketFrameCount(imager);
                        // reset counter of missed interrupts
                    }
                    else
                    {
                        g_psCIDriver->aLastGasketCount[ui8Context] =
                            HW_CI_DatagenFrameCount(i8IntDGCountSource);
                    }

                    g_psCIDriver->sLinestoreStatus.aActive[ui8Context] =
                        IMG_TRUE;
                    g_psCIDriver->sLinestoreStatus.aSize[ui8Context] =
                        ui32Width;
                    pCapture->bStarted = IMG_TRUE;
                }
            }
        }
        /*else
          {
          ret = IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
          }*/
    }
#ifndef INFOTM_USE_SPIN_LOCK // added by linyun.xiong @2016-03-22
    SYS_LockRelease(&(g_psCIDriver->sActiveContextLock));
#else
	SYS_SpinlockRelease(&(g_psCIDriver->sActiveContextLock));
#endif
    SYS_LockRelease(&(g_psCIDriver->sConnectionLock));

    if (ret != IMG_SUCCESS)
    {
        CI_FATAL("HW context %u is not available (%d) - Pipeline %d "\
            "not activated\n", ui8Context, ret, pCapture->ui32Identifier);
    }
    else
    {

        CI_INFO("Pipeline %d acquires ctx %d\n",
            pCapture->ui32Identifier, ui8Context);

        /* BRN48137: if cropping occurs at the bottom of the IFF the clocks
         * may be turned off and the end of frame from the imager could
         * be missed
         * work-around is to force the clocks on for the corresponding
         * context clocks and turn them back to auto when finished */
        {
            CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hRegFelixCore,
                "BRN48137 work around force clocks");
            HW_CI_DriverPowerManagement_BRN48137(ui8Context, PWR_FORCE_ON);
        }
    }

    return ret;
}

IMG_RESULT KRN_CI_DriverReleaseContext(KRN_CI_PIPELINE *pCapture)
{
    IMG_UINT8 ui8Context = 0;
    IMG_RESULT ret = IMG_SUCCESS;

    if (!pCapture)
    {
        CI_FATAL("pCapture is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    if (!g_psCIDriver)
    {
        CI_FATAL("Driver not initialised\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    // should have been checked already in IMG_CI_ caller
    ui8Context = pCapture->userPipeline.ui8Context;

    SYS_LockAcquire(&(g_psCIDriver->sConnectionLock));
#ifndef INFOTM_USE_SPIN_LOCK // added by linyun.xiong @2016-03-22
    SYS_LockAcquire(&(g_psCIDriver->sActiveContextLock)); // lock driver
#else
	SYS_SpinlockAcquire(&(g_psCIDriver->sActiveContextLock)); // lock driver
#endif
    {
        // set capture to NULL for the context
        g_psCIDriver->aActiveCapture[ui8Context] = NULL;
        g_psCIDriver->ui32NActiveCapture--;
        IMG_ASSERT(g_psCIDriver->ui32NActiveCapture >= 0);

        g_psCIDriver->sLinestoreStatus.aActive[ui8Context] = IMG_FALSE;
        g_psCIDriver->sLinestoreStatus.aSize[ui8Context] =
            IMG_CI_DriveComputeLinstoreSize(ui8Context);
        pCapture->bStarted = IMG_FALSE;

        if (g_psCIDriver->ui8EncOutContext == ui8Context)
        {
            // line becomes available
            g_psCIDriver->ui8EncOutContext = CI_N_CONTEXT;
            HW_CI_DriverWriteEncOut(CI_N_CONTEXT, 0); // disables the line
        }
    }
#ifndef INFOTM_USE_SPIN_LOCK // added by linyun.xiong @2016-03-22
    SYS_LockRelease(&(g_psCIDriver->sActiveContextLock)); // unlock driver
#else
	SYS_SpinlockRelease(&(g_psCIDriver->sActiveContextLock)); // unlock driver
#endif
    SYS_LockRelease(&(g_psCIDriver->sConnectionLock));

    if (ret != IMG_SUCCESS)
    {
        CI_FATAL("Pipeline %d is not active\n", pCapture->ui32Identifier);
    }
    else
    {
        CI_INFO("Pipeline %d releases ctx %d\n",
            pCapture->ui32Identifier, ui8Context);

        // BRN48137: set KRN_CI_DriverAcquireContext
        {
            CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hRegFelixCore,
                "BRN48137 work around force clocks reset");
            HW_CI_DriverPowerManagement_BRN48137(ui8Context, PWR_AUTO);
        }
    }
    return ret;
}

IMG_BOOL8 KRN_CI_DriverCheckDeshading(KRN_CI_PIPELINE *pPipeline)
{
    IMG_UINT32 ui32GridSize = 0; // in pixels
    IMG_UINT8 ctx = 0;
    IMG_UINT32 maxSize = 0;

    IMG_ASSERT(g_psCIDriver);
    IMG_ASSERT(pPipeline);

    //ui32TileSize = (1 << pPipeline->userPipeline.sDeshading.ui8TileSizeLog2);
    ui32GridSize = pPipeline->ui16LSHPixelSize;
    /*        pPipeline->userPipeline.sDeshading.ui16Width*
            ui32TileSize*CI_CFA_WIDTH;*/
    ctx = pPipeline->userPipeline.ui8Context;
    // if not start we just verify the single context size
    maxSize = g_psCIDriver->sHWInfo.context_aMaxWidthSingle[ctx];

    if (0 == ui32GridSize)
    {
        // no grid no need to check
        return IMG_TRUE;
    }

    if (pPipeline->bStarted)
    {
        int c;
#ifndef INFOTM_USE_SPIN_LOCK // added by linyun.xiong @2016-03-22
        SYS_LockAcquire(&(g_psCIDriver->sActiveContextLock));
#else
		SYS_SpinlockAcquire(&(g_psCIDriver->sActiveContextLock));
#endif
        if (g_psCIDriver->ui32NActiveCapture > 1)
        {
            maxSize = g_psCIDriver->sHWInfo.context_aMaxWidthMult[ctx];

            /* but also verifies that other context do not use more than the
            * maximum size for multi-context */
            for (c = 0; c < CI_N_CONTEXT; c++)
            {
                if (g_psCIDriver->aActiveCapture[c] &&
                    g_psCIDriver->aActiveCapture[c] != pPipeline)
                {
                    if (g_psCIDriver->aActiveCapture[c]->ui16LSHPixelSize >
                        g_psCIDriver->sHWInfo.context_aMaxWidthMult[c])
                    {
                        maxSize = 0;
                    }
                }
            }
        }
#ifndef INFOTM_USE_SPIN_LOCK // added by linyun.xiong @2016-03-22
        SYS_LockRelease(&(g_psCIDriver->sActiveContextLock));
#else
		SYS_SpinlockRelease(&(g_psCIDriver->sActiveContextLock));
#endif
    }

    if (0 == maxSize)
    {
        CI_FATAL("another context runs with its grid size bigger than its "\
            "multiple context size, context %d cannot use an LSH grid\n",
            ctx);
        return IMG_FALSE;
    }

    if (ui32GridSize > maxSize)
    {
        CI_FATAL("grid covers %d pixels which does not fit in maximum "\
            "size %d for context %d\n",
            ui32GridSize, maxSize, ctx);
        return IMG_FALSE;
    }
    return IMG_TRUE;
}

// !!! do not call if a capture's lock is locked!
IMG_RESULT KRN_CI_DriverEnableDataExtraction(enum CI_INOUT_POINTS eWanted)
{
    IMG_INT32 i;
    IMG_RESULT ret = IMG_ERROR_NOT_SUPPORTED; // if cannot change DE
    IMG_BOOL8 bAvailable = IMG_TRUE;

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hRegFelixCore, " - start");
#ifndef INFOTM_USE_SPIN_LOCK // added by linyun.xiong @2016-03-22
    SYS_LockAcquire(&(g_psCIDriver->sActiveContextLock));
#else
	SYS_SpinlockAcquire(&(g_psCIDriver->sActiveContextLock));
#endif
    if (g_psCIDriver->eCurrentDataExtraction == eWanted)
    {
        enum CI_INOUT_POINTS eRead;

        HW_CI_ReadCurrentDEPoint(g_psCIDriver, &eRead);

        if (eWanted == eRead)
        {
            // no need to check, it is currently OK
            ret = IMG_SUCCESS;
        }
        else
        {
            CI_WARNING("HW and SW have different DE points!\n");
        }

        CI_DEBUG("current SW DE point is %d (HW says %d)\n",
            (int)eWanted, (int)eRead);
    }

    for (i = 0; i < g_psCIDriver->sHWInfo.config_ui8NContexts
        && IMG_SUCCESS != ret; i++)
    {
        /* if the context is running with DE enabled and its DE point
         * is different */
        if (g_psCIDriver->aActiveCapture[i] != NULL
            && CI_INOUT_NONE !=
            g_psCIDriver->aActiveCapture[i]->userPipeline.eDataExtraction
            && g_psCIDriver->aActiveCapture[i]->userPipeline.eDataExtraction
            != eWanted
            )
        {
            bAvailable = IMG_FALSE;
            break; // no need to go further, one is not available
        }
    }

    if (bAvailable && IMG_SUCCESS != ret)
    {
        HW_CI_UpdateCurrentDEPoint(eWanted);

        CI_DEBUG("setting DE point to %d\n", (int)eWanted);
        g_psCIDriver->eCurrentDataExtraction = eWanted;
        ret = IMG_SUCCESS;
    }
#ifndef INFOTM_USE_SPIN_LOCK // added by linyun.xiong @2016-03-22
    SYS_LockRelease(&(g_psCIDriver->sActiveContextLock));
#else
	SYS_SpinlockRelease(&(g_psCIDriver->sActiveContextLock));
#endif
    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hRegFelixCore, " - done");
    return ret;
}

// !!! do not call if a capture's lock is locked!
IMG_RESULT KRN_CI_DriverChangeGammaLUT(const CI_MODULE_GMA_LUT *pNewGMA)
{
    IMG_INT32 i;
    IMG_RESULT ret = IMG_ERROR_NOT_SUPPORTED; // if cannot change GMA LUT
    IMG_BOOL8 bAvailable = IMG_TRUE;

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hRegGammaLut, " - start");

    SYS_LockAcquire(&(g_psCIDriver->sConnectionLock));
    {

        for (i = 0; i < g_psCIDriver->sHWInfo.config_ui8NContexts
            && IMG_SUCCESS != ret; i++)
        {
            // if the context is running we cannot change the GMA
            if (g_psCIDriver->sLinestoreStatus.aActive[i])
            {
                bAvailable = IMG_FALSE;
                break; // no need to go further, one is not available
            }
        }

#ifdef INFOTM_ISP
        bAvailable = IMG_TRUE;
        // add bAvailable Forced to true.the reason is:
        //first,change gamma lut only used is v2505 & v2500 prepare function one time,
        //      no used in other,so forced to true can't affect the code logic
        //second, The following code implements the write hardware register.The hardware
        //        registers are able to dynamically change in the experience of my debugging FPGA
        //third,  in my test, wo can change the gamma dynamically, no need to stop isp pipeline.
#endif

        if (bAvailable)
        {
            HW_CI_Reg_GMA(pNewGMA);
            IMG_MEMCPY(&(g_psCIDriver->sGammaLUT), pNewGMA,
                sizeof(CI_MODULE_GMA_LUT));
            ret = IMG_SUCCESS;
        }

    }
    SYS_LockRelease(&(g_psCIDriver->sConnectionLock));

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hRegGammaLut, " - done");
    return ret;
}

IMG_RESULT KRN_CI_DriverAcquireGasket(IMG_UINT32 uiGasket,
    KRN_CI_CONNECTION *pConnection)
{
    IMG_RESULT ret = IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
    int bDGUsesGasket = IMG_FALSE;

    IMG_ASSERT(g_psCIDriver != NULL);
    IMG_ASSERT(uiGasket < CI_N_IMAGERS);

    if (bDGUsesGasket)
    {
        CI_FATAL("Cannot acquire gasket %d - internal DG is using it\n",
            uiGasket);
        return ret;
    }

    SYS_LockAcquire(&(g_psCIDriver->sConnectionLock));
    // lock driver to protect pActiveDatagen
#ifndef INFOTM_USE_SPIN_LOCK // added by linyun.xiong @2016-03-22
    SYS_LockAcquire(&(g_psCIDriver->sActiveContextLock));
#else
	SYS_SpinlockAcquire(&(g_psCIDriver->sActiveContextLock));
#endif
    {
        if (g_psCIDriver->pActiveDatagen != NULL)
        {
            bDGUsesGasket =
                g_psCIDriver->pActiveDatagen->userDG.ui8Gasket == uiGasket;
        }
        if (!g_psCIDriver->apGasketUser[uiGasket] && !bDGUsesGasket)
        {
            g_psCIDriver->apGasketUser[uiGasket] = pConnection;
            ret = IMG_SUCCESS;
        }

    }
#ifndef INFOTM_USE_SPIN_LOCK // added by linyun.xiong @2016-03-22
    SYS_LockRelease(&(g_psCIDriver->sActiveContextLock));
#else
	SYS_SpinlockRelease(&(g_psCIDriver->sActiveContextLock));
#endif
    SYS_LockRelease(&(g_psCIDriver->sConnectionLock));

    if (IMG_SUCCESS == ret)
    {
        CI_INFO("acquire gasket %d\n", uiGasket);
    }
    return ret;
}

IMG_BOOL8 KRN_CI_DriverCheckGasket(IMG_UINT32 uiGasket,
    KRN_CI_CONNECTION *pConnection)
{
    IMG_BOOL8 bOwner = IMG_FALSE;

    SYS_LockAcquire(&(g_psCIDriver->sConnectionLock));
    {
        if (g_psCIDriver->apGasketUser[uiGasket] == pConnection)
        {
            bOwner = IMG_TRUE;
        }
    }
    SYS_LockRelease(&(g_psCIDriver->sConnectionLock));

    return bOwner;
}

void KRN_CI_DriverReleaseGasket(IMG_UINT32 uiGasket)
{
    IMG_ASSERT(g_psCIDriver != NULL);
    IMG_ASSERT(uiGasket < CI_N_IMAGERS);

    SYS_LockAcquire(&(g_psCIDriver->sConnectionLock));
    {
        g_psCIDriver->apGasketUser[uiGasket] = NULL;
    }
    SYS_LockRelease(&(g_psCIDriver->sConnectionLock));
}

IMG_RESULT KRN_CI_DriverAcquireDatagen(KRN_CI_DATAGEN *pDatagen)
{
    IMG_RESULT ret = IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;

    IMG_ASSERT(g_psCIDriver != NULL);
    IMG_ASSERT(pDatagen->userDG.ui8Gasket < CI_N_IMAGERS);
    IMG_ASSERT(pDatagen->userDG.ui8IIFDGIndex < CI_N_CONTEXT);

    // to protect apGasketUser
    SYS_LockAcquire(&(g_psCIDriver->sConnectionLock));
    // lock driver to protect pActiveDatagen
#ifndef INFOTM_USE_SPIN_LOCK // added by linyun.xiong @2016-03-22
    SYS_LockAcquire(&(g_psCIDriver->sActiveContextLock));
#else
	SYS_SpinlockAcquire(&(g_psCIDriver->sActiveContextLock));
#endif
    {
        if (!g_psCIDriver->apGasketUser[pDatagen->userDG.ui8Gasket]
            && !g_psCIDriver->pActiveDatagen)
        {
            g_psCIDriver->pActiveDatagen = pDatagen;
            ret = IMG_SUCCESS;
        }

    }
#ifndef INFOTM_USE_SPIN_LOCK // added by linyun.xiong @2016-03-22
    SYS_LockRelease(&(g_psCIDriver->sActiveContextLock));
#else
	SYS_SpinlockRelease(&(g_psCIDriver->sActiveContextLock));
#endif
    SYS_LockRelease(&(g_psCIDriver->sConnectionLock));

    if (IMG_SUCCESS == ret)
    {
        CI_INFO("acquire internal DG %d to replace data from gasket %d\n",
            pDatagen->userDG.ui8IIFDGIndex, pDatagen->userDG.ui8Gasket);
    }
    return ret;
}

IMG_RESULT KRN_CI_DriverReleaseDatagen(KRN_CI_DATAGEN *pDatagen)
{
    IMG_RESULT ret = IMG_ERROR_FATAL;

    // to protect apGasketUser
    SYS_LockAcquire(&(g_psCIDriver->sConnectionLock));
    // lock driver to protect pActiveDatagen
#ifndef INFOTM_USE_SPIN_LOCK // added by linyun.xiong @2016-03-22
    SYS_LockAcquire(&(g_psCIDriver->sActiveContextLock));
#else
    SYS_SpinlockAcquire(&(g_psCIDriver->sActiveContextLock));	
#endif
    {
        if (g_psCIDriver->pActiveDatagen == pDatagen)
        {
            g_psCIDriver->pActiveDatagen = NULL;
            ret = IMG_SUCCESS;
        }

    }
#ifndef INFOTM_USE_SPIN_LOCK // added by linyun.xiong @2016-03-22
    SYS_LockRelease(&(g_psCIDriver->sActiveContextLock));
#else
	SYS_SpinlockRelease(&(g_psCIDriver->sActiveContextLock));
#endif
    SYS_LockRelease(&(g_psCIDriver->sConnectionLock));

    if (IMG_SUCCESS != ret)
    {
        CI_FATAL("Cannot release not-acquired internal datagen %d\n",
            pDatagen->ui32Identifier);
    }
    return ret;
}

#if defined(FELIX_FAKE)
#if defined(WIN32)
#undef IOC_OUT  // to avoid warnings
#undef IOC_IN  // to avoid warnings
#undef IOC_INOUT  // to avoid warnings
#undef _IO  // to avoid warnings
#undef _IOR  // to avoid warnings
#undef _IOW  // to avoid warnings
#include <Windows.h>

#define FILETIME_EPOCH (116444736000000000i64)
#define FILETIME_TO_S_DIV (10000000)
#define FILETIME_TO_US_MULT 10
#define FILETIME_TO_US_MOD (1000000)

void getnstimeofday(struct timespec *ts)
{
    FILETIME t;
    ULARGE_INTEGER d;
    IMG_UINT64 v;
    IMG_ASSERT(ts);

    GetSystemTimeAsFileTime(&t);

    d.LowPart = t.dwLowDateTime;
    d.HighPart = t.dwHighDateTime;

    v = d.QuadPart;  // 100 ns interval
    v -= FILETIME_EPOCH;

    ts->tv_sec = (long)(v / FILETIME_TO_S_DIV);
    ts->tv_nsec = (long)((v * 10) % FILETIME_TO_US_MOD);
}
#else /* WIN32 */

void getnstimeofday(struct timespec *ts)
{
    struct timeval tv;
    IMG_ASSERT(ts);

    gettimeofday(&tv, NULL);
    ts->tv_sec = tv.tv_sec;
    ts->tv_nsec = tv.tv_usec*1000;
}
#endif /* WIN32 */
#endif /* FELIX_FAKE */
