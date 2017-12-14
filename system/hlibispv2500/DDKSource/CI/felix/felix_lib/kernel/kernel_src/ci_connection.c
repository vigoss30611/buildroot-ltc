/**
 ******************************************************************************
 @file ci_connection.c

 @brief

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

 *****************************************************************************/
#ifdef INFOTM_ISP
#include <asm/io.h>
#endif //INFOTM_ISP
#include "ci_kernel/ci_kernel.h"
#include "ci_kernel/ci_kernel_structs.h"
#include "ci_kernel/ci_connection.h"
#include "ci_kernel/ci_ioctrl.h" // for struct CI_POOL_PARAM
// to know if we are using tcp when running fake interface
#include "ci_kernel/ci_debug.h" 

#include "ci/ci_api_structs.h"

#include <img_types.h>
#include <img_defs.h>
#include <img_errors.h>
#ifdef INFOTM_ISP
#include <tal.h>
#endif //INFOTM_ISP

#include "linkedlist.h"
#include "mmulib/mmu.h" // to clean the pages when last connection is removed

#include "ci_kernel/ci_hwstruct.h"

#ifdef FELIX_FAKE

#include <errno.h>
// in user mode debuge copy to and from "kernel" does not make a difference
/* copy_to_user and copy_from_user return the amount of memory still to be
 * copied: i.e. 0 is the success result*/
#define copy_from_user(to, from, size) ( IMG_MEMCPY(to, from, size) == NULL ?\
    size : 0 )
#define copy_to_user(to, from, size) ( IMG_MEMCPY(to, from, size) == NULL ?\
    size : 0 )
#define __user

#else
#include <asm/uaccess.h> // copy_to_user and copy_from_user
#include <linux/compiler.h> // for the __user macro
#include <linux/kernel.h> // container_of
#include <linux/errno.h>
#endif

#ifdef INFOTM_ISP
int IsLowLight;
unsigned int g_ui32Size;
SYS_MEM g_buffer;
#endif

static void ListDestroy_KRN_CI_Pipeline(void *pObject)
{
    KRN_CI_PipelineDestroy((KRN_CI_PIPELINE*)pObject);
}

static void ListDestroy_KRN_CI_DATAGEN(void *pObject)
{
    KRN_CI_DatagenClear((KRN_CI_DATAGEN*)pObject);
}

// returns true if visiter should continue visiting the list
static IMG_BOOL8 ListVisitor_findPipeline(void* listElem, void* lookingFor)
{
    int *lookingForId = (int *)lookingFor;
    KRN_CI_PIPELINE *pConfig = (KRN_CI_PIPELINE*)listElem;

    return !(pConfig->ui32Identifier == *lookingForId);
}

// returns true if visiter should continue visiting the list
static IMG_BOOL8 ListVisitor_findDatagen(void* listElem, void* lookingFor)
{
    int *lookingForId = (int *)lookingFor;
    KRN_CI_DATAGEN *pDG = (KRN_CI_DATAGEN*)listElem;

    return !(pDG->ui32Identifier == *lookingForId);
}

static sCell_T* FindPipeline(KRN_CI_CONNECTION *pConnection, int pipelineId)
{
    sCell_T *pFound = NULL;

    if (pipelineId <= 0)
    {
        CI_FATAL("pipeline configuration is not registered\n");
        return NULL;
    }

    SYS_LockAcquire(&(pConnection->sLock));
    {
        pFound = List_visitor(&(pConnection->sList_pipeline),
            (void*)&pipelineId, &ListVisitor_findPipeline);
    }
    SYS_LockRelease(&(pConnection->sLock));

    return pFound;
}

static sCell_T* FindDatagen(KRN_CI_CONNECTION *pConnection, int datagenId)
{
    sCell_T *pFound = NULL;

    if (datagenId <= 0)
    {
        CI_FATAL("datagen is not registered\n");
        return NULL;
    }

    SYS_LockAcquire(&(pConnection->sLock));
    {
        pFound = List_visitor(&(pConnection->sList_iifdg), (void*)&datagenId,
            &ListVisitor_findDatagen);
    }
    SYS_LockRelease(&(pConnection->sLock));

    return pFound;
}

struct count_status
{
    enum KRN_CI_SHOT_eSTATUS eLookingFor;
    int result;
};

static IMG_BOOL8 List_FindShotWithStatus(void *elem, void *param)
{
    struct count_status *pParam = (struct count_status*)param;
    KRN_CI_SHOT *pShot = (KRN_CI_SHOT*)elem;

    if (pShot->eStatus == pParam->eLookingFor)
    {
        pParam->result++;
    }

    return IMG_TRUE;
}

// used in ci_intdg_km.c too
IMG_BOOL8 List_FindDGBuffer(void *elem, void *param)
{
    int *identifier = (int*)param;
    KRN_CI_DGBUFFER *pBuffer = (KRN_CI_DGBUFFER*)elem;

    if (pBuffer->ID == *identifier)
    {
        return IMG_FALSE;
    }
    return IMG_TRUE;
}

IMG_RESULT INT_CI_PipelineRegister(KRN_CI_CONNECTION *pConnection,
    CI_PIPELINE __user *pUserConfig, int *pGivenId)
{
#ifdef INFOTM_ISP
    static KRN_CI_PIPELINE *pStaPrevPipeline = NULL;
#endif //INFOTM_ISP
    KRN_CI_PIPELINE *pKrnConfig = NULL;
    IMG_RESULT ret = IMG_SUCCESS;

    IMG_ASSERT(pConnection != NULL);
    IMG_ASSERT(pGivenId != NULL);

    pKrnConfig = (KRN_CI_PIPELINE*)IMG_CALLOC(1, sizeof(KRN_CI_PIPELINE));
    if (pKrnConfig == NULL)
    {
        CI_FATAL("allocation of the pipeline failed (%" IMG_SIZEPR "u B)\n",
            sizeof(KRN_CI_PIPELINE));
        return IMG_ERROR_MALLOC_FAILED;
    }
	
    ret = copy_from_user(&(pKrnConfig->userPipeline), pUserConfig,
        sizeof(CI_PIPELINE));
    if (ret)
    {
        CI_FATAL("failed to copy_from_user pUserConfig\n");
        IMG_FREE(pKrnConfig);
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    if (KRN_CI_PipelineVerify(&(pKrnConfig->userPipeline),
        &(g_psCIDriver->sHWInfo)) != IMG_TRUE)
    {
        CI_FATAL("Cannot apply this configuration on that version of "\
            "the HW\n");
        IMG_FREE(pKrnConfig);
        return IMG_ERROR_NOT_SUPPORTED;
    }

    ret = KRN_CI_PipelineInit(pKrnConfig, pConnection);
    if (IMG_SUCCESS != ret)
    {
        CI_FATAL("Failed to create kernel side Config object\n");
        return ret;
    }

    /* update the deshading matrix because it needs to be done ASAP so that
     * user memory can be reused if need be*/
    if (pKrnConfig->userPipeline.sDeshading.bUseDeshadingGrid == IMG_TRUE)
    {
        ret = INT_CI_PipelineSetDeshading(pKrnConfig);
        if (IMG_SUCCESS != ret)
        {
            CI_FATAL("failed to get the deshading matrix - returned %d\n",
                ret);
            KRN_CI_PipelineDestroy(pKrnConfig);
            return IMG_ERROR_INVALID_PARAMETERS;
        }
    }

    if ((pKrnConfig->userPipeline.sDefectivePixels.\
        eDPFEnable | CI_DPF_READ_MAP_ENABLED))
    {
        /** @ a new method of updates for DPF should be found to avoid
         * having to do multi-level copy_from_user */
        // warning: the internal buffer is reserved when starting the capture!
        ret = INT_CI_PipelineSetDPFRead(pKrnConfig,
            pKrnConfig->userPipeline.sDefectivePixels.sInput.apDefectInput);
        if (IMG_SUCCESS != ret)
        {
            CI_FATAL("failed to get the DPF read map - returned %d\n", ret);
            KRN_CI_PipelineDestroy(pKrnConfig);
            return IMG_ERROR_INVALID_PARAMETERS;
        }
    }

    // no update - it is done when starting it
    /*if ( (ret=KRN_CI_PipelineUpdate(pKrnConfig, IMG_TRUE)) != IMG_SUCCESS )
      {
      CI_FATAL("failed to apply the initial update\n");
      KRN_CI_PipelineDestroy(pKrnConfig);
      return ret;
      }*/

    SYS_LockAcquire(&(pConnection->sLock));
    {
        // pre-incremental to avoid ui32PipelineIDBase to be 0
        pKrnConfig->ui32Identifier = ++(pConnection->ui32PipelineIDBase);
#ifdef INFOTM_ISP
        if (pKrnConfig->userPipeline.uiTotalPipeline > 1)
        {
            pKrnConfig->pPrevPipeline = (void*)pStaPrevPipeline;

#ifdef INFOTM_ENABLE_ISP_DEBUG
        if (pStaPrevPipeline == NULL)
            printk("@@@ PipeRegis[%d]: ui32Identifier=%d\n", pKrnConfig->userPipeline.ui8Context, pKrnConfig->ui32Identifier);
        else
            printk("@@@ PipeRegis[%d]: ui32Identifier=%d, PrevContext[%d]\n", pKrnConfig->userPipeline.ui8Context, pKrnConfig->ui32Identifier,
                        pStaPrevPipeline->userPipeline.ui8Context);
#endif //INFOTM_ENABLE_ISP_DEBUG

            pStaPrevPipeline = pKrnConfig;
        }
#endif//INFOTM_ISP


        *pGivenId = pKrnConfig->ui32Identifier;
        ret = List_pushBackObject(&(pConnection->sList_pipeline), pKrnConfig);
        if (IMG_SUCCESS != ret)
        {
            CI_FATAL("failed to add the configuration to the "\
                "connection's list\n");
        }
    }
    SYS_LockRelease(&(pConnection->sLock));
    /* if lower than 0 it overflowed (not likely to happen
     * - means INT_MAX different config per connection)*/
    IMG_ASSERT(pKrnConfig->ui32Identifier > 0);

    if (IMG_SUCCESS != ret)
    {
        KRN_CI_PipelineDestroy(pKrnConfig);
    }

    return ret;
}

IMG_RESULT INT_CI_PipelineDeregister(KRN_CI_CONNECTION *pConnection,
    int ui32ConfigId)
{
    sCell_T *pCell = NULL;
    IMG_RESULT ret = IMG_SUCCESS;
    KRN_CI_PIPELINE *pKrnConfig = NULL;

    IMG_ASSERT(pConnection != NULL);

    pCell = FindPipeline(pConnection, ui32ConfigId);
    if (!pCell)
    {
        CI_FATAL("configuration not found in the driver's list\n");
        return IMG_ERROR_FATAL;
    }

    pKrnConfig = (KRN_CI_PIPELINE*)pCell->object;
    IMG_ASSERT(pKrnConfig != NULL);

    if (pKrnConfig->bStarted)
    {
        ret = KRN_CI_PipelineStopCapture(pKrnConfig);

        /// @ what should we do if the stop fails? abort?
    }

    SYS_LockAcquire(&(pConnection->sLock));
    {
        ret = List_remove(pCell);
    }
    SYS_LockRelease(&(pConnection->sLock));

    IMG_ASSERT(ret == IMG_SUCCESS); // removing from a list should never fail

    ret = KRN_CI_PipelineDestroy(pKrnConfig);
    if (IMG_SUCCESS != ret)
    {
        /** @ what should we do if it occurs? repush the pipeline in
         * the list? */
        CI_FATAL("failed to destroy configuration\n");
    }

    return ret;
}

IMG_RESULT KRN_CI_ConnectionCreate(KRN_CI_CONNECTION **ppConnection)
{
    IMG_RESULT ret = IMG_SUCCESS;

    IMG_ASSERT(ppConnection != NULL);

    if (!g_psCIDriver)
    {
        return IMG_ERROR_NOT_SUPPORTED;
    }
    if (*ppConnection)
    {
        return IMG_ERROR_MEMORY_IN_USE;
    }

    *ppConnection = (KRN_CI_CONNECTION*)IMG_CALLOC(1,
        sizeof(KRN_CI_CONNECTION));
    if (!*ppConnection)
    {
        CI_FATAL("failed to allocate connection structure (%ld bytes)\n",
            sizeof(KRN_CI_CONNECTION));
        return IMG_ERROR_MALLOC_FAILED;
    }

    ret = SYS_LockInit(&((*ppConnection)->sLock));
    if (IMG_SUCCESS != ret)
    {
        CI_FATAL("failed to create new connection's lock\n");
        goto connect_exit_lock;
    }

    ret = List_init(&((*ppConnection)->sList_pipeline));
    if (IMG_SUCCESS != ret)
    {
        CI_FATAL("failed to create the configuration list\n");
        goto connect_exit_config;
    }

    ret = List_init(&((*ppConnection)->sList_iifdg));
    if (IMG_SUCCESS != ret)
    {
        CI_FATAL("failed to create the internal datagen list\n");
        goto connect_exit_config;
    }

    ret = List_init(&((*ppConnection)->sList_unmappedShots));
    if (IMG_SUCCESS != ret)
    {
        CI_FATAL("failed to create the unmapped Shot list\n");
        goto connect_exit_config;
    }

    ret = List_init(&((*ppConnection)->sList_unmappedBuffers));
    if (IMG_SUCCESS != ret)
    {
        CI_FATAL("failed to create the unmapped Buffer list\n");
        goto connect_exit_config;
    }

    ret = List_init(&((*ppConnection)->sList_unmappedDGBuffers));
    if (IMG_SUCCESS != ret)
    {
        CI_FATAL("failed to create the unmapped DGBuffer list\n");
        goto connect_exit_config;
    }

    SYS_LockAcquire(&(g_psCIDriver->sConnectionLock));
    {
        if (0 == g_psCIDriver->sList_connection.ui32Elements)
        {
            CI_INFO("1st connection: reset HW, request interrupt and "\
                "configure MMU\n");
            CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hRegFelixCore,
                "1st connection - start");

            // also turns power on
            if (0 != DEV_CI_Resume(g_psCIDriver->pDevice))
            {
                goto connect_exit;
            }

#ifdef FELIX_FAKE
            /* do not request the interrupt when using transif (because it
             * spawns a thread that should not be used*/
            if (bUseTCP == IMG_TRUE)
            {
#endif

#ifdef CI_DEBUGFS
                KRN_CI_ResetDebugFS();
#endif

                ret = SYS_DevRequestIRQ(g_psCIDriver->pDevice, "felix");
                if (IMG_SUCCESS != ret)
                {
                    CI_FATAL("Failed to request IRQ!\n");
                    goto connect_exit;
                }

#ifdef FELIX_FAKE
            }
#endif

            CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hRegFelixCore,
                "1st connection - done");
        }

        List_pushBackObject(&(g_psCIDriver->sList_connection),
            (*ppConnection));

        IMG_MEMCPY(&((*ppConnection)->publicConnection.sLinestoreStatus),
            &(g_psCIDriver->sLinestoreStatus), sizeof(CI_LINESTORE));
        IMG_MEMCPY(&((*ppConnection)->publicConnection.sGammaLUT),
            &(g_psCIDriver->sGammaLUT), sizeof(CI_MODULE_GMA_LUT));
    }
    SYS_LockRelease(&(g_psCIDriver->sConnectionLock));

    // HW info struct is written only once thus no need to lock it
    IMG_MEMCPY(&((*ppConnection)->publicConnection.sHWInfo),
        &(g_psCIDriver->sHWInfo), sizeof(CI_HWINFO));

    return ret;

connect_exit:
    SYS_LockRelease(&(g_psCIDriver->sConnectionLock));
connect_exit_config:
    SYS_LockDestroy(&((*ppConnection)->sLock));
connect_exit_lock:
    IMG_FREE(*ppConnection);
    *ppConnection = NULL;
    return ret;
}

IMG_RESULT INT_CI_DriverConnect(KRN_CI_CONNECTION *pKrnConnection,
    CI_CONNECTION __user *pConnection)
{
    IMG_RESULT ret = IMG_SUCCESS;

    IMG_ASSERT(pKrnConnection != NULL);

    ret = copy_to_user(pConnection, &(pKrnConnection->publicConnection),
        sizeof(CI_CONNECTION));
    if (ret)
    {
        CI_FATAL("failed to copy_to_user pConnection\n");
        ret = IMG_ERROR_INVALID_PARAMETERS;
    }

    return ret;
}

IMG_RESULT INT_CI_DriverDisconnect(KRN_CI_CONNECTION *pConnection)
{
    IMG_RESULT ret = IMG_SUCCESS;

    if (pConnection)
    {
        ret = KRN_CI_ConnectionDestroy(pConnection);
        if (IMG_SUCCESS != ret)
        {
            CI_FATAL("failed to destroy the connection\n");
        }
    }

    return ret;
}

IMG_RESULT KRN_CI_ConnectionDestroy(KRN_CI_CONNECTION *pConnection)
{
    sCell_T *pFound = NULL;
    IMG_RESULT ret = IMG_SUCCESS;

    IMG_ASSERT(pConnection != NULL);

    // the device has been closed - should not be searched anymore
    //SYS_LockAcquire(&(pConnection->sLock));
    {
        ret = List_clearObjects(&(pConnection->sList_pipeline),
            &ListDestroy_KRN_CI_Pipeline);
        if (IMG_SUCCESS != ret)
        {
            CI_FATAL("failed to clear the pipeline list\n");
        }
        ret = List_clearObjects(&(pConnection->sList_iifdg),
            &ListDestroy_KRN_CI_DATAGEN);
        if (IMG_SUCCESS != ret)
        {
            CI_FATAL("failed to clear the internal data generator list\n");
        }
        pFound = List_visitor(&(pConnection->sList_unmappedShots), NULL,
            &ListDestroy_KRN_CI_Shot);
        if (pFound)
        {
            CI_FATAL("failed to clear the unmapped Shot list\n");
        }
        pFound = List_visitor(&(pConnection->sList_unmappedBuffers), NULL,
            &ListDestroy_KRN_CI_Buffer);
        if (pFound)
        {
            CI_FATAL("failed to clear the unmapped Buffer list\n");
        }
        pFound = List_visitor(&(pConnection->sList_unmappedDGBuffers), NULL,
            &ListDestroy_KRN_CI_DGBuffer);
        if (pFound)
        {
            CI_FATAL("failed to clear the unmapped Buffer list\n");
        }
        /* should clear the list as well but it does not matter as we are
         * destroying the container anyway */
    }
    //SYS_LockRelease(&(pConnection->sLock));

    SYS_LockDestroy(&(pConnection->sLock));

    // clear last because needs unmapping first
    if (g_psCIDriver)
    {
        IMG_UINT32 i;

        SYS_LockAcquire(&(g_psCIDriver->sConnectionLock));

        pFound = List_visitor(&(g_psCIDriver->sList_connection), pConnection,
            &ListVisitor_findPointer);

        if (!pFound)
        {
            CI_FATAL("connection was not registered in the driver!\n");
            ret = IMG_ERROR_FATAL;
        }
        else
        {
            ret = List_remove(pFound);

            // if this has any acquired gasket remove them
            for (i = 0; i < CI_N_IMAGERS; i++)
            {
                if (g_psCIDriver->apGasketUser[i] == pConnection)
                {
                    CI_GASKET sGasket;
                    sGasket.uiGasket = i;
                    CI_FATAL("release gasket %d on connection destruction\n",
                        i);
                    g_psCIDriver->apGasketUser[i] = NULL;
                    HW_CI_WriteGasket(&sGasket, 0);
                }
            }
        }

        if (g_psCIDriver->sList_connection.ui32Elements == 0)
        {
            CI_INFO("last connection closed: release interrupt and "\
                "pause MMU\n");
            CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hRegFelixCore,
                "last connection - start");
            SYS_DevFreeIRQ(g_psCIDriver->pDevice);

            // no more memory request
            ret = KRN_CI_MMUPause(&(g_psCIDriver->sMMU), IMG_TRUE);
            if (IMG_SUCCESS != ret)
            {
                CI_FATAL("failed to pause MMU! may create trouble to "\
                    "clean the directory...\n");
            }

            for (i = 0; g_psCIDriver->sMMU.apDirectory[0]
                && i < g_psCIDriver->sHWInfo.config_ui8NContexts; i++)
            {
                // remove the page tables allocated for that directory
                IMGMMU_DirectoryClean(g_psCIDriver->sMMU.apDirectory[i]);
            }

            // stop the clock is not necessary (auto-clocks)

#ifdef FELIX_FAKE
            CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hRegFelixCore,
                "HW power OFF");
#else
            CI_DEBUG("HW power OFF");
#endif
            ret = SYS_DevPowerControl(g_psCIDriver->pDevice, IMG_FALSE);
            if (IMG_SUCCESS != ret)
            {
                CI_FATAL("Failed to turn the power off!\n");
            }

            CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hRegFelixCore,
                "last connection - done");
        }


        SYS_LockRelease(&(g_psCIDriver->sConnectionLock));
    }

    IMG_FREE(pConnection);

    return ret;
}

IMG_RESULT INT_CI_PipelineStartCapture(KRN_CI_CONNECTION *pConnection,
    int ui32CaptureId)
{
    KRN_CI_PIPELINE *pKrnCapture = NULL;
    sCell_T *pFound = NULL;

    IMG_ASSERT(pConnection != NULL);

    pFound = FindPipeline(pConnection, ui32CaptureId);
    if (!pFound)
    {
        CI_FATAL("capture not found in that connection\n");
        return IMG_ERROR_FATAL;
    }
    pKrnCapture = (KRN_CI_PIPELINE*)pFound->object;
    IMG_ASSERT(pKrnCapture != NULL);

    return KRN_CI_PipelineStartCapture(pKrnCapture);
}

IMG_RESULT INT_CI_PipelineStopCapture(KRN_CI_CONNECTION *pConnection,
    int ui32CaptureId)
{
    KRN_CI_PIPELINE *pKrnCapture = NULL;
    sCell_T *pFound = NULL;

    IMG_ASSERT(pConnection != NULL);

    pFound = FindPipeline(pConnection, ui32CaptureId);
    if (!pFound)
    {
        CI_FATAL("capture not found in that connection\n");
        return IMG_ERROR_FATAL;
    }
    pKrnCapture = (KRN_CI_PIPELINE*)pFound->object;
    IMG_ASSERT(pKrnCapture != NULL);

    return KRN_CI_PipelineStopCapture(pKrnCapture);
}

IMG_RESULT INT_CI_PipelineIsStarted(KRN_CI_CONNECTION *pConnection,
    int ui32CaptureId, IMG_BOOL8 *bIsStarted)
{
    KRN_CI_PIPELINE *pKrnCapture = NULL;
    sCell_T *pFound = NULL;

    IMG_ASSERT(pConnection != NULL);
    IMG_ASSERT(bIsStarted != NULL);

    pFound = FindPipeline(pConnection, ui32CaptureId);
    if (!pFound)
    {
        CI_FATAL("capture not found in that connection\n");
        return IMG_ERROR_FATAL;
    }
    pKrnCapture = (KRN_CI_PIPELINE*)pFound->object;
    IMG_ASSERT(pKrnCapture != NULL);

    *bIsStarted = pKrnCapture->bStarted;
    return IMG_SUCCESS;
}

IMG_RESULT INT_CI_PipelineTriggerShoot(KRN_CI_CONNECTION *pConnection,
struct CI_BUFFER_TRIGG __user *userParam)
{
    KRN_CI_PIPELINE *pKrnCapture = NULL;
    sCell_T *pFound = NULL;
    struct CI_BUFFER_TRIGG sParam;
    IMG_RESULT ret;

    IMG_ASSERT(pConnection != NULL);

    ret = copy_from_user(&sParam, userParam, sizeof(struct CI_BUFFER_TRIGG));
    if (ret)
    {
        CI_FATAL("failed to copy CI_BUFFER_PARAM from user\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pFound = FindPipeline(pConnection, sParam.captureId);
    if (!pFound)
    {
        CI_FATAL("capture not found in that connection\n");
        return IMG_ERROR_FATAL;
    }
    pKrnCapture = (KRN_CI_PIPELINE*)pFound->object;

    ret = KRN_CI_PipelineTriggerShoot(pKrnCapture, sParam.bBlocking,
        &sParam.bufferIds);

    return ret;
}

IMG_RESULT INT_CI_PipelineAcquireShot(KRN_CI_CONNECTION *pConnection,
struct CI_BUFFER_PARAM __user *pUserParam)
{
    KRN_CI_PIPELINE *pKrnCapture = NULL;
    sCell_T *pFound = NULL;
    IMG_RESULT ret = IMG_SUCCESS;
    KRN_CI_SHOT *pShot = NULL;
    struct CI_BUFFER_PARAM param;
#if defined(W_TIME_PRINT)
    struct timespec t0, t1, t2, t3;

    getnstimeofday(&t0);
#endif

    IMG_ASSERT(pConnection != NULL);

    if (!pUserParam)
    {
        CI_FATAL("given CI_BUFFER_PARAM is NULL!\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    ret = copy_from_user(&param, pUserParam, sizeof(struct CI_BUFFER_PARAM));
    if (ret)
    {
        CI_FATAL("Failed to copy_from_user the capture buffer parameters\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pFound = FindPipeline(pConnection, param.captureId);
    if (!pFound)
    {
        CI_FATAL("Capture not registered\n");
        return IMG_ERROR_FATAL;
    }
    pKrnCapture = (KRN_CI_PIPELINE*)pFound->object;

#if defined(W_TIME_PRINT)
    getnstimeofday(&t1);

    W_TIME_PRINT(t0, t1, "copy_from_user+find");
#endif

    ret = KRN_CI_PipelineAcquireShot(pKrnCapture, param.bBlocking, &pShot);

#if defined(W_TIME_PRINT)
    getnstimeofday(&t2);

    W_TIME_PRINT(t1, t2, "KRN_CI_PipelineAcquireShot");
#endif

    if (IMG_SUCCESS == ret)
    {
        param.shotId = pShot->iIdentifier;
        IMG_MEMCPY(&(param.shot), &(pShot->userShot), sizeof(CI_SHOT));
        /* clear the addresses just to ensure nothing kernel-side is
         * propagated to user-side*/
        param.shot.pEncoderOutput = NULL;
        param.shot.pDisplayOutput = NULL;
        param.shot.pHDRExtOutput = NULL;
        param.shot.pRaw2DExtOutput = NULL;
        param.shot.pStatistics = NULL;
        param.shot.pDPFMap = NULL;
        param.shot.pENSOutput = NULL;
        //param.shot.ui32LinkedListTS = pShot->userShot.ui32LinkedListTS;
        //param.shot.ui32InterruptTS = pShot->userShot.ui32InterruptTS;

        if (pShot->phwEncoderOutput)
        {
            param.shot.encId = pShot->phwEncoderOutput->ID;
            param.shot.bEncTiled = pShot->phwEncoderOutput->bTiled;
#ifdef INFOTM_ISP
            param.shot.pEncoderOutputPhyAddr = pShot->phwEncoderOutput->sMemory.pPhysAddress;
#endif //INFOTM_ISP
        }
        if (pShot->phwDisplayOutput)
        {
            param.shot.dispId = pShot->phwDisplayOutput->ID;
            param.shot.bDispTiled = pShot->phwDisplayOutput->bTiled;
#ifdef INFOTM_ISP
            param.shot.pDisplayOutputPhyAddr = pShot->phwDisplayOutput->sMemory.pPhysAddress;
#endif //INFOTM_ISP
        }
        if (pShot->phwHDRExtOutput)
        {
            param.shot.HDRExtId = pShot->phwHDRExtOutput->ID;
            param.shot.bHDRExtTiled = pShot->phwHDRExtOutput->bTiled;
#ifdef INFOTM_ISP
            param.shot.pHDRExtOutputPhyAddr = pShot->phwHDRExtOutput->sMemory.pPhysAddress;
#endif //INFOTM_ISP
        }
        if (pShot->phwRaw2DExtOutput)
        {
            param.shot.raw2DExtId = pShot->phwRaw2DExtOutput->ID;
#ifdef INFOTM_ISP
            param.shot.pRaw2DExtOutputPhyAddr = pShot->phwRaw2DExtOutput->sMemory.pPhysAddress;
#endif //INFOTM_ISP
        }

        // will updates sizes and all according to tiling
        //KRN_CI_BufferFromConfig(&(param.shot), pKrnCapture);

        // propagate to user-space
        //param.shot.i32MissedFrames = pShot->userShot.i32MissedFrames;

        ret = copy_to_user(pUserParam, &param, sizeof(struct CI_BUFFER_PARAM));
        if (ret)
        {
            /// @ release buffer?
            CI_FATAL("Failed to copy_to_user the capture buffer parameters\n");
            ret = IMG_ERROR_INVALID_PARAMETERS;
        }
    }
    else
    {
        CI_DEBUG("KRN_CI_PipelineAcquireShot failed...");
    }

#if defined(W_TIME_PRINT)
    getnstimeofday(&t3);

    W_TIME_PRINT(t2, t3, "copy_to_user");

    W_TIME_PRINT(t0, t3, "=");
#endif
    return ret;
}

IMG_RESULT INT_CI_PipelineReleaseShot(KRN_CI_CONNECTION *pConnection,
    const struct CI_BUFFER_TRIGG __user *pUserParam)
{
    KRN_CI_PIPELINE *pKrnCapture = NULL;
    sCell_T *pFound = NULL;
    struct CI_BUFFER_TRIGG param;
    IMG_RESULT ret;

    IMG_ASSERT(pConnection != NULL);

    ret = copy_from_user(&param, pUserParam, sizeof(struct CI_BUFFER_TRIGG));
    if (ret)
    {
        CI_FATAL("Failed to copy_from_user the capture buffer parameters\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pFound = FindPipeline(pConnection, param.captureId);
    if (!pFound)
    {
        CI_FATAL("Capture not registered\n");
        return IMG_ERROR_FATAL;
    }

    pKrnCapture = (KRN_CI_PIPELINE*)pFound->object;

    // search for the shot and release it
    ret = KRN_CI_PipelineRelaseShot(pKrnCapture, param.shotId);
    return ret;
}

IMG_RESULT INT_CI_PipelineUpdate(KRN_CI_CONNECTION *pConnection,
struct CI_PIPE_PARAM __user *pUserParam)
{
    KRN_CI_PIPELINE *pKrnCapture = NULL;
    CI_PIPELINE *pBackup = NULL;
    sCell_T *pFound = NULL;
    IMG_RESULT ret = IMG_ERROR_INVALID_PARAMETERS;
    int r = 0;
    IMG_BOOL8 bUpdateReg = IMG_FALSE;
    IMG_BOOL8 bFullUpdate = IMG_FALSE;
    struct CI_PIPE_PARAM *pParam = NULL;

    IMG_ASSERT(pConnection != NULL);

    // because pParam may be too big for the stack
    pParam = (struct CI_PIPE_PARAM*)IMG_MALLOC(sizeof(struct CI_PIPE_PARAM));
    if (!pParam)
    {
        CI_FATAL("Failed to allocate copy capture's parameter!\n");
        return IMG_ERROR_MALLOC_FAILED;
    }

    r = copy_from_user(pParam, pUserParam, sizeof(struct CI_PIPE_PARAM));
    if (r)
    {
        CI_FATAL("Failed to copy_from_user capture's parameter\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pFound = FindPipeline(pConnection, pParam->identifier);
    if (!pFound)
    {
        CI_FATAL("Capture not registered\n");
        return IMG_ERROR_FATAL;
    }

    pKrnCapture = (KRN_CI_PIPELINE*)pFound->object;
    IMG_ASSERT(pKrnCapture != NULL);

    pBackup = (CI_PIPELINE*)IMG_MALLOC(sizeof(CI_PIPELINE));
    if (!pBackup)
    {
        CI_FATAL("failed to allocate backup configuration\n");
        IMG_FREE(pParam);
        return IMG_ERROR_MALLOC_FAILED;
    }
    // backup
    IMG_MEMCPY(pBackup, &(pKrnCapture->userPipeline), sizeof(CI_PIPELINE));

    bFullUpdate = IMG_FALSE;
    if (!pKrnCapture->bStarted)
    {
        /* check if any buffers were allocated at all to know if we can do
         * a full copy or copy module by module */
        /* as the pipeline is stopped there can be no buffers in pending and
         * processed */
        SYS_LockAcquire(&(pKrnCapture->sListLock));
        {
            /* needs no allocated buffers and no Shots in user-space that
             * could hide buffers */
            if (0 == pKrnCapture->sList_availableBuffers.ui32Elements
                && 0 == pKrnCapture->sList_sent.ui32Elements)
            {
                bFullUpdate = IMG_TRUE;
            }
        }
        SYS_LockRelease(&(pKrnCapture->sListLock));
    }

    /*
     * then perform the copy, either full copy if not started and no buffers
     * were allocated yet or module by module
     */
    if (bFullUpdate)
    {
        IMG_BOOL8 bVerif;
        IMG_UINT32 width =
            (pKrnCapture->userPipeline.sImagerInterface.ui16ImagerSize[1] + 1)
            * CI_CFA_WIDTH; // in pixels

        bVerif = KRN_CI_PipelineVerify(&(pParam->sUserPipeline),
            &(g_psCIDriver->sHWInfo));
        if (!bVerif)
        {
            CI_FATAL("Given configuration is not valid for this "\
                "HW version!\n");
            return IMG_ERROR_NOT_SUPPORTED;
        }
        IMG_MEMCPY(&(pKrnCapture->userPipeline), &(pParam->sUserPipeline),
            sizeof(CI_PIPELINE));

        pKrnCapture->uiENSSizeNeeded = HW_CI_EnsOutputSize(
            pKrnCapture->userPipeline.sEncStats.ui8Log2NCouples,
            width);

        /*pKrnCapture->sSizes.bDispTiled = IMG_FALSE;
        pKrnCapture->sSizes.bEncTiled = IMG_FALSE;
        pKrnCapture->sSizes.bHDRExtTiled = IMG_FALSE;
        ret = KRN_CI_BufferFromConfig(&(pKrnCapture->sSizes), pKrnCapture);
        if (ret)
        {
        CI_FATAL("could not compute minimal buffer sizes\n");
        return IMG_ERROR_NOT_SUPPORTED;
        }

        if (pKrnCapture->userPipeline.bSupportTiling)
        {
        pKrnCapture->sTiledSize.bDispTiled = IMG_TRUE;
        pKrnCapture->sTiledSize.bEncTiled = IMG_TRUE;
        pKrnCapture->sTiledSize.bHDRExtTiled = IMG_TRUE;
        ret = KRN_CI_BufferFromConfig(&(pKrnCapture->sTiledSize), pKrnCapture);
        if (ret)
        {
        CI_FATAL("could not compute tiled minimal buffer sizes\n");
        return IMG_ERROR_NOT_SUPPORTED;
        }
        }*/
    }
    else
    {
        /*
         * first check for the modules that need access to registers
         */
        if ((pParam->eUpdateMask & CI_UPD_REG) != 0)
        {
            ret = IMG_SUCCESS;
            bUpdateReg = IMG_TRUE;
            // will allow update if this block does not return
#ifndef INFOTM_USE_SPIN_LOCK // added by linyun.xiong @2016-03-22
            SYS_LockAcquire(&(pKrnCapture->sListLock));
#else
			SYS_SpinlockAcquire(&(pKrnCapture->sProcessingSpinlock));
#endif
            {
                if (pKrnCapture->sList_pending.ui32Elements > 0)
                {
                    ret = IMG_ERROR_NOT_SUPPORTED;
                }
            }
#ifndef INFOTM_USE_SPIN_LOCK // added by linyun.xiong @2016-03-22
            SYS_LockRelease(&(pKrnCapture->sListLock));
#else
			SYS_SpinlockRelease(&(pKrnCapture->sProcessingSpinlock));
#endif
            if (IMG_SUCCESS != ret)
            {
                int r;
                CI_WARNING("trying to update configuration that needs "\
                    "register access while having pending buffers\n");
                pParam->eUpdateMask = pParam->eUpdateMask & CI_UPD_REG;
                // to propagate the error
                r = copy_to_user(pUserParam, pParam,
                    sizeof(struct CI_PIPE_PARAM));
                if (r)
                {
                    CI_FATAL("failed to propagate the error to userspace!\n");
                }
                IMG_FREE(pParam);
                IMG_FREE(pBackup);

                return ret;
            }
        }

        // copy the configuration ID
        pKrnCapture->userPipeline.ui8PrivateValue =
            pParam->sUserPipeline.ui8PrivateValue;

        // update module by module
        if ((pParam->eUpdateMask & CI_UPD_BLC) != 0)
        {
            IMG_MEMCPY(&(pKrnCapture->userPipeline.sBlackCorrection),
                &(pParam->sUserPipeline.sBlackCorrection),
                sizeof(CI_MODULE_BLC));
        }

        if ((pParam->eUpdateMask & CI_UPD_RLT) != 0)
        {
            IMG_MEMCPY(&(pKrnCapture->userPipeline.sRawLUT),
                &(pParam->sUserPipeline.sRawLUT),
                sizeof(CI_MODULE_RLT));
        }

        if ((pParam->eUpdateMask & CI_UPD_LSH) != 0)
        {
            IMG_MEMCPY(&(pKrnCapture->userPipeline.sDeshading),
                &(pParam->sUserPipeline.sDeshading),
                sizeof(CI_MODULE_LSH));
        }

        if ((pParam->eUpdateMask & CI_UPD_WBC) != 0)
        {
            IMG_MEMCPY(&(pKrnCapture->userPipeline.sWhiteBalance),
                &(pParam->sUserPipeline.sWhiteBalance),
                sizeof(CI_MODULE_WBC));
        }

        if ((pParam->eUpdateMask & CI_UPD_DNS) != 0)
        {
            IMG_MEMCPY(&(pKrnCapture->userPipeline.sDenoiser),
                &(pParam->sUserPipeline.sDenoiser),
                sizeof(CI_MODULE_DNS));
        }

        if ((pParam->eUpdateMask & (CI_UPD_DPF | CI_UPD_DPF_INPUT)) != 0)
        {
            IMG_MEMCPY(&(pKrnCapture->userPipeline.sDefectivePixels),
                &(pParam->sUserPipeline.sDefectivePixels),
                sizeof(CI_MODULE_DPF));
        }

        if ((pParam->eUpdateMask & CI_UPD_LCA) != 0)
        {
            IMG_MEMCPY(&(pKrnCapture->userPipeline.sChromaAberration),
                &(pParam->sUserPipeline.sChromaAberration),
                sizeof(CI_MODULE_LCA));
        }

        if ((pParam->eUpdateMask & CI_UPD_CCM) != 0)
        {
            IMG_MEMCPY(&(pKrnCapture->userPipeline.sColourCorrection),
                &(pParam->sUserPipeline.sColourCorrection),
                sizeof(CI_MODULE_CCM));
        }

        if ((pParam->eUpdateMask & CI_UPD_MGM) != 0)
        {
            IMG_MEMCPY(&(pKrnCapture->userPipeline.sMainGamutMapper),
                &(pParam->sUserPipeline.sMainGamutMapper),
                sizeof(CI_MODULE_MGM));
        }

        if ((pParam->eUpdateMask & CI_UPD_GMA) != 0)
        {
            IMG_MEMCPY(&(pKrnCapture->userPipeline.sGammaCorrection),
                &(pParam->sUserPipeline.sGammaCorrection),
                sizeof(CI_MODULE_GMA));
        }

        if ((pParam->eUpdateMask & CI_UPD_R2Y) != 0)
        {
            IMG_MEMCPY(&(pKrnCapture->userPipeline.sRGBToYCC),
                &(pParam->sUserPipeline.sRGBToYCC),
                sizeof(CI_MODULE_R2Y));
        }

        if ((pParam->eUpdateMask & CI_UPD_MIE) != 0)
        {
            IMG_MEMCPY(&(pKrnCapture->userPipeline.sImageEnhancer),
                &(pParam->sUserPipeline.sImageEnhancer),
                sizeof(CI_MODULE_MIE));
        }

        if ((pParam->eUpdateMask & CI_UPD_TNM) != 0)
        {
            IMG_MEMCPY(&(pKrnCapture->userPipeline.sToneMapping),
                &(pParam->sUserPipeline.sToneMapping),
                sizeof(CI_MODULE_TNM));
        }

        if ((pParam->eUpdateMask & CI_UPD_SHA) != 0)
        {
            IMG_MEMCPY(&(pKrnCapture->userPipeline.sSharpening),
                &(pParam->sUserPipeline.sSharpening),
                sizeof(CI_MODULE_SHA));
        }

        if ((pParam->eUpdateMask & CI_UPD_ESC) != 0)
        {
            IMG_MEMCPY(&(pKrnCapture->userPipeline.sEncoderScaler),
                &(pParam->sUserPipeline.sEncoderScaler),
                sizeof(CI_MODULE_ESC));
        }

        if ((pParam->eUpdateMask & CI_UPD_DSC) != 0)
        {
            IMG_MEMCPY(&(pKrnCapture->userPipeline.sDisplayScaler),
                &(pParam->sUserPipeline.sDisplayScaler),
                sizeof(CI_MODULE_DSC));
        }

        if ((pParam->eUpdateMask & CI_UPD_Y2R) != 0)
        {
            IMG_MEMCPY(&(pKrnCapture->userPipeline.sYCCToRGB),
                &(pParam->sUserPipeline.sYCCToRGB),
                sizeof(CI_MODULE_Y2R));
        }

        if ((pParam->eUpdateMask & CI_UPD_DGM) != 0)
        {
            IMG_MEMCPY(&(pKrnCapture->userPipeline.sDisplayGamutMapper),
                &(pParam->sUserPipeline.sDisplayGamutMapper),
                sizeof(CI_MODULE_DGM));
        }

        if ((pParam->eUpdateMask & CI_UPD_EXS) != 0)
        {
            IMG_MEMCPY(&(pKrnCapture->userPipeline.sExposureStats),
                &(pParam->sUserPipeline.sExposureStats),
                sizeof(CI_MODULE_EXS));
        }

        if ((pParam->eUpdateMask & CI_UPD_FOS) != 0)
        {
            IMG_MEMCPY(&(pKrnCapture->userPipeline.sFocusStats),
                &(pParam->sUserPipeline.sFocusStats),
                sizeof(CI_MODULE_FOS));
        }

        if ((pParam->eUpdateMask & CI_UPD_WBS) != 0)
        {
            IMG_MEMCPY(&(pKrnCapture->userPipeline.sWhiteBalanceStats),
                &(pParam->sUserPipeline.sWhiteBalanceStats),
                sizeof(CI_MODULE_WBS));
        }

        if ((pParam->eUpdateMask & CI_UPD_HIS) != 0)
        {
            IMG_MEMCPY(&(pKrnCapture->userPipeline.sHistStats),
                &(pParam->sUserPipeline.sHistStats),
                sizeof(CI_MODULE_HIS));
        }

        if ((pParam->eUpdateMask & CI_UPD_FLD) != 0)
        {
            IMG_MEMCPY(&(pKrnCapture->userPipeline.sFlickerStats),
                &(pParam->sUserPipeline.sFlickerStats),
                sizeof(CI_MODULE_FLD));
        }

        /*if ( (pParam->eUpdateMask & CI_UPD_ENS) != 0 )
        {
        IMG_MEMCPY(&(pKrnCapture->userPipeline.sEncStats),
        &(pParam->sUserPipeline.sEncStats),
        sizeof(CI_MODULE_ENS));
        pKrnCapture->uiENSSizeNeeded = HW_CI_EnsOutputSize(
        pKrnCapture->userPipeline.sEncStats.ui8Log2NCouples,
        (pKrnCapture->userPipeline.sImagerInterface.ui16ImagerSize[1]+1)*2);
        }*/

        // statistics desired output may have changed
        pKrnCapture->userPipeline.eOutputConfig =
            pParam->sUserPipeline.eOutputConfig;
    } // update

    /* if the capture is not started the device memory and registers are
     * not updated - they will be updated when the capture starts */
    if (pKrnCapture->bStarted)
    {
        // the register updates were not done
        pParam->eUpdateMask &= ~(CI_UPD_REG);
        /* just to make sure we don't have left-overs - because
         * INT_CI_PipelineSetDPFRead() will not be called */
        pKrnCapture->userPipeline.sDefectivePixels.sInput.apDefectInput =
            NULL;

        ret = KRN_CI_PipelineUpdate(pKrnCapture, bUpdateReg,
            pParam->bUpdateASAP);
        if (IMG_SUCCESS != ret)
        {
            CI_FATAL("failed to update the HW configuration\n");
            pParam->eUpdateMask = CI_UPD_ALL;
            goto pipeupdate_failed; // returns ret
        }
    }
    else
    {
        int i;

        if ((pParam->eUpdateMask & CI_UPD_LSH) != 0)
        {
            /* but even if not started the deshading grid is updated if need
             * be (because it needs copy_from_user to be done ASAP) */
            if (pKrnCapture->userPipeline.sDeshading.bUseDeshadingGrid)
            {
                ret = INT_CI_PipelineSetDeshading(pKrnCapture);
                if (IMG_SUCCESS != ret)
                {
                    CI_FATAL("failed to convert the deshading matrix - "\
                        "returned %d\n", ret);
                    pParam->eUpdateMask = CI_UPD_LSH;
                    goto pipeupdate_failed; // returns ret;
                }
            }
        }
        for (i = 0; i < LSH_GRADS_NO; i++)
        {
            pKrnCapture->userPipeline.sDeshading.matrixDiff[i] = NULL;
            pKrnCapture->userPipeline.sDeshading.matrixStart[i] = NULL;
        }

        if ((pParam->eUpdateMask & CI_UPD_DPF_INPUT) != 0)
        {
            /** @ a new method of updates for DPF should be found to
             * avoid having to do multi-level copy_from_user */
            ret = INT_CI_PipelineSetDPFRead(pKrnCapture,
                pKrnCapture->userPipeline.sDefectivePixels.sInput.\
                apDefectInput);
            if (IMG_SUCCESS != ret)
            {
                CI_FATAL("failed to convert the DPF Read map - "\
                    "returned %d\n", ret);
                pParam->eUpdateMask = CI_UPD_DPF_INPUT;
                goto pipeupdate_failed; // returns ret;
            }
        }
        pKrnCapture->userPipeline.sDefectivePixels.sInput.apDefectInput = NULL;
    }
    pKrnCapture->eLastUpdated |= pParam->eUpdateMask; // add up the flags

    IMG_FREE(pParam);
    IMG_FREE(pBackup);
    return IMG_SUCCESS;

pipeupdate_failed:
    // rollback using backup
    IMG_MEMCPY(&(pKrnCapture->userPipeline), pBackup, sizeof(CI_PIPELINE));
    // do not apply ASAP, it is just to rollback the device memory
    r = KRN_CI_PipelineUpdate(pKrnCapture, bUpdateReg, IMG_FALSE);
    if (IMG_SUCCESS != r)
    {
        CI_FATAL("failed to re-apply the rolled-back configuration!\n");
    }
    IMG_FREE(pParam);
    IMG_FREE(pBackup);
    r = copy_to_user(pUserParam, pParam, sizeof(struct CI_PIPE_PARAM));
    if (r)
    {
        CI_FATAL("failed to propagate the error to userspace!\n");
    }
    /// @ wouldn't it be simpler to change the flag to CI_UPD_NONE?
    return ret;
}

IMG_RESULT INT_CI_PipelineCreateBuffer(KRN_CI_CONNECTION *pConnection,
struct CI_ALLOC_PARAM __user *pUserParam)
{
    IMG_RESULT ret;
    struct CI_ALLOC_PARAM param;
    sCell_T *pFound = NULL;
    KRN_CI_PIPELINE *pKrnConfig = NULL;
    KRN_CI_BUFFER *pBuffer = NULL;

    IMG_ASSERT(pConnection != NULL);

    if (!pUserParam)
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    ret = copy_from_user(&param, pUserParam, sizeof(struct CI_ALLOC_PARAM));
    if (ret)
    {
        CI_FATAL("Failed to copy_from_user Alloc params\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pFound = FindPipeline(pConnection, param.configId);
    if (!pFound)
    {
        CI_FATAL("configuration %d is not registered\n", param.configId);
        return IMG_ERROR_FATAL;
    }
    pKrnConfig = (KRN_CI_PIPELINE*)pFound->object;

    pBuffer = KRN_CI_PipelineCreateBuffer(pKrnConfig, &param, &ret);

    if (!pBuffer)
    {
        CI_FATAL("Failed to allocate the buffer!\n");
        return ret;
    }

    ret = KRN_CI_PipelineAddBuffer(pKrnConfig, pBuffer);
    if (IMG_SUCCESS == ret)
    {
#ifdef INFOTM_ISP
    	char szName[20];
    	switch (pBuffer->type)
    	{
    		case CI_BUFF_ENC:
    			sprintf(szName, "Enc(%d)", pBuffer->ID);
    			break;
    		case CI_BUFF_DISP:
    			sprintf(szName, "Disp(%d)", pBuffer->ID);
    			break;
    		case CI_BUFF_HDR_WR:
    			sprintf(szName, "Hdr(%d)", pBuffer->ID);
    			break;
    		case CI_BUFF_RAW2D:
    			sprintf(szName, "Raw2d(%d)", pBuffer->ID);
    			break;
    		case CI_BUFF_STAT:
    			sprintf(szName, "Stat(%d)", pBuffer->ID);
    			break;
    		case CI_BUFF_DPF_WR:
    			sprintf(szName, "Dpf(%d)", pBuffer->ID);
    			break;
    		case CI_BUFF_ENS_OUT:
    			sprintf(szName, "Ens(%d)", pBuffer->ID);
    			break;

    	}

    	IMG_STRNCPY(pBuffer->sMemory.szMemName, szName, 19);
#ifdef INFOTM_ENABLE_ISP_DEBUG
        printk("@@@    AddBuf[%d]: name=%s, Type=%d, ID=%d, VirtAddr=0x%p, PhysAddr=0x%p\n",
                    pKrnConfig->userPipeline.ui8Context,
                    pBuffer->sMemory.szMemName,
                    pBuffer->type,
                    pBuffer->ID,
                    pBuffer->sMemory.pVirtualAddress,
                    pBuffer->sMemory.pPhysAddress);
#endif //INFOTM_ENABLE_ISP_DEBUG
#endif //INFOTM_ISP

        param.uiMemMapId = pBuffer->ID;
        ret = copy_to_user(pUserParam, &param, sizeof(struct CI_ALLOC_PARAM));
        if (ret)
        {
            CI_FATAL("Failed to copy_to_user Alloc params\n");
            return IMG_ERROR_INVALID_PARAMETERS;
        }
    }

    return ret;
}

IMG_RESULT INT_CI_PipelineDeregBuffer(KRN_CI_CONNECTION *pConnection,
struct CI_ALLOC_PARAM __user *pUserParam)
{
    sCell_T *pCell = NULL;
    IMG_RESULT ret = IMG_SUCCESS;
    KRN_CI_PIPELINE *pKrnConfig = NULL;
    struct CI_ALLOC_PARAM param;

    IMG_ASSERT(pConnection != NULL);

    if (!pUserParam)
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    ret = copy_from_user(&param, pUserParam, sizeof(struct CI_ALLOC_PARAM));
    if (ret)
    {
        CI_FATAL("Failed to copy_from_user buffer fds\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pCell = FindPipeline(pConnection, param.configId);
    if (!pCell)
    {
        CI_FATAL("configuration not found in the driver's list\n");
        return IMG_ERROR_FATAL;
    }

    pKrnConfig = (KRN_CI_PIPELINE*)pCell->object;
    IMG_ASSERT(pKrnConfig != NULL);

    ret = KRN_CI_PipelineDeregisterBuffer(pKrnConfig, param.uiMemMapId);
    if (IMG_SUCCESS != ret)
    {
        CI_FATAL("failed to deregister the buffer %d\n", param.uiMemMapId);
    }

    return ret;
}

IMG_RESULT INT_CI_PipelineSetDeshading(KRN_CI_PIPELINE *pPipeline)
{
    IMG_RESULT ret;
    IMG_SIZE uiStride; // distance between 2 channels in bytes
    /* size of a channel in bytes - must be a multiple of 16 Bytes when
     * written to HW */
    IMG_SIZE uiLineSize;
    IMG_SIZE uiAllocation;
    IMG_UINT8 c;
    IMG_INT16 *matrixDiff[LSH_GRADS_NO];
    IMG_UINT16 *matrixStart[LSH_GRADS_NO];
    IMG_SIZE matrixDiffSize =
        (pPipeline->userPipeline.sDeshading.ui16Width - 1)
        *(pPipeline->userPipeline.sDeshading.ui16Height)
        *sizeof(IMG_UINT16);
    IMG_SIZE matrixStartSize =
        (pPipeline->userPipeline.sDeshading.ui16Height)
        *sizeof(IMG_UINT16);
    IMG_BOOL8 bValidSize = IMG_FALSE;
    /* the separation is in HW [line][channel][pixel] while in
     * CI [channel][line*pixel]*/

    pPipeline->ui16LSHPixelSize = pPipeline->userPipeline.sDeshading.ui16Width
        *(1 << pPipeline->userPipeline.sDeshading.ui8TileSizeLog2)
        * CI_CFA_WIDTH;

    bValidSize = KRN_CI_DriverCheckDeshading(pPipeline);

    if (!bValidSize)
    {
        CI_FATAL("the deshading grid size %d does not fit in current "\
            "context limitations\n");
        pPipeline->ui16LSHPixelSize = 0;
        return IMG_ERROR_NOT_SUPPORTED;
    }

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "start");

    for (c = 0; c < LSH_GRADS_NO; c++)
    {
        matrixDiff[c] = NULL;
    }
    for (c = 0; c < LSH_GRADS_NO; c++)
    {
        if (matrixDiffSize > 2 * PAGE_SIZE)
        {
            matrixDiff[c] = (IMG_INT16*)IMG_BIGALLOC(matrixDiffSize);
        }
        else
        {
            matrixDiff[c] = (IMG_INT16*)IMG_MALLOC(matrixDiffSize);
        }
        if (matrixStartSize > 2 * PAGE_SIZE)
        {
            matrixStart[c] = (IMG_UINT16*)IMG_BIGALLOC(matrixStartSize);
        }
        else
        {
            matrixStart[c] = (IMG_UINT16*)IMG_MALLOC(matrixStartSize);
        }

        if (matrixDiff[c] == NULL || matrixStart[c] == NULL)
        {
            CI_FATAL("failed to allocate copy of the LSH matrix\n");
            ret = IMG_ERROR_MALLOC_FAILED;
            goto setdeshading_clean;
        }

        /** @ a new method of updates for LSH should be found to avoid
         * having to do multi-level copy_from_user */
        ret = copy_from_user(matrixDiff[c],
            pPipeline->userPipeline.sDeshading.matrixDiff[c],
            matrixDiffSize);
        if (ret)
        {
            CI_FATAL("failed to copy from user the LSH matrix diff\n");
            ret = IMG_ERROR_INVALID_PARAMETERS;
            goto setdeshading_clean;
        }
        /** @ a new method of updates for LSH should be found to avoid
         * having to do multi-level copy_from_user */
        ret = copy_from_user(matrixStart[c],
            pPipeline->userPipeline.sDeshading.matrixStart[c],
            matrixStartSize);
        if (ret)
        {
            CI_FATAL("failed to copy from user the LSH matrix start\n");
            ret = IMG_ERROR_INVALID_PARAMETERS;
            goto setdeshading_clean;
        }

        /* once copied make the kernel pointer NULL because it should
         * not be used */
        pPipeline->userPipeline.sDeshading.matrixDiff[c] = NULL;
        pPipeline->userPipeline.sDeshading.matrixStart[c] = NULL;
    }

    // needed bits is: 16 bits + [(width-1)*bitsPerDiff]
    // need rounding [A] to a byte and the whole result to 16 Bytes
    uiLineSize = (2 + ((pPipeline->userPipeline.sDeshading.ui16Width - 1)
        *pPipeline->userPipeline.sDeshading.ui8BitsPerDiff + 7) / 8);
    uiLineSize = ((uiLineSize + 15) / 16) * 16;

    uiStride = ((uiLineSize + SYSMEM_ALIGNMENT - 1) / SYSMEM_ALIGNMENT)
        *SYSMEM_ALIGNMENT;

    uiAllocation = uiStride*pPipeline->userPipeline.sDeshading.ui16Height
        *LSH_GRADS_NO;

    // remove the previous matrix
    if (uiAllocation > pPipeline->sDeshadingGrid.uiAllocated)
    {
        if (pPipeline->sDeshadingGrid.uiAllocated > 0)
        {
            ret = SYS_MemFree(&(pPipeline->sDeshadingGrid));
            if (IMG_SUCCESS != ret)
            {
                CI_FATAL("failed to free the exisiting LSH matrix\n");
                goto setdeshading_clean;
            }
        }

#ifdef INFOTM_ISP
        ret = SYS_MemAlloc(&(pPipeline->sDeshadingGrid), uiAllocation,
            pPipeline->apHeap[CI_DATA_HEAP], 0, "DeshadingGrid");
#else
        ret = SYS_MemAlloc(&(pPipeline->sDeshadingGrid), uiAllocation,
            pPipeline->apHeap[CI_DATA_HEAP], 0);
#endif //INFOTM_ISP
        if (IMG_SUCCESS != ret)
        {
            CI_FATAL("failed to allocate the new LSH matrix\n");
            goto setdeshading_clean;
        }
    }

    // in multiple of 16 bytes -1
    pPipeline->ui16LSHLineSize = (uiLineSize / 16) - 1;
    pPipeline->uiLSHMatrixStride = uiStride;
    //pPipeline->ui16LSHPixelSize set before checking for size

    // conversion of the matrix
    {
        IMG_UINT32 mask =
            ((1 << pPipeline->userPipeline.sDeshading.ui8BitsPerDiff) - 1);
        IMG_UINT16 y, x;
        IMG_UINT8 *pCurr; // current position in memory - the device memory
        IMG_UINT8 *memory = NULL; // the device memory

        memory = (IMG_UINT8 *)Platform_MemGetMemory(
            &(pPipeline->sDeshadingGrid));
        IMG_MEMSET(memory, 0, uiAllocation);

        for (y = 0; y < pPipeline->userPipeline.sDeshading.ui16Height; y++)
        {
            for (c = 0; c < LSH_GRADS_NO; c++)
            {
                IMG_INT32 packedData = 0;
                IMG_INT32 packedNbits = 0;

                pCurr = &(memory[(y*LSH_GRADS_NO + c)*uiStride]);

                // first data is full range - 16b
                *(IMG_UINT16*)pCurr = matrixStart[c][y];
                pCurr += sizeof(IMG_UINT16); // next element

                for (x = 0; x
                    < pPipeline->userPipeline.sDeshading.ui16Width - 1; x++)
                {
                    int off = y*
                        (pPipeline->userPipeline.sDeshading.ui16Width - 1)
                        + x;
                    packedData |= (matrixDiff[c][off] & mask) << packedNbits;
                    packedNbits +=
                        pPipeline->userPipeline.sDeshading.ui8BitsPerDiff;

                    /* either enough data or it's the last bits and we need
                     * to flush */
                    while (packedNbits >= 8
                        || (x >= (pPipeline->userPipeline.sDeshading.ui16Width - 1)
                        && packedNbits > 0))
                    {
                        *pCurr = (packedData & 0xFF);
                        pCurr++;
                        /* if it is the last element (i.e. if 2nd condition
                         * of the while */
                        packedData >>= 8;
                        packedNbits -= 8; // then these steps are useless
                    } // while enough data
                } // for column
            } // for channel
        } // for line

        CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle,
            "update after transformation");
        Platform_MemUpdateDevice(&(pPipeline->sDeshadingGrid));
    }

    ret = IMG_SUCCESS;
setdeshading_clean:
    for (c = 0; c < LSH_GRADS_NO; c++)
    {
        if (matrixDiff[c] != NULL)
        {
            if (matrixDiffSize > 2 * PAGE_SIZE)
            {
                IMG_BIGFREE(matrixDiff[c]);
                matrixDiff[c] = NULL;
            }
            else
            {
                IMG_FREE(matrixDiff[c]);
                matrixDiff[c] = NULL;
            }
        }
        if (matrixStart[c] != NULL)
        {
            if (matrixStartSize > 2 * PAGE_SIZE)
            {
                IMG_BIGFREE(matrixStart[c]);
                matrixStart[c] = NULL;
            }
            else
            {
                IMG_FREE(matrixStart[c]);
                matrixStart[c] = NULL;
            }
        }
        pPipeline->userPipeline.sDeshading.matrixDiff[c] = NULL;
        pPipeline->userPipeline.sDeshading.matrixStart[c] = NULL;
    }

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "done");

    return ret;
}

IMG_RESULT INT_CI_PipelineSetDPFRead(KRN_CI_PIPELINE *pPipeline,
    IMG_UINT16 __user *pUserDPF)
{
    IMG_RESULT ret = IMG_SUCCESS;

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "start");

    // this pointer should never be used in kernel-side!
    pPipeline->userPipeline.sDefectivePixels.sInput.apDefectInput = NULL;
    if (pPipeline->userPipeline.sDefectivePixels.sInput.ui32NDefect > 0)
    {
        IMG_UINT16 *apDpfInput = NULL;
        IMG_SIZE inputSize =
            2 * pPipeline->userPipeline.sDefectivePixels.sInput.ui32NDefect
            *sizeof(IMG_UINT16);

        // the internal buffer space is reserved when starting the context
        pPipeline->uiWantedDPFBuffer = pPipeline->userPipeline.\
            sDefectivePixels.sInput.ui16InternalBufSize;

        if (inputSize < 2 * PAGE_SIZE)
        {
            apDpfInput = (IMG_UINT16*)IMG_MALLOC(inputSize);
        }
        else
        {
            /* happens if there are 2048 dead pixels in the map...
             * in 4k sensor it's 0.01%, 720p is 0.2% (so change is high) */
            apDpfInput = (IMG_UINT16*)IMG_BIGALLOC(inputSize);
        }

        if (!apDpfInput)
        {
            CI_FATAL("failed to allocate the DPF input tmp buffer\n");
            return IMG_ERROR_MALLOC_FAILED;
        }

        ret = copy_from_user(apDpfInput, pUserDPF, inputSize);
        if (ret)
        {
            CI_FATAL("failed to copy DPF input map from user\n");
            ret = IMG_ERROR_INVALID_PARAMETERS;
        }
        else
        {
            ret = HW_CI_DPF_ReadMapConvert(pPipeline, apDpfInput,
                pPipeline->userPipeline.sDefectivePixels.sInput.ui32NDefect);
            if (IMG_SUCCESS != ret)
            {
                CI_FATAL("failed to convert the DPF input map - "\
                    "returned %d\n", ret);
            }
        }

        if (apDpfInput)
        {
            if (inputSize < 2 * PAGE_SIZE)
            {
                IMG_FREE(apDpfInput);
            }
            else
            {
                IMG_BIGFREE(apDpfInput);
            }
            apDpfInput = NULL;
        }
    }

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "done");

    return ret;
}

IMG_RESULT INT_CI_PipelineHasAvailable(KRN_CI_CONNECTION *pConnection,
struct CI_HAS_AVAIL __user *pParam)
{
    KRN_CI_PIPELINE *pKrnConfig = NULL;
    sCell_T *pFound = NULL;
    struct CI_HAS_AVAIL sParam;
    int ret;

    IMG_ASSERT(pConnection != NULL);

    ret = copy_from_user(&sParam, pParam, sizeof(struct CI_HAS_AVAIL));
    if (ret)
    {
        CI_FATAL("Failed to copy_from_user\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pFound = FindPipeline(pConnection, sParam.captureId);
    if (!pFound)
    {
        CI_FATAL("configuration is not registered\n");
        return IMG_ERROR_FATAL;
    }

    pKrnConfig = (KRN_CI_PIPELINE*)pFound->object;

    SYS_LockAcquire(&(pKrnConfig->sListLock));
    {
        struct count_status srch;
        srch.eLookingFor = CI_SHOT_AVAILABLE;
        srch.result = 0;

        List_visitor(&(pKrnConfig->sList_available), &srch,
            &List_FindShotWithStatus);
        sParam.uiShots = srch.result;
        sParam.uiBuffers = pKrnConfig->sList_availableBuffers.ui32Elements;
    }
    SYS_LockRelease(&(pKrnConfig->sListLock));

    ret = copy_to_user(pParam, &sParam, sizeof(struct CI_HAS_AVAIL));
    if (ret)
    {
        CI_FATAL("Failed to copy_to_user\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    return IMG_SUCCESS;
}

IMG_RESULT INT_CI_PipelineAddShot(KRN_CI_CONNECTION *pConnection,
struct CI_POOL_PARAM __user *userParam)
{
    KRN_CI_PIPELINE *pKrnConfig = NULL;
    sCell_T *pFound = NULL;
    IMG_RESULT ret = IMG_SUCCESS;
    KRN_CI_SHOT *pNew = NULL;
    struct CI_POOL_PARAM param;

    IMG_ASSERT(pConnection != NULL);

    if (!userParam) // test in advance to avoid cleaning
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    ret = copy_from_user(&param, userParam, sizeof(struct CI_POOL_PARAM));
    if (ret)
    {
        CI_FATAL("Failed to copy_from_user a config add buffer "\
            "parameter struct\n");
        return IMG_ERROR_FATAL;
    }

    pFound = FindPipeline(pConnection, param.configId);
    if (!pFound)
    {
        CI_FATAL("configuration %d is not registered\n", param.configId);
        return IMG_ERROR_FATAL;
    }

    pKrnConfig = (KRN_CI_PIPELINE*)pFound->object;

    if (pKrnConfig->bStarted)
    {
        CI_FATAL("cannot create new shots while capture is started\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    pNew = KRN_CI_ShotCreate(&ret);
    if (!pNew)
    {
        CI_FATAL("failed to create a shot\n");
        return ret;
    }

    ret = KRN_CI_PipelineAddShot(pKrnConfig, pNew);
    if (IMG_SUCCESS != ret)
    {
        CI_FATAL("failed to add a shot to the configuration\n");
        KRN_CI_ShotDestroy(pNew);
        pNew = NULL;
    }
    else
    {
        IMG_MEMSET(param.aMemMapIds, 0,
            CI_POOL_MAP_ID_COUNT*sizeof(IMG_UINT32));

        if (pNew->hwSave.sMemory.uiAllocated > 0)
        {
            param.aMemMapIds[CI_POOL_MAP_ID_STAT] = pNew->hwSave.ID;
        }
        if (pNew->sDPFWrite.sMemory.uiAllocated > 0)
        {
            param.aMemMapIds[CI_POOL_MAP_ID_DPF] = pNew->sDPFWrite.ID;
        }
        if (pNew->sENSOutput.sMemory.uiAllocated > 0)
        {
            param.aMemMapIds[CI_POOL_MAP_ID_ENS] = pNew->sENSOutput.ID;
        }
        param.iShotId = pNew->iIdentifier;
        IMG_MEMCPY(&(param.sample), &(pNew->userShot), sizeof(CI_SHOT));
    }

    ret = copy_to_user(userParam, &param, sizeof(struct CI_POOL_PARAM));
    if (ret)
    {
        CI_FATAL("Failed to copy_to_user a config add buffer parameter "\
            "struct\n");
        /* we need some cleaning here to remove the ready to be mapped
         * buffers (but it is unlikely that the copy_from_user succeeded and
         * that the copy_to_user will fail)*/
        ret = IMG_ERROR_INVALID_PARAMETERS;
    }

    return ret;
}

IMG_RESULT INT_CI_PipelineDeleteShots(KRN_CI_CONNECTION *pConnection,
    int pipelineID)
{
    KRN_CI_PIPELINE *pKrnConfig = NULL;
    sCell_T *pFound = NULL;
    IMG_RESULT ret = IMG_SUCCESS;
    IMG_UINT32 uiCleanProcessed = 0;

    IMG_ASSERT(pConnection != NULL);

    pFound = FindPipeline(pConnection, pipelineID);
    if (!pFound)
    {
        CI_FATAL("configuration %d is not registered\n", pipelineID);
        return IMG_ERROR_FATAL;
    }

    pKrnConfig = (KRN_CI_PIPELINE*)pFound->object;

    if (pKrnConfig->bStarted)
    {
        CI_FATAL("pipeline configuration is running - cannot delete shots\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    SYS_LockAcquire(&(pKrnConfig->sListLock));
    {
        if (pKrnConfig->sList_sent.ui32Elements != 0)
        {
            ret = IMG_ERROR_CANCELLED;
        }
        else
        {
            /* if present shots are removed from sList_processed to be able
             * to free memory (that can sleep) without the spinlock acquired */
            //SYS_LockAcquire(&(pKrnConfig->sAvailableLock));
            {
                // unlikely as we checked if the capture was started before
                if (pKrnConfig->sList_pending.ui32Elements != 0)
                {
                    ret = IMG_ERROR_CANCELLED;
                }
                else
                {
                    /*bCleanProcessed =
                        pKrnConfig->sList_processed.ui32Elements>0
                        ?IMG_TRUE:IMG_FALSE;*/

                    IMG_RESULT r =
                        SYS_SemTryDecrement(&(pKrnConfig->sProcessedSem));
                    while (IMG_SUCCESS == r)
                    {
                        uiCleanProcessed++;

                        pFound = List_popFront(&(pKrnConfig->sList_processed));
                        List_pushBack(&(pKrnConfig->sList_available), pFound);

                        r =
                            SYS_SemTryDecrement(&(pKrnConfig->sProcessedSem));
                    }
                    pFound = NULL;
                }
            }
            //SYS_LockRelease(&(pKrnConfig->sAvailableLock));

            // we can destroy the shots
            if (IMG_SUCCESS == ret)
            {
                // returns last visited which should be tail therefore NULL
                pFound = List_popFront(&(pKrnConfig->sList_available));
                while (pFound)
                {
                    ListDestroy_KRN_CI_Shot(pFound->object, NULL);
                    pFound = List_popFront(&(pKrnConfig->sList_available));
                }

                // sList_sent should be empty
                // sList_processed should be empty
                // sList_pending should be empty
            }
        }
    }
    SYS_LockRelease(&(pKrnConfig->sListLock));

    if (IMG_SUCCESS != ret)
    {
        CI_FATAL("Some shots are still acquired by user; cannot destroy "\
            "all shots\n");
        return ret;
    }

    CI_DEBUG("%u shots were removed from processed list to available list "\
        "prior to destruction\n", uiCleanProcessed);

    if (pFound) // unlikely
    {
        CI_FATAL("Failed to clean all available shots!\n");
        ret = IMG_ERROR_FATAL;
    }
    return ret;
}

IMG_RESULT INT_CI_PipelineHasPending(KRN_CI_CONNECTION *pConnection,
    int ui32CaptureId, IMG_BOOL8 *bResult)
{
    sCell_T *pFound = NULL;
    KRN_CI_PIPELINE *pKrnCapture = NULL;

    IMG_ASSERT(pConnection != NULL);
    IMG_ASSERT(bResult != NULL);

    pFound = FindPipeline(pConnection, ui32CaptureId);
    if (!pFound)
    {
        CI_FATAL("capture is not registered\n");
        return IMG_ERROR_FATAL;
    }
    pKrnCapture = (KRN_CI_PIPELINE*)pFound->object;
#ifndef INFOTM_USE_SPIN_LOCK // added by linyun.xiong @2016-03-22
    SYS_LockAcquire(&(pKrnCapture->sListLock));
#else
    SYS_SpinlockAcquire(&(pKrnCapture->sProcessingSpinlock));
#endif
    {
        *bResult = pKrnCapture->sList_pending.ui32Elements > 0
            ? IMG_TRUE : IMG_FALSE;
    }
#ifndef INFOTM_USE_SPIN_LOCK // added by linyun.xiong @2016-03-22
    SYS_LockRelease(&(pKrnCapture->sListLock));
#else
    SYS_SpinlockRelease(&(pKrnCapture->sProcessingSpinlock));
#endif
    return IMG_SUCCESS;
}

IMG_RESULT INT_CI_PipelineHasWaiting(KRN_CI_CONNECTION *pConnection,
    int ui32CaptureId, int *pNbWaiting)
{
    sCell_T *pFound = NULL;
    KRN_CI_PIPELINE *pKrnCapture = NULL;

    IMG_ASSERT(pConnection != NULL);
    IMG_ASSERT(pNbWaiting != NULL);

    pFound = FindPipeline(pConnection, ui32CaptureId);
    if (!pFound)
    {
        CI_FATAL("capture is not registered\n");
        return IMG_ERROR_FATAL;
    }
    pKrnCapture = (KRN_CI_PIPELINE*)pFound->object;
#ifndef INFOTM_USE_SPIN_LOCK // added by linyun.xiong @2016-03-22
    SYS_LockAcquire(&(pKrnCapture->sListLock));
#else
    SYS_SpinlockAcquire(&(pKrnCapture->sProcessingSpinlock));
#endif
    {
        *pNbWaiting = pKrnCapture->sList_processed.ui32Elements;
    }
#ifndef INFOTM_USE_SPIN_LOCK // added by linyun.xiong @2016-03-22
    SYS_LockRelease(&(pKrnCapture->sListLock));
#else
    SYS_SpinlockRelease(&(pKrnCapture->sProcessingSpinlock));
#endif
    return IMG_SUCCESS;
}

// linestore status adn size is updated when starting a capture!
IMG_RESULT INT_CI_DriverSetLineStore(const CI_LINESTORE __user *pNew)
{
    IMG_UINT32 i;
    IMG_RESULT ret = IMG_SUCCESS;
    CI_LINESTORE sKrnCopy;

    IMG_ASSERT(g_psCIDriver != NULL);

    ret = copy_from_user(&sKrnCopy, pNew, sizeof(CI_LINESTORE));
    if (ret)
    {
        CI_FATAL("failed to copy_from_user pNew\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    // linestore

    SYS_LockAcquire(&(g_psCIDriver->sConnectionLock));
    {

        // verify that the changes do not upset the running contexts
        for (i = 0; i < g_psCIDriver->sHWInfo.config_ui8NContexts
            && IMG_SUCCESS == ret; i++)
        {
            if (g_psCIDriver->sLinestoreStatus.aActive[i])
            {
                if (sKrnCopy.aStart[i] !=
                    g_psCIDriver->sLinestoreStatus.aStart[i])
                {
                    ret = IMG_ERROR_DEVICE_UNAVAILABLE;
                    CI_FATAL("context %d is active - cannot change its "\
                        "linestore start\n", i);
                }
                else if (sKrnCopy.aSize[i] <
                    g_psCIDriver->sLinestoreStatus.aSize[i])
                {
                    ret = IMG_ERROR_DEVICE_UNAVAILABLE;
                    CI_FATAL("context %d is active - cannot reduce its "\
                        "size\n", i);
                }
            }
        }

        // if it does not upset the running contexts apply the changes
        if (IMG_SUCCESS == ret)
        {
            for (i = 0; i < g_psCIDriver->sHWInfo.config_ui8NContexts; i++)
            {
                g_psCIDriver->sLinestoreStatus.aStart[i] = sKrnCopy.aStart[i];
                g_psCIDriver->sLinestoreStatus.aSize[i] = sKrnCopy.aSize[i];
            }
        }

    }
    SYS_LockRelease(&(g_psCIDriver->sConnectionLock));

    return ret;
}

IMG_RESULT INT_CI_DriverGetLineStore(CI_LINESTORE __user *pCopy)
{
    CI_LINESTORE sKrnCopy;
    int ret;
    IMG_MEMSET(&sKrnCopy, 0, sizeof(CI_LINESTORE));

    IMG_ASSERT(g_psCIDriver != NULL);
    // tested beforehand because it saves some locking time
    if (!pCopy)
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    // could be copied directly to user memory
    SYS_LockAcquire(&(g_psCIDriver->sConnectionLock));
    {
        IMG_MEMCPY(&sKrnCopy, &(g_psCIDriver->sLinestoreStatus),
            sizeof(CI_LINESTORE));
    }
    SYS_LockRelease(&(g_psCIDriver->sConnectionLock));

    ret = copy_to_user(pCopy, &sKrnCopy, sizeof(CI_LINESTORE));
    if (ret)
    {
        CI_FATAL("failed to copy_to_user Linestore\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    return IMG_SUCCESS;
}

IMG_RESULT INT_CI_DriverGetGammaLUT(CI_MODULE_GMA_LUT __user *pCopy)
{
    IMG_RESULT ret = IMG_SUCCESS;
    IMG_ASSERT(g_psCIDriver != NULL);

    SYS_LockAcquire(&(g_psCIDriver->sConnectionLock));
    {
        ret = copy_to_user(pCopy, &(g_psCIDriver->sGammaLUT),
            sizeof(CI_MODULE_GMA_LUT));
        if (ret)
        {
            CI_FATAL("failed to copy_to_user pCopy Gamma LUT\n");
            ret = IMG_ERROR_INVALID_PARAMETERS;
        }
    }
    SYS_LockRelease(&(g_psCIDriver->sConnectionLock));

    return ret;
}

IMG_RESULT INT_CI_DriverSetGammaLUT(const CI_MODULE_GMA_LUT __user *pNew)
{
    IMG_RESULT ret;
    CI_MODULE_GMA_LUT sKrnCopy;

    IMG_ASSERT(g_psCIDriver != NULL);

    ret = copy_from_user(&sKrnCopy, pNew, sizeof(CI_MODULE_GMA_LUT));
    if (ret)
    {
        CI_FATAL("failed to copy_from_user pCopy Gamma LUT\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    ret = KRN_CI_DriverChangeGammaLUT(&sKrnCopy);
    if (IMG_SUCCESS != ret)
    {
        CI_FATAL("failed to change Gamma LUT\n");
    }

    return ret;
}

IMG_RESULT INT_CI_DriverGetTimestamp(IMG_UINT32 __user *puiTimestamp)
{
    IMG_UINT32 timestamp;
    int ret;

    IMG_ASSERT(g_psCIDriver != NULL);

    HW_CI_ReadTimestamps(&timestamp);

    ret = copy_to_user(puiTimestamp, &timestamp, sizeof(IMG_UINT32));
    if (ret)
    {
        CI_FATAL("failed to copy_to_user the timestamp\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    return IMG_SUCCESS;
}

IMG_RESULT INT_CI_DriverGetDPFInternal(IMG_UINT32 __user *puiDPFInt)
{
    IMG_UINT32 currVal = 0;
    int ret;

    IMG_ASSERT(g_psCIDriver != NULL);

    SYS_LockAcquire(&(g_psCIDriver->sConnectionLock));
    {
        currVal = g_psCIDriver->ui32DPFInternalBuff;
    }
    SYS_LockRelease(&(g_psCIDriver->sConnectionLock));

    ret = copy_to_user(puiDPFInt, &currVal, sizeof(IMG_UINT32));
    if (ret)
    {
        CI_FATAL("failed to copy_to_user the DPF internal buffer value\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    return IMG_SUCCESS;
}

IMG_RESULT INT_CI_DriverGetRTM(CI_RTM_INFO __user *pRTMInfo)
{
    CI_RTM_INFO sKrnRTM;
    int r;
    IMG_UINT8 i;

    IMG_ASSERT(g_psCIDriver != NULL);

    if (g_psCIDriver->sHWInfo.config_ui8NRTMRegisters > FELIX_MAX_RTM_VALUES)
    {
        // should not happen
        CI_FATAL("not enough values in FELIX_MAX_RTM_VALUES!!!");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    IMG_MEMSET(&sKrnRTM, 0, sizeof(CI_RTM_INFO));

    for (i = 0; i < g_psCIDriver->sHWInfo.config_ui8NRTMRegisters; i++)
    {
        sKrnRTM.aRTMEntries[i] = HW_CI_ReadCoreRTM(i);
    }
    for (i = 0; i < g_psCIDriver->sHWInfo.config_ui8NContexts; i++)
    {
        HW_CI_ReadContextRTM(i, &sKrnRTM);
    }

    r = copy_to_user(pRTMInfo, &sKrnRTM, sizeof(CI_RTM_INFO));
    if (r)
    {
        CI_FATAL("failed to copy_to_user the RTM value\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    return IMG_SUCCESS;
}

#ifdef CI_DEBUG_FCT
#include <tal.h>
#endif

IMG_RESULT INT_CI_DebugReg(struct CI_DEBUG_REG_PARAM __user *pInfo)
{
#ifdef CI_DEBUG_FCT
    struct CI_DEBUG_REG_PARAM krnV;
    int r;
    IMG_HANDLE regHandle = NULL;
    IMG_RESULT ret;

    IMG_ASSERT(g_psCIDriver != NULL);

    r = copy_from_user(&krnV, pInfo, sizeof(struct CI_DEBUG_REG_PARAM));
    if (r)
    {
        CI_FATAL("failed to copy_from_user the Debug Reg value\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    if (CI_BANK_CORE == krnV.eBank)
    {
        regHandle = g_psCIDriver->sTalHandles.hRegFelixCore;
    }
    else if (CI_BANK_CTX <= krnV.eBank && CI_BANK_CTX_MAX > krnV.eBank)
    {
        r = krnV.eBank - CI_BANK_CTX;
        if (r >= (int)g_psCIDriver->sHWInfo.config_ui8NContexts)
        {
            CI_FATAL("trying to read from unexisting context %d\n", r);
            return IMG_ERROR_INVALID_PARAMETERS;
        }
        regHandle = g_psCIDriver->sTalHandles.hRegFelixContext[r];
    }
    else if (CI_BANK_GASKET <= krnV.eBank && CI_BANK_GASKET_MAX > krnV.eBank)
    {
        r = krnV.eBank - CI_BANK_GASKET;
        if (r >= (int)g_psCIDriver->sHWInfo.config_ui8NImagers)
        {
            CI_FATAL("trying to read from unexisting gasket %d\n", r);
            return IMG_ERROR_INVALID_PARAMETERS;
        }
        regHandle = g_psCIDriver->sTalHandles.hRegGaskets[r];
    }
    else if (CI_BANK_IIFDG <= krnV.eBank && CI_BANK_IIF_MAX > krnV.eBank)
    {
        r = krnV.eBank - CI_BANK_IIFDG;
        if (r >= (int)g_psCIDriver->sHWInfo.config_ui8NIIFDataGenerator)
        {
            CI_FATAL("trying to read from unexisting IIFDG %d\n", r);
            return IMG_ERROR_INVALID_PARAMETERS;
        }
        regHandle = g_psCIDriver->sTalHandles.hRegIIFDataGen[r];
    }
    else if (CI_BANK_MEM == krnV.eBank)
    {
        regHandle = g_psCIDriver->sTalHandles.hMemHandle;
    }

    ret = IMG_ERROR_INVALID_PARAMETERS;
    if (regHandle)
    {
        if (krnV.bRead)
        {
            if (CI_BANK_MEM == krnV.eBank)
            {
                ret = TALMEM_ReadWord32(regHandle,
                    (IMG_UINT64)krnV.ui32Offset,
                    &krnV.ui32Value);
            }
            else
            {
                ret = TALREG_ReadWord32(regHandle,
                    (IMG_UINT64)krnV.ui32Offset,
                    &krnV.ui32Value);
            }
            if (ret)
            {
                CI_FATAL("failed to read\n");
                return ret;
            }

            r = copy_to_user(pInfo, &krnV, sizeof(struct CI_DEBUG_REG_PARAM));
            if (r)
            {
                CI_FATAL("failed to copy_to_user the Debug Reg value\n");
                return ret;
            }
        }
        else
        {
            if (CI_BANK_MEM == krnV.eBank)
            {
                ret = TALMEM_WriteWord32(regHandle,
                    (IMG_UINT64)krnV.ui32Offset, krnV.ui32Value);
            }
            else
            {
                ret = TALREG_WriteWord32(regHandle,
                    krnV.ui32Offset, krnV.ui32Value);
            }

            if (ret)
            {
                CI_FATAL("failed to write\n");
                return ret;
            }
        }
    }

    return ret;
#else
    return IMG_ERROR_NOT_SUPPORTED;
#endif
}

IMG_RESULT INT_CI_GasketAcquire(KRN_CI_CONNECTION *pConnection,
    CI_GASKET __user *pGasket)
{
    CI_GASKET sKrnGasket;
    IMG_RESULT ret;
    IMG_ASSERT(g_psCIDriver != NULL);

    ret = copy_from_user(&sKrnGasket, pGasket, sizeof(CI_GASKET));
    if (ret)
    {
        CI_FATAL("failed to copy_from_user the gasket info\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    if (sKrnGasket.uiGasket >= g_psCIDriver->sHWInfo.config_ui8NImagers)
    {
        CI_FATAL("gasket %d not supported - max gasket %d\n",
            sKrnGasket.uiGasket, g_psCIDriver->sHWInfo.config_ui8NImagers);
        return IMG_ERROR_DEVICE_UNAVAILABLE;
    }

    if ((sKrnGasket.bParallel == IMG_TRUE && 0 == (g_psCIDriver->sHWInfo.\
        gasket_aType[sKrnGasket.uiGasket] & CI_GASKET_PARALLEL))
        ||
        (sKrnGasket.bParallel == IMG_FALSE && 0 == (g_psCIDriver->sHWInfo.\
        gasket_aType[sKrnGasket.uiGasket] & CI_GASKET_MIPI)))
    {
        CI_FATAL("gasket %d does not support %s format!\n",
            sKrnGasket.uiGasket, sKrnGasket.bParallel ? "Parallel" : "MIPI");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    ret = KRN_CI_DriverAcquireGasket(sKrnGasket.uiGasket, pConnection);
    if (IMG_SUCCESS != ret)
    {
        return ret;
    }
    // now this connection is the owner of the gasket - we can configure it

    ret = HW_CI_WriteGasket(&sKrnGasket, 1);
    if (IMG_SUCCESS != ret)
    {
        CI_FATAL("unsuported parallel bitdepth %d\n",
            sKrnGasket.ui8ParallelBitdepth);
        KRN_CI_DriverReleaseGasket(sKrnGasket.uiGasket);
    }

    return ret;
}

IMG_RESULT INT_CI_GasketRelease(KRN_CI_CONNECTION *pConnection,
    IMG_UINT8 uiGasket)
{
    CI_GASKET sKrnGasket;
    IMG_RESULT ret = IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;

    IMG_ASSERT(pConnection != NULL);

    sKrnGasket.uiGasket = uiGasket;

    if (sKrnGasket.uiGasket >= g_psCIDriver->sHWInfo.config_ui8NImagers)
    {
        CI_FATAL("gasket %d not supported - max gasket %d\n",
            uiGasket, g_psCIDriver->sHWInfo.config_ui8NImagers);
        return IMG_ERROR_DEVICE_UNAVAILABLE;
    }

    if (KRN_CI_DriverCheckGasket(sKrnGasket.uiGasket, pConnection))
    {
        /* the CI_GASKET structure is only used to know which gasket it is
         * disabling here */
        ret = HW_CI_WriteGasket(&sKrnGasket, 0);
        if (IMG_SUCCESS == ret)
        {
            KRN_CI_DriverReleaseGasket(sKrnGasket.uiGasket);
        }
    }

    return ret;
}

IMG_RESULT INT_CI_GasketGetInfo(struct CI_GASKET_PARAM __user *pUserGasket)
{
    struct CI_GASKET_PARAM gasket;
    int ret;

    ret = copy_from_user(&gasket, pUserGasket, sizeof(struct CI_GASKET_PARAM));
    if (ret)
    {
        CI_FATAL("Failed to copy gasket info request\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    if (gasket.uiGasket >= g_psCIDriver->sHWInfo.config_ui8NImagers)
    {
        CI_FATAL("gasket %d not available (NGaskets=NImages=%d)\n",
            gasket.uiGasket, g_psCIDriver->sHWInfo.config_ui8NImagers);
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    IMG_MEMSET(&(gasket.sGasketInfo), 0, sizeof(CI_GASKET_INFO));

    HW_CI_ReadGasket(&(gasket.sGasketInfo), gasket.uiGasket);

    ret = copy_to_user(pUserGasket, &gasket, sizeof(struct CI_GASKET_PARAM));
    if (ret)
    {
        CI_FATAL("Failed to copy gasket info back to user\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    return IMG_SUCCESS;
}

/*
 * internal dg commands
 */

IMG_RESULT INT_CI_DatagenRegister(KRN_CI_CONNECTION *pConnection,
    int iIIFDGId, int *regId)
{
    IMG_RESULT ret = IMG_ERROR_NOT_SUPPORTED;
    KRN_CI_DATAGEN *pKrnDatagen = NULL;

    IMG_ASSERT(g_psCIDriver != NULL);
    IMG_ASSERT(pConnection != NULL);
    IMG_ASSERT(regId != NULL);

    *regId = -1; // invalid

    if (0 == (g_psCIDriver->sHWInfo.\
        eFunctionalities&CI_INFO_SUPPORTED_IIF_DATAGEN))
    {
        CI_FATAL("HW does not support internal DG!\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    if (iIIFDGId < 0)
    {
        // registration
        pKrnDatagen = (KRN_CI_DATAGEN*)IMG_CALLOC(1, sizeof(KRN_CI_DATAGEN));
        if (pKrnDatagen == NULL)
        {
            CI_FATAL("Failed to allocate internal datagen structure "\
                "(%ld Bytes)\n", sizeof(KRN_CI_DATAGEN));
            return IMG_ERROR_MALLOC_FAILED;
        }

        // initialise elements of the IIF DG
        ret = KRN_CI_DatagenInit(pKrnDatagen, pConnection);
        if (IMG_SUCCESS == ret)
        {
            *regId = pKrnDatagen->ui32Identifier;
        }
        else
        {
            IMG_FREE(pKrnDatagen);
            pKrnDatagen = NULL;
        }
    }
    else
    {
        // deregistration
        sCell_T *pDGCell = FindDatagen(pConnection, iIIFDGId);

        if (!pDGCell)
        {
            CI_FATAL("could not find registered data generator %d\n",
                iIIFDGId);
            return IMG_ERROR_FATAL;
        }

        pKrnDatagen = (KRN_CI_DATAGEN*)pDGCell->object;
        IMG_ASSERT(pKrnDatagen != NULL);

        if (pKrnDatagen->bCapturing)
        {
            CI_WARNING("trying to destroy a running DG - stop it first\n");
            //KRN_CI_DatagenStop();

        }

        // destroy elemets of data generator
        ret = KRN_CI_DatagenClear(pKrnDatagen);
        *regId = ret; // because given as "ret"
    }

    return ret;
}

IMG_RESULT INT_CI_DatagenStart(KRN_CI_CONNECTION *pConnection,
    const struct CI_DG_PARAM __user *pUserParam)
{
    struct CI_DG_PARAM sKernelParam;
    KRN_CI_DATAGEN *pKrnDatagen = NULL;
    sCell_T *pDGCell = NULL;
    IMG_RESULT ret;

    IMG_ASSERT(pConnection != NULL);

    ret = copy_from_user(&sKernelParam, pUserParam,
        sizeof(struct CI_DG_PARAM));
    if (ret)
    {
        CI_FATAL("failed to copy_from_user the datagen info\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pDGCell = FindDatagen(pConnection, sKernelParam.datagenId);
    if (!pDGCell)
    {
        CI_FATAL("could not find registered data generator %d\n",
            sKernelParam.datagenId);
        return IMG_ERROR_FATAL;
    }

    pKrnDatagen = (KRN_CI_DATAGEN *)pDGCell->object;

    ret = KRN_CI_DatagenStart(pKrnDatagen, &(sKernelParam.sDG));

    return ret;
}

IMG_RESULT INT_CI_DatagenIsStarted(KRN_CI_CONNECTION *pConnection,
    int iIIFDGId, int *pbResult)
{
    KRN_CI_DATAGEN *pKrnDatagen = NULL;
    sCell_T *pDGCell = NULL;

    IMG_ASSERT(pConnection != NULL);
    IMG_ASSERT(pbResult != NULL);

    pDGCell = FindDatagen(pConnection, iIIFDGId);
    if (!pDGCell)
    {
        CI_FATAL("could not find registered data generator %d\n", iIIFDGId);
        return IMG_ERROR_FATAL;
    }

    pKrnDatagen = (KRN_CI_DATAGEN *)pDGCell->object;

    *pbResult = pKrnDatagen->bCapturing;

    return IMG_SUCCESS;
}

IMG_RESULT INT_CI_DatagenStop(KRN_CI_CONNECTION *pConnection, int iIIFDGId)
{
    KRN_CI_DATAGEN *pKrnDatagen = NULL;
    sCell_T *pDGCell = NULL;
    IMG_RESULT ret;

    IMG_ASSERT(pConnection != NULL);

    pDGCell = FindDatagen(pConnection, iIIFDGId);
    if (!pDGCell)
    {
        CI_FATAL("could not find registered data generator %d\n", iIIFDGId);
        return IMG_ERROR_FATAL;
    }

    pKrnDatagen = (KRN_CI_DATAGEN *)pDGCell->object;

    ret = KRN_CI_DatagenStop(pKrnDatagen);

    return ret;
}

IMG_RESULT INT_CI_DatagenAllocate(KRN_CI_CONNECTION *pConnection,
struct CI_DG_FRAMEINFO __user *pUserParam)
{
    struct CI_DG_FRAMEINFO sKernelParam;
    KRN_CI_DATAGEN *pKrnDatagen = NULL;
    sCell_T *pDGCell = NULL;
    IMG_RESULT ret;

    IMG_ASSERT(pConnection != NULL);

    ret = copy_from_user(&sKernelParam, pUserParam,
        sizeof(struct CI_DG_FRAMEINFO));
    if (ret)
    {
        CI_FATAL("failed to copy_from_user the datagen frame info\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pDGCell = FindDatagen(pConnection, sKernelParam.datagenId);
    if (!pDGCell)
    {
        CI_FATAL("could not find registered datagenerator %d\n",
            sKernelParam.datagenId);
        return IMG_ERROR_FATAL;
    }

    pKrnDatagen = (KRN_CI_DATAGEN *)pDGCell->object;

    ret = KRN_CI_DatagenAllocateFrame(pKrnDatagen, &sKernelParam);
    if (ret != IMG_SUCCESS)
    {
        CI_FATAL("Failed to allocate a data generator frame\n");
        return ret;
    }

    ret = copy_to_user(pUserParam, &sKernelParam,
        sizeof(struct CI_DG_FRAMEINFO));
    if (ret)
    {
        CI_FATAL("failed to copy_to_user the datagen frame info\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    return IMG_SUCCESS;
}

IMG_RESULT INT_CI_DatagenAcquireFrame(KRN_CI_CONNECTION *pConnection,
struct CI_DG_FRAMEINFO __user *pUserParam)
{
    struct CI_DG_FRAMEINFO sKernelParam;
    KRN_CI_DATAGEN *pKrnDatagen = NULL;
    sCell_T *pFound = NULL;
    IMG_RESULT ret;

    IMG_ASSERT(pConnection != NULL);

    ret = copy_from_user(&sKernelParam, pUserParam,
        sizeof(struct CI_DG_FRAMEINFO));
    if (ret)
    {
        CI_FATAL("failed to copy_from_user the datagen frame info\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pFound = FindDatagen(pConnection, sKernelParam.datagenId);
    if (!pFound)
    {
        CI_FATAL("could not find registered datagenerator %d\n",
            sKernelParam.datagenId);
        return IMG_ERROR_FATAL;
    }

    pKrnDatagen = (KRN_CI_DATAGEN *)pFound->object;

    ret = KRN_CI_DatagenAcquireFrame(pKrnDatagen, &sKernelParam);
    if (IMG_SUCCESS != ret)
    {
        CI_FATAL("could not acquire frame\n");
        return ret;
    }

    ret = copy_to_user(pUserParam, &sKernelParam,
        sizeof(struct CI_DG_FRAMEINFO));
    if (ret)
    {
        CI_FATAL("failed to copy_to_user the datagen frame info\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    return IMG_SUCCESS;
}

IMG_RESULT INT_CI_DatagenTrigger(KRN_CI_CONNECTION *pConnection,
    const struct CI_DG_FRAMETRIG __user *pUserParam)
{
    struct CI_DG_FRAMETRIG sKernelParam;
    KRN_CI_DATAGEN *pKrnDatagen = NULL;
    sCell_T *pFound = NULL;
    int ret;

    IMG_ASSERT(pConnection != NULL);

    ret = copy_from_user(&sKernelParam, pUserParam,
        sizeof(struct CI_DG_FRAMETRIG));
    if (ret)
    {
        CI_FATAL("failed to copy_from_user the datagen frame info\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pFound = FindDatagen(pConnection, sKernelParam.datagenId);
    if (!pFound)
    {
        CI_FATAL("could not find registered datagenerator %d\n",
            sKernelParam.datagenId);
        return IMG_ERROR_FATAL;
    }

    pKrnDatagen = (KRN_CI_DATAGEN *)pFound->object;

    if (sKernelParam.bTrigger)
    {
        return KRN_CI_DatagenTriggerFrame(pKrnDatagen, &sKernelParam);
    }

    return KRN_CI_DatagenReleaseFrame(pKrnDatagen, sKernelParam.uiFrameId);
}

IMG_RESULT INT_CI_DatagenFreeFrame(KRN_CI_CONNECTION *pConnection,
    const struct CI_DG_FRAMEINFO __user *pUserParam)
{
    struct CI_DG_FRAMEINFO sKernelParam;
    KRN_CI_DATAGEN *pKrnDatagen = NULL;
    sCell_T *pFound = NULL;
    IMG_RESULT ret = IMG_SUCCESS;

    IMG_ASSERT(pConnection != NULL);

    ret = copy_from_user(&sKernelParam, pUserParam,
        sizeof(struct CI_DG_FRAMEINFO));
    if (ret)
    {
        CI_FATAL("failed to copy_from_user the datagen frame info\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pFound = FindDatagen(pConnection, sKernelParam.datagenId);
    if (!pFound)
    {
        CI_FATAL("could not find registered datagenerator %d\n",
            sKernelParam.datagenId);
        return IMG_ERROR_FATAL;
    }

    pKrnDatagen = (KRN_CI_DATAGEN *)pFound->object;
    if (pKrnDatagen->bCapturing)
    {
        CI_FATAL("Cannot delete a buffer while running - stop the "\
            "capture first\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    SYS_LockAcquire(&(pKrnDatagen->slistLock));
    {
        int identifier = sKernelParam.uiFrameId;
        pFound = List_visitor(&(pKrnDatagen->sList_available), &identifier,
            &List_FindDGBuffer);

        if (pFound)
        {
            List_detach(pFound);
        }
    }
    SYS_LockRelease(&(pKrnDatagen->slistLock));

    if (pFound)
    {
        ret = KRN_CI_DatagenFreeFrame((KRN_CI_DGBUFFER*)pFound->object);
        if (IMG_SUCCESS != ret)
        {
            CI_FATAL("Failed to free DG Buffer!\n");
        }
    }
    else
    {
        CI_FATAL("Given frame not found in the available list\n");
        ret = IMG_ERROR_FATAL;
    }

    return ret;
}

#if defined(INFOTM_ISP)
IMG_RESULT INT_CI_RegGet(KRN_CI_CONNECTION *pConnection, struct CI_REG_PARAM __user *pUserParam)
{
	struct KRN_CI_REG_PARAM sKernelParam;
    IMG_HANDLE hRegHandle = NULL;
    IMG_RESULT ret = IMG_SUCCESS;
    IMG_ASSERT(g_psCIDriver != NULL);

    SYS_LockAcquire(&(g_psCIDriver->sConnectionLock));
    {
        if (copy_from_user(&sKernelParam, pUserParam, sizeof(struct KRN_CI_REG_PARAM)) != 0)
        {
            CI_FATAL("failed to copy_from_user pCopy Register Param\n");
            return IMG_ERROR_INVALID_PARAMETERS;
        }

        hRegHandle = TAL_GetMemSpaceHandle(sKernelParam.pchRegSpaceName);
        if (hRegHandle) {
            TALREG_ReadWord32(hRegHandle, sKernelParam.ui32RegOffset, &sKernelParam.ui32RegValue);
        } else {
            ret = IMG_ERROR_INVALID_PARAMETERS;
        }

        if (copy_to_user(pUserParam, &sKernelParam, sizeof(struct KRN_CI_REG_PARAM)) != 0)
        {
            CI_FATAL("failed to copy_to_user pCopy Register Param\n");
            ret = IMG_ERROR_INVALID_PARAMETERS;
        }
    }
    SYS_LockRelease(&(g_psCIDriver->sConnectionLock));

    return ret;
}

IMG_RESULT INT_CI_RegSet(KRN_CI_CONNECTION *pConnection, const struct CI_REG_PARAM __user *pUserParam)
{
	struct KRN_CI_REG_PARAM sKernelParam;
    IMG_HANDLE hRegHandle = NULL;
    IMG_RESULT ret = IMG_SUCCESS;
    IMG_ASSERT(g_psCIDriver != NULL);

    SYS_LockAcquire(&(g_psCIDriver->sConnectionLock));
    {
        if (copy_from_user(&sKernelParam, pUserParam, sizeof(struct KRN_CI_REG_PARAM)) != 0)
        {
            CI_FATAL("failed to copy_from_user pCopy Register Param\n");
            return IMG_ERROR_INVALID_PARAMETERS;
        }

        hRegHandle = TAL_GetMemSpaceHandle(sKernelParam.pchRegSpaceName);
        if (hRegHandle) {
            TALREG_WriteWord32(hRegHandle, sKernelParam.ui32RegOffset, sKernelParam.ui32RegValue);
        } else {
            ret = IMG_ERROR_INVALID_PARAMETERS;
        }
    }
    SYS_LockRelease(&(g_psCIDriver->sConnectionLock));

    return ret;
}

IMG_RESULT INT_CI_PipelineSetYUV(KRN_CI_CONNECTION *pConnection, int flag)
{
	SYS_LockAcquire(&(g_psCIDriver->sConnectionLock));
	{
		IsLowLight = flag;
		if(flag && (NULL == g_buffer.pAlloc)) {
			//for debug used.
			//printk("alloc ::g_ui32Size = %d====, g_buffer.pAlloc= 0x%x\n", g_ui32Size, g_buffer.pAlloc);
			SYS_MemAlloc(&g_buffer, g_ui32Size, 0,
#ifdef INFOTM_ISP
					0, "YUV-Const");
#else
					NULL, "YUV-Const");
#endif

		}
		else if(!flag) {
			if(g_buffer.pAlloc) {
				SYS_MemFree(&g_buffer);
				memset((void *)&g_buffer, 0, sizeof(g_buffer));
			}

		}
	}
	SYS_LockRelease(&(g_psCIDriver->sConnectionLock));
	return IMG_SUCCESS;
}

#endif //INFOTM_ISP
