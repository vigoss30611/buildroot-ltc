/**
*******************************************************************************
 @file ci_api.c

 @brief All the public functions that delegates to private elements of the CI
 driver

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

#include "ci/ci_api.h"
#include "ci/ci_api_internal.h"

#ifdef IMG_KERNEL_MODULE
#error "this should not be in a kernel module"
#endif

#include "ci_internal/ci_errors.h" // toErrno and toImgResult
#include "ci_kernel/ci_ioctrl.h"

#include "sys/sys_userio.h"

#ifdef WIN32
#ifndef MAP_FAILED
#define MAP_FAILED ((void *)-1)
#endif
#endif

#define FELIX_DEV "/dev/imgfelix0"

#ifdef FELIX_FAKE

#include "ci_kernel/ci_kernel.h" // to access g_psCIDriver
#include "ci_kernel/ci_connection.h"
// fake device
#include "sys/sys_userio_fake.h"

static int DEV_CI_Munmap(void *addr, size_t length)
{
    (void)addr;
    (void)length;
    return 0;
}

#else // FELIX_FAKE

/**
 * @brief used for SYS_IO_Open() as a fake pointer to the 
 * struct file_operations that is not used anyway
 */
static int gFakeOps;

#endif // FELIX_FAKE

#ifdef IMG_MALLOC_CHECK
// needed from img_defs definitions of MALLOC
IMG_UINT32 gui32Alloc = 0;
IMG_UINT32 gui32Free = 0;
#endif

#define LOG_TAG "CI_API"
#include <felixcommon/userlog.h>

/*-----------------------------------------------------------------------------
 * Following elements are in in static scope
 *---------------------------------------------------------------------------*/

/**
 * @ingroup INT_FCT
 */
static IMG_BOOL8 List_ClearPipeline(void *listConfig, void *param)
{
    INT_PIPELINE *pIntPipe = (INT_PIPELINE*)listConfig;
    //pPipeline->pConnection = NULL;
    CI_PipelineDestroy(&(pIntPipe->publicPipeline));
    return IMG_TRUE;
}

/**
 * @ingroup INT_FCT
 * @brief Compute the sizes of the linestore structure using the start points
 *
 * The size of context[i] is computed from its start point up to the start of
 * the next context.
 * If the next starting context is outside of the maximum size for context[i]
 * the maximum single size is used.
 *
 * It is possible that a non-started context has a size of 0
 *
 * @InOut pLinestore structure to read and update
 * @Input ui32Max size of the linestore buffer
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_VALUE_OUT_OF_RANGE if a starting point is after the last
 * possible element (ui32Max-1)
 * @return IMG_ERROR_FATAL if the sum of the element's size is > ui32Max
 * (should not happen as size is computed in this function)
 */
static IMG_RESULT CI_LinestoreComputeSizes(const CI_HWINFO *pHWInfo,
    CI_LINESTORE *pLinestore)
{
    IMG_UINT32 ui32Sum, i, j;
    //CI_HWINFO *pHWInfo = &(pConn->sHWInfo);
    IMG_UINT32 ui8NContext = pHWInfo->config_ui8NContexts;

    // first compute the sizes
    ui32Sum = 0;
    for (i = 0 ; i < ui8NContext ; i++)
    {
        IMG_INT32 size;

        if (pLinestore->aStart[i] < 0)
        {
            pLinestore->aSize[i] = 0;
            continue; // this context is disabled
        }
        else 
            if ((IMG_UINT32)pLinestore->aStart[i] >= pHWInfo->ui32MaxLineStore)
        {
            LOG_ERROR("context %d linestore start (%u) is outside of the "\
                "maximum possible linestore (%u)\n",
                i, pLinestore->aStart[i], pHWInfo->ui32MaxLineStore);
            return IMG_ERROR_VALUE_OUT_OF_RANGE;
        }

        // assume this context[i] uses as much as it can
        size = IMG_MIN_INT(pHWInfo->context_aMaxWidthSingle[i],
            pHWInfo->ui32MaxLineStore - pLinestore->aStart[i]);
        for (j = 0 ; j < ui8NContext ; j++)
        {
            /* if context[j] is after context[i] the size of context[i]
             * is recomputed */
            // context[j] starts inside the max of context[i]
            if (i != j && pLinestore->aStart[j] > pLinestore->aStart[i])
            {
                /* take the minimum: if context[j] is too far for context[i]
                 * to reach it size is unchanged (but there is a gap) */
                size = IMG_MIN_INT(size,
                    pLinestore->aStart[j]-pLinestore->aStart[i]);
            }
        }

        pLinestore->aSize[i] = size;

        if (pLinestore->aSize[i] > pHWInfo->context_aMaxWidthSingle[i])
        {
            LOG_ERROR("context %d linestore (start %u - size %u) is bigger "\
                "than its maximum buffer size (%u)\n",
                i, pLinestore->aStart[i], pLinestore->aSize[i],
                pHWInfo->context_aMaxWidthMult[i]);
            return IMG_ERROR_VALUE_OUT_OF_RANGE;
        }

        ui32Sum += size;
    }

    // this should never happen as we are computing size...
    if (ui32Sum > pHWInfo->ui32MaxLineStore)
    {
        LOG_ERROR("internal error while computing the linestore sizes: "\
            "sum>maximum\n");
        return IMG_ERROR_FATAL;
    }

    return IMG_SUCCESS;
}

/*-----------------------------------------------------------------------------
 * Following functions are part of the CI library
 *---------------------------------------------------------------------------*/

IMG_RESULT CI_DriverInit(CI_CONNECTION **ppConnection)
{
    IMG_RESULT ret = IMG_SUCCESS;
    INT_CONNECTION *pIntConn = NULL;
    void *fakeOps = NULL; // used only if fake device

    if (!ppConnection)
    {
        LOG_ERROR("ppConnection is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntConn = (INT_CONNECTION*)IMG_CALLOC(1, sizeof(INT_CONNECTION));

    if ( !pIntConn )
    {
        LOG_ERROR("failed to create internal connection object (%lu B)\n",
            (unsigned long)sizeof(INT_CONNECTION));
        return IMG_ERROR_MALLOC_FAILED;
    }

    ret = List_init(&(pIntConn->sList_pipelines));
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to create internal pipeline list\n");
        IMG_FREE(pIntConn);
        pIntConn = NULL;
        return ret;
    }

    ret = List_init(&(pIntConn->sList_datagen));
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to create internal datagen list\n");
        IMG_FREE(pIntConn);
        pIntConn = NULL;
        return ret;
    }

#ifdef FELIX_FAKE
    // kernel object should have been created first
    if (!g_psCIDriver)
    {
        LOG_ERROR("device not found (fake insmod not done)\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    IMG_ASSERT(g_psCIDriver->pDevice!=NULL);
    /* because the kernel does not have unmap (it does it automatically),
     * but fake interface needs to do it manually */
    g_psCIDriver->pDevice->sFileOps.munmap = &DEV_CI_Munmap; 
    fakeOps = (void*)&(g_psCIDriver->pDevice->sFileOps);
#endif

    pIntConn->fileDesc = SYS_IO_Open(FELIX_DEV, O_RDWR, fakeOps);
    if (pIntConn->fileDesc)
    {
        ret = SYS_IO_Control(pIntConn->fileDesc, CI_IOCTL_INFO,
            (long int)&(pIntConn->publicConnection)); //INT_CI_DriverConnect()
        if (0 != ret)
        {
            LOG_ERROR("Failed to ask for driver informations (returned %d)\n",
                ret);
            SYS_IO_Close(pIntConn->fileDesc);
            IMG_FREE(pIntConn);
            return toImgResult(ret);
        }

        if (sizeof(CI_PIPELINE) !=
            pIntConn->publicConnection.sHWInfo.uiSizeOfPipeline)
        {
            LOG_ERROR("The kernel-side CI_PIPELINE driver is not compatible "\
                "with the user-side. Are you using the correct driver/app?\n");
            SYS_IO_Close(pIntConn->fileDesc);
            IMG_FREE(pIntConn);
            return IMG_ERROR_FATAL;
        }

        *ppConnection = &(pIntConn->publicConnection);
        ret = IMG_SUCCESS;
    }
    else
    {
        LOG_ERROR("Failed to initialise the device\n");
        IMG_FREE(pIntConn);
        ret = IMG_ERROR_FATAL;
    }

    return ret;
}

IMG_RESULT CI_DriverFinalise(CI_CONNECTION *pConnection)
{
    IMG_RESULT ret;
    INT_CONNECTION *pIntCon = NULL;

    if (!pConnection)
    {
        LOG_ERROR("pConnection is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntCon = container_of(pConnection, INT_CONNECTION, publicConnection);
    IMG_ASSERT(pIntCon);

    List_visitor(&(pIntCon->sList_pipelines), NULL, &List_ClearPipeline);

    ret = SYS_IO_Close(pIntCon->fileDesc);
    if (0 != ret)
    {
        LOG_ERROR("Failed to close the device (returned %d)\n", ret);
    }
    else
    {
        IMG_FREE(pIntCon);
    }

    return ret;
}

IMG_RESULT CI_DriverVerifLinestore(CI_CONNECTION *pConnection,
    CI_LINESTORE *pLinestore)
{
    IMG_RESULT ret;
    IMG_UINT32 i, j;
    IMG_UINT32 enabled = 0;

    if (!pLinestore || !pConnection)
    {
        LOG_ERROR("pLinestore or pConnection is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    for (i = 0 ; i < pConnection->sHWInfo.config_ui8NContexts ; i++)
    {
        if (pLinestore->aStart[i] < 0)
        {
#ifdef INFOTM_ENABLE_WARNING_DEBUG
            LOG_INFO("context %d linestore disabled\n", i);
#endif //INFOTM_ENABLE_WARNING_DEBUG
            continue;
        }
        enabled++;
        for (j = i+1 ; j < pConnection->sHWInfo.config_ui8NContexts ; j++)
        {
            if (pLinestore->aStart[j] > 0
                && pLinestore->aStart[i] == pLinestore->aStart[j])
            {
                LOG_ERROR("linestore for context %d and %d start at the "\
                    "same position\n", i, j);
                return IMG_ERROR_FATAL;
            }
        }
    }

    if (0 == enabled)
    {
        LOG_ERROR("all context's linestore are disabled\n");
        return IMG_ERROR_FATAL;
    }

    ret = CI_LinestoreComputeSizes(&(pConnection->sHWInfo), pLinestore);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to compute linestores' size\n");
        return ret;
    }

    return IMG_SUCCESS;
}

IMG_RESULT CI_DriverGetLinestore(CI_CONNECTION *pConnection)
{
    IMG_RESULT ret;
    CI_LINESTORE tmp;
    INT_CONNECTION *pIntCon = NULL;

    if (!pConnection)
    {
        LOG_ERROR("pConnection is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntCon = container_of(pConnection, INT_CONNECTION, publicConnection);
    IMG_ASSERT(pIntCon);

    ret = SYS_IO_Control(pIntCon->fileDesc, CI_IOCTL_LINE_GET, (long)&(tmp)); //INT_CI_DriverGetLineStore()
    if (0 != ret)
    {
        LOG_ERROR("Failed to update the linestore information\n");
        return toImgResult(ret);
    }

    IMG_MEMCPY(&(pConnection->sLinestoreStatus), &tmp, sizeof(CI_LINESTORE));

    return IMG_SUCCESS;
}

IMG_RESULT CI_DriverSetLinestore(CI_CONNECTION *pConnection,
    CI_LINESTORE *pLinestore)
{
    IMG_RESULT ret;
    INT_CONNECTION *pIntCon = NULL;

    if (!pLinestore || !pConnection)
    {
        LOG_ERROR("pLinestore or pConnection is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntCon = container_of(pConnection, INT_CONNECTION, publicConnection);
    IMG_ASSERT(pIntCon);

    ret = CI_DriverVerifLinestore(pConnection, pLinestore);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("Given linestore is not correct\n");
        return ret;
    }

    ret = SYS_IO_Control(pIntCon->fileDesc, CI_IOCTL_LINE_SET,
        (long)pLinestore); //INT_CI_DriverSetLineStore()
    if (0 != ret)
    {
        LOG_ERROR("Failed to propose a new linestore\n");
        return toImgResult(ret);
    }

    // on success copy the proposed linestore to the structure
    if (pLinestore != &(pConnection->sLinestoreStatus))
    {
        IMG_MEMCPY(&(pConnection->sLinestoreStatus), pLinestore,
            sizeof(CI_LINESTORE));
    }

    return IMG_SUCCESS;
}

IMG_RESULT CI_DriverGetGammaLUT(CI_CONNECTION *pConnection)
{
    IMG_RESULT ret;
    CI_MODULE_GMA_LUT tmp;
    INT_CONNECTION *pIntCon = NULL;

    if (!pConnection)
    {
        LOG_ERROR("pConnection is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntCon = container_of(pConnection, INT_CONNECTION, publicConnection);
    IMG_ASSERT(pIntCon);

    ret = SYS_IO_Control(pIntCon->fileDesc, CI_IOCTL_GMAL_GET, (long)&(tmp)); //INT_CI_DriverGetGammaLUT()
    if (0 != ret)
    {
        LOG_ERROR("Failed to update the Gamma LUT information\n");
        return toImgResult(ret);
    }

    IMG_MEMCPY(&(pConnection->sGammaLUT), &tmp, sizeof(CI_MODULE_GMA_LUT));

    return IMG_SUCCESS;
}

IMG_RESULT CI_DriverSetGammaLUT(CI_CONNECTION *pConnection,
    const CI_MODULE_GMA_LUT *pNewGammaLUT)
{
    IMG_RESULT ret;
    INT_CONNECTION *pIntCon = NULL;

    if (!pConnection || !pNewGammaLUT)
    {
        LOG_ERROR("pConnection or pNewGammaLUT is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntCon = container_of(pConnection, INT_CONNECTION, publicConnection);
    IMG_ASSERT(pIntCon);

    ret = SYS_IO_Control(pIntCon->fileDesc, CI_IOCTL_GMAL_SET,
        (long)pNewGammaLUT); //INT_CI_DriverSetGammaLUT()
    if (0 != ret)
    {
        LOG_ERROR("Failed to set a new Gamma LUT\n");
        return toImgResult(ret);
    }

    if (pNewGammaLUT != &(pConnection->sGammaLUT))
    {
        IMG_MEMCPY(&(pConnection->sGammaLUT), pNewGammaLUT,
            sizeof(CI_MODULE_GMA_LUT));
    }

    return IMG_SUCCESS;
}

IMG_RESULT CI_DriverGetTimestamp(CI_CONNECTION *pConnection,
    IMG_UINT32 *puiCurrentStamp)
{
    IMG_RESULT ret;
    IMG_UINT32 timestamp;
    INT_CONNECTION *pIntCon = NULL;

    if (!pConnection || !puiCurrentStamp)
    {
        LOG_ERROR("pConnection or puiCurrentStamp is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntCon = container_of(pConnection, INT_CONNECTION, publicConnection);
    IMG_ASSERT(pIntCon);

    ret = SYS_IO_Control(pIntCon->fileDesc, CI_IOCTL_TIME_GET,
        (long)(&timestamp)); //INT_CI_DriverGetTimestamp()
    if (0 != ret)
    {
        LOG_ERROR("Failed to get the timestamp\n");
        return toImgResult(ret);
    }

    *puiCurrentStamp = timestamp;
    return IMG_SUCCESS;
}

IMG_RESULT CI_DriverGetAvailableInternalDPFBuffer(CI_CONNECTION *pConnection,
    IMG_UINT32 *puiAvailableDPF)
{
    IMG_RESULT ret;
    IMG_UINT32 dpfval;
    INT_CONNECTION *pIntCon = NULL;

    if (!pConnection)
    {
        LOG_ERROR("pConnection is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntCon = container_of(pConnection, INT_CONNECTION, publicConnection);
    IMG_ASSERT(pIntCon);

    if (!puiAvailableDPF)
    {
        LOG_ERROR("puiAvailableDPF is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    ret = SYS_IO_Control(pIntCon->fileDesc, CI_IOCTL_DPFI_GET,
        (long)(&dpfval)); //INT_CI_DriverGetDPFInternal()
    if (0 != ret)
    {
        LOG_ERROR("Failed to get the current DPF internal buffer value\n");
        return toImgResult(ret);
    }

    *puiAvailableDPF = dpfval;
    return IMG_SUCCESS;
}

IMG_RESULT CI_DriverGetRTMInfo(CI_CONNECTION *pConnection, CI_RTM_INFO *pRTM)
{
    IMG_RESULT ret;
    INT_CONNECTION *pIntCon = NULL;

    if (!pConnection)
    {
        LOG_ERROR("pConnection is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntCon = container_of(pConnection, INT_CONNECTION, publicConnection);
    IMG_ASSERT(pIntCon);

    if (!pRTM)
    {
        LOG_ERROR("pRTM is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    ret = SYS_IO_Control(pIntCon->fileDesc, CI_IOCTL_RTM_GET,
        (long)(pRTM));
    if (0 != ret)
    {
        LOG_ERROR("Failed to get the RTM values\n");
        return toImgResult(ret);
    }

    return ret;
}

IMG_RESULT CI_DriverDebugRegRead(CI_CONNECTION *pConnection,
    IMG_UINT32 eBank, IMG_UINT32 ui32Offset, IMG_UINT32 *pResult)
{
    IMG_RESULT ret;
    INT_CONNECTION *pIntCon = NULL;
    struct CI_DEBUG_REG_PARAM param;

    if (!pConnection || !pResult)
    {
        LOG_ERROR("pConnection or pResult is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntCon = container_of(pConnection, INT_CONNECTION, publicConnection);
    IMG_ASSERT(pIntCon);

    param.bRead = IMG_TRUE;
    param.eBank = eBank;
    param.ui32Offset = ui32Offset;
    param.ui32Value = 0;

    ret = SYS_IO_Control(pIntCon->fileDesc, CI_IOCTL_DBG_REG, (long)&param);
    if (ret)
    {
        if (-ENOTTY == ret)
        {
            LOG_ERROR("Kernel side does not support debug functions\n");
            return IMG_ERROR_NOT_SUPPORTED;
        }
        else
        {
            LOG_ERROR("Failed to read register value\n");
            return toImgResult(ret);
        }
    }

    *pResult = param.ui32Value;
    return IMG_SUCCESS;
}

IMG_RESULT CI_DriverDebugRegWrite(CI_CONNECTION *pConnection,
    IMG_UINT32 eBank, IMG_UINT32 ui32Offset, IMG_UINT32 ui32Value)
{
    IMG_RESULT ret;
    INT_CONNECTION *pIntCon = NULL;
    struct CI_DEBUG_REG_PARAM param;

    if (!pConnection)
    {
        LOG_ERROR("pConnection or pResult is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntCon = container_of(pConnection, INT_CONNECTION, publicConnection);
    IMG_ASSERT(pIntCon);

    param.bRead = IMG_FALSE;
    param.eBank = eBank;
    param.ui32Offset = ui32Offset;
    param.ui32Value = ui32Value;

    ret = SYS_IO_Control(pIntCon->fileDesc, CI_IOCTL_DBG_REG, (long)&param);
    if (ret)
    {
        if (-ENOTTY == ret)
        {
            LOG_ERROR("Kernel side does not support debug functions\n");
            return IMG_ERROR_NOT_SUPPORTED;
        }
        else
        {
            LOG_ERROR("Failed to write register value\n");
            return toImgResult(ret);
        }
    }

    return IMG_SUCCESS;
}

#if defined(INFOTM_ISP)
IMG_RESULT CI_DriverGetReg(CI_CONNECTION *pConnection, CI_REG_PARAM *pRegParam)
{
    IMG_RESULT ret;
    INT_CONNECTION *pIntCon = NULL;

    if ( pConnection == NULL )
    {
        LOG_ERROR("pConnection is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntCon = container_of(pConnection, INT_CONNECTION, publicConnection);
    IMG_ASSERT(pIntCon);

    if ( pRegParam == NULL )
    {
        LOG_ERROR("pRegParam is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    if ( (ret = SYS_IO_Control(pIntCon->fileDesc, CI_IOCTL_REG_GET, (long)(pRegParam))) != 0 )
    {
        LOG_ERROR("Failed to get the register value\n");
        return toImgResult(ret);
    }

    return IMG_SUCCESS;
}

IMG_RESULT CI_DriverSetReg(CI_CONNECTION *pConnection, CI_REG_PARAM *pRegParam)
{
    IMG_RESULT ret;
    INT_CONNECTION *pIntCon = NULL;

    if ( pConnection == NULL )
    {
        LOG_ERROR("pConnection is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntCon = container_of(pConnection, INT_CONNECTION, publicConnection);
    IMG_ASSERT(pIntCon);

    if ( pRegParam == NULL )
    {
        LOG_ERROR("pRegParam is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    if ( (ret = SYS_IO_Control(pIntCon->fileDesc, CI_IOCTL_REG_SET, (long)(pRegParam))) != 0 )
    {
        LOG_ERROR("Failed to set the register value\n");
        return toImgResult(ret);
    }

    return IMG_SUCCESS;
}

#endif //INFOTM_ISP
