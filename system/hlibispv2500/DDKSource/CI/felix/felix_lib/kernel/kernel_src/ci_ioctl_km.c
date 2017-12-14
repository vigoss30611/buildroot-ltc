/**
*******************************************************************************
@file ci_ioctl_km.c

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

******************************************************************************/
#include "ci_kernel/ci_kernel.h"
#include "ci_kernel/ci_connection.h"
#include "ci_internal/ci_errors.h" // toErrno and toImgResult
#include "ci_kernel/ci_ioctrl.h"

// write GMA_LUT when resuming & reset HW
#include "ci_kernel/ci_hwstruct.h"
#include "registers/core.h"
#include "target.h"

#ifdef FELIX_FAKE
#include <errno.h>
#else
#include <linux/errno.h>
//#include <asm/uaccess.h> // copy_to_user and copy_from_user
#endif

#ifdef IMG_SCB_DRIVER
extern void sys_dev_pci_resume(void);
#endif

int DEV_CI_Open(struct inode *inode, struct file *flip)
{
    KRN_CI_CONNECTION *pKrnConnection = NULL;
    IMG_RESULT ret = IMG_SUCCESS;
    (void)inode; // not used at the moment

    CI_INFO("connection received\n");
    ret = KRN_CI_ConnectionCreate(&pKrnConnection);
    if (IMG_SUCCESS != ret)
    {
        CI_FATAL("failed to create the kernel connection object "\
            "(returned %d)\n", ret);
        return -1;
    }

    flip->private_data = (void*)pKrnConnection;
    return 0; // success
}

int DEV_CI_Close(struct inode *inode, struct file *flip)
{
    IMG_RESULT ret = IMG_SUCCESS;
    (void)inode; // not used at the moment

    IMG_ASSERT(flip->private_data != NULL);

    CI_INFO("connection closed\n");
    ret = INT_CI_DriverDisconnect((KRN_CI_CONNECTION*)flip->private_data);
    if (IMG_SUCCESS != ret)
    {
        CI_FATAL("failed to disconnect (returned %d)\n", ret);
        return -1;
    }

    return 0; // success
}

long DEV_CI_Ioctl(struct file *flip, unsigned int ioctl_cmd,
    unsigned long ioctl_param)
{
    int ret = 0;
    int givenId = 0;
    IMG_RESULT imgRet = IMG_SUCCESS;
    KRN_CI_CONNECTION *pKrnConnection = NULL;

    if (flip == NULL || flip->private_data == NULL)
    {
        CI_FATAL("file structure is NULL or does not have private data\n");
        return (long)-ENODEV;
    }
    // we are in bad trouble: the driver does not exists!
    if (g_psCIDriver == NULL)
    {
        CI_FATAL("the driver is not initialised!!!\n");
        return (long)-ENODEV;
    }
    pKrnConnection = (KRN_CI_CONNECTION*)flip->private_data;

    //CI_INFO("ioctl received: 0x%x 0x%x\n", ioctl_cmd, ioctl_param);
    switch (ioctl_cmd)
    {
        /*
        * Driver part
        */
    case CI_IOCTL_INFO: //CI_DriverInit()
        CI_DEBUG("ioctl 0x%X=CI_INFO\n", ioctl_cmd);
        imgRet = INT_CI_DriverConnect(pKrnConnection,
            (CI_CONNECTION __user *)ioctl_param);
        if (IMG_SUCCESS != imgRet)
        {
            CI_FATAL("Failed to create a new connection (returned %d)\n",
                imgRet);
            ret = (long)toErrno(imgRet);
        }
        break;

    case CI_IOCTL_LINE_GET: //CI_DriverGetLinestore()
        CI_DEBUG("ioctl 0x%X=LINE_GET\n", ioctl_cmd);
        imgRet = INT_CI_DriverGetLineStore((CI_LINESTORE __user *)ioctl_param);
        if (IMG_SUCCESS != imgRet)
        {
            CI_FATAL("Failed to get linestore information (returned %d)\n",
                imgRet);
            ret = toErrno(imgRet);
        }
        break;

    case CI_IOCTL_LINE_SET: //CI_DriverSetLinestore()
        CI_DEBUG("ioctl 0x%X=LINE_SET\n", ioctl_cmd);
        imgRet = INT_CI_DriverSetLineStore((CI_LINESTORE __user *)ioctl_param);
        if (IMG_SUCCESS != imgRet)
        {
            CI_FATAL("Failed to set the linestore information "\
                "(returned %d)\n", imgRet);
            ret = toErrno(imgRet);
        }
        break;

    case CI_IOCTL_GMAL_GET: //CI_DriverGetGammaLUT()
        CI_DEBUG("ioctl 0x%X=GMAL_GET\n", ioctl_cmd);
        imgRet = INT_CI_DriverGetGammaLUT(
            (CI_MODULE_GMA_LUT __user *)ioctl_param);
        if (IMG_SUCCESS != imgRet)
        {
            CI_FATAL("Failed to get the Gamma LUT information "\
                "(returned %d)\n", imgRet);
            ret = toErrno(imgRet);
        }
        break;

    case CI_IOCTL_GMAL_SET: //CI_DriverSetGammaLUT()
        CI_DEBUG("ioctl 0x%X=GMAL_SET\n", ioctl_cmd);
        imgRet = INT_CI_DriverSetGammaLUT(
            (CI_MODULE_GMA_LUT __user *)ioctl_param);
        if (IMG_SUCCESS != imgRet)
        {
            CI_FATAL("Failed to set a new Gamma LUT (returned %d)\n", imgRet);
            ret = toErrno(imgRet);
        }
        break;

    case CI_IOCTL_TIME_GET: //CI_DriverGetTimestamp()
        CI_DEBUG("ioctl 0x%X=TIME_GET\n", ioctl_cmd);
        imgRet = INT_CI_DriverGetTimestamp((IMG_UINT32 __user *)ioctl_param);
        if (IMG_SUCCESS != imgRet)
        {
            CI_FATAL("Failed to get the current tiemstamp (returned %d)\n",
                imgRet);
            ret = toErrno(imgRet);
        }
        break;

    case CI_IOCTL_DPFI_GET: //CI_DriverGetAvailableInternalDPFBuffer()
        CI_DEBUG("ioctl 0x%x=DPFI_GET\n", ioctl_cmd);
        imgRet = INT_CI_DriverGetDPFInternal((IMG_UINT32 __user*)ioctl_param);
        if (IMG_SUCCESS != imgRet)
        {
            CI_FATAL("Failed to get the available DPF internal buffer "\
                "(returned %d)\n", imgRet);
            ret = toErrno(imgRet);
        }
        break;

    case CI_IOCTL_RTM_GET:
        CI_DEBUG("ioctl 0x%x=RTM_GET\n", ioctl_cmd);
        imgRet = INT_CI_DriverGetRTM((CI_RTM_INFO __user*)ioctl_param);
        if (IMG_SUCCESS != imgRet)
        {
            CI_FATAL("Failed to get RTM information (returend %d)\n", imgRet);
            ret = toErrno(imgRet);
        }
        break;

#ifdef CI_DEBUG_FCT
    case CI_IOCTL_DBG_REG:
        CI_DEBUG("ioctl 0x%x=DBG_REG\n", ioctl_cmd);
        imgRet = INT_CI_DebugReg(
            (struct CI_DEBUG_REG_PARAM __user*)ioctl_param);
        if (IMG_SUCCESS != imgRet)
        {
            CI_FATAL("Failed to Debug Reg information (returend %d)\n",
                imgRet);
            ret = toErrno(imgRet);
        }
        break;
#endif /* CI_DEBUG_FCT */

        /*
        * Config part of the pipeline
        */
    case CI_IOCTL_PIPE_REG: //CI_PipelineRegister()
        CI_DEBUG("ioctl 0x%X=PIPE_REG\n", ioctl_cmd);
        imgRet = INT_CI_PipelineRegister(pKrnConnection,
            (CI_PIPELINE __user *)ioctl_param, &ret);
        if (IMG_SUCCESS != imgRet)
        {
            CI_FATAL("Failed to register the configuration (returned %d)\n",
                imgRet);
            ret = toErrno(imgRet);
        }
        break;

    case CI_IOCTL_PIPE_DEL: //CI_PipelineDestroy()
    {
        CI_DEBUG("ioctl 0x%X=PIPE_DEL\n", ioctl_cmd);
        givenId = (int)ioctl_param;
        imgRet = INT_CI_PipelineDeregister(pKrnConnection, givenId);
        if (IMG_SUCCESS != imgRet)
        {
            CI_FATAL("Failed to deregister pipeline id=%d\n", givenId);
            ret = toErrno(imgRet);
        }
    }
    break;

    case CI_IOCTL_PIPE_ADD: //CI_PipelineAddPool()
    {
        CI_DEBUG("ioctl 0x%X=PIPE_ADD\n", ioctl_cmd);
        imgRet = INT_CI_PipelineAddShot(pKrnConnection,
            (struct CI_POOL_PARAM __user *)ioctl_param);
        if (IMG_SUCCESS != imgRet)
        {
            CI_FATAL("Failed to add buffer (returned %d)\n", imgRet);
            ret = toErrno(imgRet);
        }
    }
    break;

    case CI_IOCTL_PIPE_REM:
    {
        CI_DEBUG("ioctl 0x%X=PIPE_REM\n", ioctl_cmd);
        imgRet = INT_CI_PipelineDeleteShots(pKrnConnection,
            (int)ioctl_param);
        if (IMG_SUCCESS != imgRet)
        {
            CI_FATAL("Failed to delete buffers (returned %d)\n", imgRet);
            ret = toErrno(imgRet);
        }
    }
    break;

    case CI_IOCTL_PIPE_UPD: //IMG_CI_PipelineUpdate()
    {
        CI_DEBUG("ioctl 0x%X=PIPE_UPD\n", ioctl_cmd);
        imgRet = INT_CI_PipelineUpdate(pKrnConnection,
            (struct CI_PIPE_PARAM __user *)ioctl_param);
        if (IMG_SUCCESS != imgRet)
        {
            CI_FATAL("Failed to update a kernel-side capture\n");
            ret = toErrno(imgRet);
        }
    }
    break;

    case CI_IOCTL_CREATE_BUFF: //IMG_CI_PipelineCreateBuffer()
    {
        CI_DEBUG("ioctl 0x%X=CREATE_BUFF\n", ioctl_cmd);
        imgRet = INT_CI_PipelineCreateBuffer(pKrnConnection,
            (struct CI_ALLOC_PARAM __user *)ioctl_param);
        if (IMG_SUCCESS != imgRet)
        {
            CI_FATAL("Failed to add buffers to kernel space\n");
            ret = toErrno(imgRet);
        }
    }
    break;

    case CI_IOCTL_DEREG_BUFF: //CI_PipelineDeregisterBuffer()
    {
        CI_DEBUG("ioctl 0x%X=DEREG_BUFF\n", ioctl_cmd);
        imgRet = INT_CI_PipelineDeregBuffer(pKrnConnection,
            (struct CI_ALLOC_PARAM __user *)ioctl_param);
        if (IMG_SUCCESS != imgRet)
        {
            CI_FATAL("Failed to deregister buffer (returned %d)\n",
                imgRet);
            ret = toErrno(imgRet);
        }
    }
    break;

    /*
    * Capture part
    */
    case CI_IOCTL_SET_YUV:
    {

		imgRet = INT_CI_PipelineSetYUV(pKrnConnection, ioctl_param);
		if (IMG_SUCCESS != imgRet)
		{
			CI_FATAL("Failed to Set YUV const =%d\n", ioctl_param);
			ret = toErrno(imgRet);
		}
    }
    break;
    case CI_IOCTL_CAPT_STA: //CI_PipelineStartCapture()
    {
        CI_DEBUG("ioctl 0x%X=CAPT_STA\n", ioctl_cmd);
        givenId = (int)ioctl_param;
        imgRet = INT_CI_PipelineStartCapture(pKrnConnection, givenId);
        if (IMG_SUCCESS != imgRet)
        {
            CI_FATAL("Failed to deregister capture id=%d\n", givenId);
            ret = toErrno(imgRet);
        }
    }
    break;

    case CI_IOCTL_CAPT_STP: //CI_PipelineStopCapture()
    {
        CI_DEBUG("ioctl 0x%X=CAPT_STP\n", ioctl_cmd);
        givenId = (int)ioctl_param;
        imgRet = INT_CI_PipelineStopCapture(pKrnConnection, givenId);
        if (IMG_SUCCESS != imgRet)
        {
            CI_FATAL("Failed to deregister capture id=%d\n", givenId);
            ret = toErrno(imgRet);
        }
    }
    break;

    case CI_IOCTL_CAPT_ISS: //CI_PipelineIsStarted()
    {
        IMG_BOOL8 bIsStarted = IMG_FALSE;
        CI_DEBUG("ioctl 0x%X=CAPT_ISSTARTED\n", ioctl_cmd);
        givenId = (int)ioctl_param;
        imgRet = INT_CI_PipelineIsStarted(pKrnConnection, givenId,
            &bIsStarted);
        if (IMG_SUCCESS != imgRet)
        {
            CI_FATAL("Failed to verify if capture id=%d is started\n",
                givenId);
            ret = toErrno(imgRet);
        }
        else
        {
            ret = bIsStarted;
        }
    }
    break;

    case CI_IOCTL_CAPT_TRG: //IMG_PipelineTriggerShoot()
    {
        CI_DEBUG("ioctl 0x%X=CI_IOCTL_CAPT_TRG\n", ioctl_cmd);
        imgRet = INT_CI_PipelineTriggerShoot(pKrnConnection,
            (struct CI_BUFFER_TRIGG __user *)ioctl_param);
        if (IMG_SUCCESS != imgRet)
        {
            CI_FATAL("Failed to trigger capture id=%d\n", givenId);
            ret = toErrno(imgRet);
        }
    }
    break;

    case CI_IOCTL_CAPT_BAQ: //IMG_PipelineAcquireBuffer()
    {
        CI_DEBUG("ioctl 0x%X=CAPT_BAQ\n", ioctl_cmd);
        imgRet = INT_CI_PipelineAcquireShot(pKrnConnection,
            (struct CI_BUFFER_PARAM __user *)ioctl_param);
        if (imgRet != IMG_SUCCESS)
        {
#ifndef FELIX_FAKE // otherwise polution of message
            if(imgRet != IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE) {
                // don't emit message if nonblocking shot acquisition
                CI_FATAL("Failed to acquire a buffer\n");
            }
#endif
            ret = toErrno(imgRet);
        }
    }
    break;

    case CI_IOCTL_CAPT_BRE: //CI_PipelineReleaseShot()
    {
        CI_DEBUG("ioctl 0x%X=CAPT_BRE\n", ioctl_cmd);
        imgRet = INT_CI_PipelineReleaseShot(pKrnConnection,
            (struct CI_BUFFER_TRIGG __user *)ioctl_param);
        if (IMG_SUCCESS != imgRet)
        {
            CI_FATAL("Failed to release a buffer\n");
            ret = toErrno(imgRet);
        }
    }
    break;

    case CI_IOCTL_PIPE_PEN: //CI_PipelineHasPending()
    {
        IMG_BOOL8 bResult = IMG_FALSE;
        CI_DEBUG("ioctl 0x%X=PIPE_PEN\n", ioctl_cmd);
        imgRet = INT_CI_PipelineHasPending(pKrnConnection,
            (int)ioctl_param, &bResult);
        if (IMG_SUCCESS != imgRet)
        {
            CI_FATAL("Failed to know if a capture is active\n");
            ret = toErrno(imgRet);
        }
        else
        {
            ret = (bResult == IMG_TRUE ? 1 : 0);
        }
    }
    break;

    case CI_IOCTL_PIPE_WAI: //CI_PipelineHasWaiting()
    {
        CI_DEBUG("ioctl 0x%X=PIPE_WAI\n", ioctl_cmd);
        ret = 0;
        imgRet = INT_CI_PipelineHasWaiting(pKrnConnection,
            (int)ioctl_param, &ret);
        if (IMG_SUCCESS != imgRet)
        {
            CI_FATAL("Failed to know if a capture is active\n");
            ret = toErrno(imgRet);
        }
    }
    break;

    case CI_IOCTL_PIPE_AVL: //CI_PipelineHasAvailableShots()
    {
        CI_DEBUG("ioctl 0x%X=PIPE_AVL\n", ioctl_cmd);
        imgRet = INT_CI_PipelineHasAvailable(pKrnConnection,
            (struct CI_HAS_AVAIL __user *)ioctl_param);
        if (IMG_SUCCESS != imgRet)
        {
            CI_FATAL("Failed to know if a capture is active\n");
            ret = toErrno(imgRet);
        }
    }
    break;

    /*
    * Gasket commands
    */
    case CI_IOCTL_GASK_ACQ: //CI_GasketAcquire()
    {
        CI_DEBUG("ioctl 0x%X=GASK_ACQ\n", ioctl_cmd);
        imgRet = INT_CI_GasketAcquire(pKrnConnection,
            (CI_GASKET __user*)ioctl_param);
        if (IMG_SUCCESS != imgRet)
        {
            CI_FATAL("Failed to acquire a gasket\n");
            ret = toErrno(imgRet);
        }
    }
    break;

    case CI_IOCTL_GASK_REL: //CI_GasketRelease()
    {
        CI_DEBUG("ioctl 0x%X=GASK_REL\n", ioctl_cmd);
        imgRet = INT_CI_GasketRelease(pKrnConnection,
            (IMG_UINT8)ioctl_param);
        if (IMG_SUCCESS != imgRet)
        {
            CI_FATAL("Failed to release a gasket\n");
            ret = toErrno(imgRet);
        }
    }
    break;

    case CI_IOCTL_GASK_NFO: //CI_GasketGetInfo()
    {
        CI_DEBUG("ioctl 0x%X=GASK_NFOL\n", ioctl_cmd);
        imgRet = INT_CI_GasketGetInfo(
            (struct CI_GASKET_PARAM __user *)ioctl_param);
        if (IMG_SUCCESS != imgRet)
        {
            CI_FATAL("Failed to get gasket information\n");
            ret = toErrno(imgRet);
        }
    }
    break;

    /*
     * Internal DG commands
     */
    case CI_IOCTL_INDG_REG: //CI_DatagenCreate() //CI_DatagenDestroy()
    {
        CI_DEBUG("ioctl 0x%X=INDG_REG\n", ioctl_cmd);
        imgRet = INT_CI_DatagenRegister(pKrnConnection,
            (int)ioctl_param, &ret);
        if (IMG_SUCCESS != imgRet)
        {
            CI_FATAL("Failed to register/deregister IIF DG object\n");
            ret = toErrno(imgRet);
        }
    }
    break;

    case CI_IOCTL_INDG_STA: //CI_DatagenStart()
    {
        CI_DEBUG("ioctl 0x%X=INDG_STA\n", ioctl_cmd);
        imgRet = INT_CI_DatagenStart(pKrnConnection,
            (const struct CI_DG_PARAM __user *)ioctl_param);
        if (IMG_SUCCESS != imgRet)
        {
            CI_FATAL("Failed to acquire IIF DG HW\n");
            ret = toErrno(imgRet);
        }
    }
    break;

    case CI_IOCTL_INDG_ISS: //CI_DatagenIsStarted()
    {
        CI_DEBUG("ioctl 0x%X=INDG_ISS\n", ioctl_cmd);
        imgRet = INT_CI_DatagenIsStarted(pKrnConnection,
            (int)ioctl_param, &ret);
        if (IMG_SUCCESS != imgRet)
        {
            CI_FATAL("Failed to acquire IIF DG HW\n");
            ret = toErrno(imgRet);
        }
    }
    break;

    case CI_IOCTL_INDG_STP: //CI_DatagenStop()
    {
        CI_DEBUG("ioctl 0x%X=INDG_STP\n", ioctl_cmd);
        imgRet = INT_CI_DatagenStop(pKrnConnection, (int)ioctl_param);
        if (IMG_SUCCESS != imgRet)
        {
            CI_FATAL("Failed to release IIF DG HW\n");
            ret = toErrno(imgRet);
        }
    }
    break;

    case CI_IOCTL_INDG_ALL: //CI_DatagenAllocateFrame()
    {
        CI_DEBUG("ioctl 0x%X=INDG_ALL\n", ioctl_cmd);
        imgRet = INT_CI_DatagenAllocate(pKrnConnection,
            (struct CI_DG_FRAMEINFO __user *)ioctl_param);
        if (IMG_SUCCESS != imgRet)
        {
            CI_FATAL("Failed to allocate a frame for IIF DG HW\n");
            ret = toErrno(imgRet);
        }
    }
    break;

    case CI_IOCTL_INDG_ACQ: //IMG_DatagenGetFrame()
    {
        CI_DEBUG("ioctl 0x%X=INDG_UPD\n", ioctl_cmd);
        imgRet = INT_CI_DatagenAcquireFrame(pKrnConnection,
            (struct CI_DG_FRAMEINFO __user *)ioctl_param);
        if (IMG_SUCCESS != imgRet)
        {
            CI_FATAL("Failed to update frame info for IIF DG HW\n");
            ret = toErrno(imgRet);
        }
    }
    break;

    case CI_IOCTL_INDG_TRG: //IMG_DatagenTrigger()
    {
        CI_DEBUG("ioctl 0x%X=INDG_TRG\n", ioctl_cmd);
        imgRet = INT_CI_DatagenTrigger(pKrnConnection,
            (const struct CI_DG_FRAMETRIG __user *)ioctl_param);
        if (IMG_SUCCESS != imgRet)
        {
            CI_FATAL("Failed to trigger frame capture for IIF DG HW\n");
            ret = toErrno(imgRet);
        }
    }
    break;

    case CI_IOCTL_INDG_FRE: //IMG_deleteFrame()
    {
        CI_DEBUG("ioctl 0x%X=INDG_FRE\n", ioctl_cmd);
        imgRet = INT_CI_DatagenFreeFrame(pKrnConnection,
            (const struct CI_DG_FRAMEINFO __user *)ioctl_param);
        if (IMG_SUCCESS != imgRet)
        {
            CI_FATAL("Failed to free a frame for IIF DG HW\n");
            ret = toErrno(imgRet);
        }
    }
    break;
	
#if defined(INFOTM_ISP)
    case CI_IOCTL_REG_GET:
        {
        	CI_DEBUG("ioctl 0x%X=REG_GET\n", ioctl_cmd);
            imgRet = INT_CI_RegGet(pKrnConnection, (struct CI_REG_PARAM __user *)ioctl_param);
            if ( IMG_SUCCESS != imgRet )
            {
                CI_FATAL("Failed to free a frame for IIF DG HW\n");
                ret = toErrno(imgRet);
            }
        }
        break;

    case CI_IOCTL_REG_SET:
        {
        	CI_DEBUG("ioctl 0x%X=REG_SET\n", ioctl_cmd);
            imgRet = INT_CI_RegSet(pKrnConnection, (const struct CI_REG_PARAM __user *)ioctl_param);
            if ( IMG_SUCCESS != imgRet )
            {
                CI_FATAL("Failed to free a frame for IIF DG HW\n");
                ret = toErrno(imgRet);
            }
        }
        break;

#endif //INFOTM_ISP

    default:
        CI_WARNING("Received unsupported ioctl 0x%X\n", ioctl_cmd);
        ret = -ENOTTY;
    } // IOCTL switch

    CI_DEBUG("ioctl 0x%X returns %d\n", ioctl_cmd, ret);
    return (long)ret;
}

struct SearchShot
{
    int mmapId;
    int buffer; // same than KRN_CI_SHOT::aMemMapIds
};

static IMG_BOOL8 List_SearchShot(void* listElem, void* lookingFor)
{
    KRN_CI_SHOT *pListShot = (KRN_CI_SHOT*)listElem;
    struct SearchShot *searching = (struct SearchShot*)lookingFor;

    // dynamic buffers should not be searched in the shot when mapping
    if (pListShot->hwSave.sMemory.uiAllocated > 0
        && pListShot->hwSave.ID == searching->mmapId)
    {
        searching->buffer = CI_BUFF_STAT;
        return IMG_FALSE; // stop searching
    }
    if (pListShot->sDPFWrite.sMemory.uiAllocated > 0
        && pListShot->sDPFWrite.ID == searching->mmapId)
    {
        searching->buffer = CI_BUFF_DPF_WR;
        return IMG_FALSE; // stop searching
    }
    if (pListShot->sENSOutput.sMemory.uiAllocated > 0
        && pListShot->sENSOutput.ID == searching->mmapId)
    {
        searching->buffer = CI_BUFF_ENS_OUT;
        return IMG_FALSE; // stop searching
    }

    return IMG_TRUE; // not found - continue searching
}

static IMG_BOOL8 ListSearch_KRN_DG_Buffer(void* listElem, void* lookingFor)
{
    KRN_CI_DGBUFFER *pBuff = (KRN_CI_DGBUFFER*)listElem;
    int *identifier = (int*)lookingFor;

    if (pBuff->ID == *identifier)
    {
        return IMG_FALSE;
    }
    return IMG_TRUE;
}

static IMG_RESULT IMG_CI_Memory_map(struct vm_area_struct *vma,
    SYS_MEM *psMem)
{
    if (psMem)
    {
        IMG_RESULT ret = IMG_SUCCESS;

        CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle,
            "mapping memory to user-space");

        /* we map just after allocation there should be no need to flush
         * caches yet */
        //Platform_MemUpdateHost(psMem);
        ret = Platform_MemMapUser(psMem, vma);

        CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "done");

        return ret;
    }

    return IMG_ERROR_FATAL;
}

static IMG_RESULT IMG_CI_Shot_map(struct vm_area_struct *vma,
    KRN_CI_SHOT *pShot, int buffer)
{
    SYS_MEM *psMem = NULL;
    IMG_RESULT ret = IMG_ERROR_FATAL;
    switch (buffer)
    {
        // dynamic buffers
    case CI_BUFF_ENC: // encoder
        pShot->phwEncoderOutput->type = CI_BUFF_ENC;
        psMem = &(pShot->phwEncoderOutput->sMemory);
        break;
    case CI_BUFF_DISP: // display
        pShot->phwDisplayOutput->type = CI_BUFF_DISP;
        psMem = &(pShot->phwDisplayOutput->sMemory);
        break;
    case CI_BUFF_HDR_WR: // HDR Ext
        pShot->phwHDRExtOutput->type = CI_BUFF_HDR_WR;
        psMem = &(pShot->phwHDRExtOutput->sMemory);
        break;
    case CI_BUFF_RAW2D: // Raw 2D Ext
        pShot->phwRaw2DExtOutput->type = CI_BUFF_RAW2D;
        psMem = &(pShot->phwRaw2DExtOutput->sMemory);
        break;

        // shot buffers
    case CI_BUFF_STAT: // stats
        pShot->hwSave.type = CI_BUFF_STAT;
        psMem = &(pShot->hwSave.sMemory);
        break;
    case CI_BUFF_DPF_WR: // DPF output
        pShot->sDPFWrite.type = CI_BUFF_DPF_WR;
        psMem = &(pShot->sDPFWrite.sMemory);
        break;
    case CI_BUFF_ENS_OUT: // ENS output
        pShot->sENSOutput.type = CI_BUFF_ENS_OUT;
        psMem = &(pShot->sENSOutput.sMemory);
        break;
    }
    IMG_ASSERT(psMem);

    if (psMem)
    {
        CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle,
            "map buffer to user-space");

        /* we map just after allocation so we do not need to flush the
         * cache just yet */
        //Platform_MemUpdateHost(psMem);
        ret = Platform_MemMapUser(psMem, vma);

        CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "done");
    }

    return ret;
}

int DEV_CI_Mmap(struct file *flip, struct vm_area_struct *vma)
{
    IMG_RESULT imgRet = IMG_SUCCESS;
    KRN_CI_CONNECTION *pKrnConnection = NULL;
    struct SearchShot searching;
    sCell_T *pFound = NULL;
    KRN_CI_SHOT *pShot = NULL; // only if it found a Shot
    KRN_CI_BUFFER *pBuffer = NULL; // only if it found a Buffer
    KRN_CI_DGBUFFER *pDGBuffer = NULL; // only if it found a DGBuffer

    searching.mmapId = (int)(vma->vm_pgoff);
    searching.buffer = -1;

    if (flip == NULL || vma == NULL || flip->private_data == NULL)
    {
        CI_FATAL("file structure or vm_area_struct is NULL or does not "\
            "have private data\n");
        return toErrno(IMG_ERROR_INVALID_PARAMETERS);
    }
    pKrnConnection = (KRN_CI_CONNECTION*)flip->private_data;

    CI_DEBUG("MemMap of offset %ld\n", vma->vm_pgoff);
    SYS_LockAcquire(&(pKrnConnection->sLock));
    {
        //searching.mmapId <<= PAGE_SHIFT;
        // search for SHOT mapping
        pFound = List_visitor(&(pKrnConnection->sList_unmappedShots),
            (void*)&searching, &List_SearchShot);
        if (pFound != NULL)
        {
            pShot = (KRN_CI_SHOT*)pFound->object;

            imgRet = IMG_CI_Shot_map(vma, pShot, searching.buffer);
            if (IMG_SUCCESS != imgRet)
            {
                CI_FATAL("IMG_CI_Shot_map failed!\n");
            }
        }
        else
        {
            // search for dynamic buffer
            pFound = List_visitor(&(pKrnConnection->sList_unmappedBuffers),
                (void*)&(searching.mmapId), &ListSearch_KRN_CI_Buffer);
            if (pFound != NULL)
            {
                pBuffer = (KRN_CI_BUFFER*)pFound->object;

                imgRet = IMG_CI_Memory_map(vma, &(pBuffer->sMemory));
                if (IMG_SUCCESS != imgRet)
                {
                    CI_FATAL("IMG_CI_Buffer_map failed!\n");
                }
            }
            else
            {
                // search for internal datagen buffer
                pFound = List_visitor(&(pKrnConnection->sList_unmappedDGBuffers),
                    (void*)&(searching.mmapId), &ListSearch_KRN_DG_Buffer);
                if (pFound != NULL)
                {
                    pDGBuffer = (KRN_CI_DGBUFFER*)pFound->object;

                    imgRet = IMG_CI_Memory_map(vma, &(pDGBuffer->sMemory));
                    if (IMG_SUCCESS != imgRet)
                    {
                        CI_FATAL("IMG_CI_DGBuffer_map failed!\n");
                    }
                    else
                    {
                        List_detach(pFound);

                        SYS_LockAcquire(&(pDGBuffer->pDatagen->slistLock));
                        {
                            imgRet = List_pushBack(
                                &(pDGBuffer->pDatagen->sList_available),
                                pFound);
                        }
                        SYS_LockRelease(&(pDGBuffer->pDatagen->slistLock));
                        IMG_ASSERT(imgRet == IMG_SUCCESS); // cannot fail
                    }
                }
            }
        }

        if (pFound == NULL)
        {
            CI_FATAL("Failed to find ID %d as either Shot or Buffer\n",
                searching.mmapId);
            imgRet = IMG_ERROR_FATAL;
        }
    }
    SYS_LockRelease(&(pKrnConnection->sLock));

    if (pShot)
    {
        /* the type of the buffer is only updated when mapped - when all are
         * updated the shot is considered ready
         * if a buffer is not allocated or mapped it is consider ready */
        int i = 0; // no buffer mapped

        if (pShot->hwSave.sMemory.uiAllocated == 0
            || pShot->hwSave.type != 0)
        {
            i++;
        }
        if (pShot->sDPFWrite.sMemory.uiAllocated == 0
            || pShot->sDPFWrite.type != 0)
        {
            i++;
        }
        if (pShot->sENSOutput.sMemory.uiAllocated == 0
            || pShot->sENSOutput.type != 0)
        {
            i++;
        }

        if (CI_POOL_MAP_ID_COUNT == i) // all buffers are ready
        {
            //printk("DEV_CI_Mmap: ShotTypp=%d, ID=%d, i=%d\n", pShot->eKrnShotType, pShot->iIdentifier, i);

            CI_DEBUG("Shot %d becomes available\n", pShot->iIdentifier);
            /* this is done after the search OUTSIDE the locking of
             * pKrnConnection->sLock because it will relock it to remove it */
            imgRet = KRN_CI_PipelineShotMapped(pShot->pPipeline, pShot);
            if (IMG_SUCCESS != imgRet)
            {
                CI_FATAL("Failed to make the shot available to the "\
                    "pipeline object\n");
            }
        }
    }
    if (pBuffer)
    {
        imgRet = KRN_CI_PipelineBufferMapped(pBuffer->pPipeline, pBuffer);
        if (IMG_SUCCESS != imgRet)
        {
            CI_FATAL("Failed to make the buffer available to the "\
                "pipeline object\n");
        }
    }

    return toErrno(imgRet);
}

IMG_RESULT DEV_CI_Suspend(SYS_DEVICE *pDev, pm_message_t state)
{
    /* note: state is one of these:
    PMSG_INVALID PMSG_ON PMSG_FREEZE PMSG_QUIESCE PMSG_SUSPEND PMSG_HIBERNATE
    PMSG_RESUME PMSG_THAW PMSG_RESTORE PMSG_RECOVER PMSG_USER_SUSPEND
    PMSG_USER_RESUME PMSG_REMOTE_RESUME PMSG_AUTO_SUSPEND PMSG_AUTO_RESUME
    */

    if (g_psCIDriver)
    {
        int i;
        CI_INFO("suspending...\n");

        //
        // the user-mod should be frozen
        //
        for (i = 0; i < CI_N_CONTEXT; i++)
        {
            KRN_CI_PIPELINE *pPipe = NULL;
            KRN_CI_DATAGEN *pDG = NULL;
#ifndef INFOTM_USE_SPIN_LOCK // added by linyun.xiong @2016-03-22
            SYS_LockAcquire(&(g_psCIDriver->sActiveContextLock));
#else
			SYS_SpinlockAcquire(&(g_psCIDriver->sActiveContextLock));
#endif
            {
                pPipe = g_psCIDriver->aActiveCapture[i];
                // because in the future may have 1 internal DG per context
                pDG = g_psCIDriver->pActiveDatagen;
            }
#ifndef INFOTM_USE_SPIN_LOCK // added by linyun.xiong @2016-03-22
            SYS_LockRelease(&(g_psCIDriver->sActiveContextLock));
#else
			SYS_SpinlockRelease(&(g_psCIDriver->sActiveContextLock));
#endif
            if (pDG)
            {
                CI_INFO("Stopping running internal datagen %d\n",
                    pDG->userDG.ui8IIFDGIndex);
                KRN_CI_DatagenStop(pDG);
            }

            if (pPipe)
            {
                CI_INFO("Stopping a running capture on context %d\n",
                    pPipe->userPipeline.ui8Context);
                KRN_CI_PipelineStopCapture(pPipe);
            }
        }

#ifdef FELIX_FAKE
        CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hRegFelixCore,
            "HW power OFF");
#else
        CI_DEBUG("HW power OFF");
#endif
        SYS_DevPowerControl(pDev, IMG_FALSE);
        return IMG_SUCCESS;
    }
    return IMG_ERROR_NOT_INITIALISED;
}

IMG_RESULT DEV_CI_Resume(SYS_DEVICE *pDev)
{
    if (g_psCIDriver)
    {
        int ret = 0;
        CI_INFO("resuming...\n");

#ifdef FELIX_FAKE
        CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hRegFelixCore,
            "HW power ON");
#else
        CI_DEBUG("HW power ON");
#endif
        ret = SYS_DevPowerControl(g_psCIDriver->pDevice, IMG_TRUE);
        if (IMG_SUCCESS != ret)
        {
            CI_FATAL("Failed to turn the power back on!\n");
            return ret;
        }

        HW_CI_DriverResetHW();

#ifdef IMG_SCB_DRIVER
        /*
         * This functionality is needed only when SCB driver is enabled.
         * Above function resets SCB block so it needs to be resumed here.
         */
        sys_dev_pci_resume();
#endif

        // start the clock
        HW_CI_DriverPowerManagement(PWR_AUTO);

        // configure timestamps
        HW_CI_DriverTimestampManagement();

        // load the current GMA LUT table
        HW_CI_Reg_GMA(&(g_psCIDriver->sGammaLUT));

        HW_CI_ReadCurrentDEPoint(g_psCIDriver,
            &(g_psCIDriver->eCurrentDataExtraction));

        if (g_psCIDriver->eCurrentDataExtraction >= CI_INOUT_NONE)
        {
            CI_WARNING("Unkown state for data extraction field!\n");
            g_psCIDriver->eCurrentDataExtraction = CI_INOUT_NONE;
        }

        // setup MMU because reset HW resets the MMU
        ret = KRN_CI_MMUConfigure(&(g_psCIDriver->sMMU),
            g_psCIDriver->sHWInfo.config_ui8NContexts,
            g_psCIDriver->sHWInfo.config_ui8NContexts,
            g_psCIDriver->sMMU.apDirectory[0] != NULL ? IMG_FALSE : IMG_TRUE);
        if (IMG_SUCCESS != ret)
        {
            int reg = 0;
            CI_FATAL("Failed to configure the HW MMU!\n");
            reg = 1 << FELIX_CORE_CORE_RESET_MMU_RESET_SHIFT;
            TALREG_WriteWord32(g_psCIDriver->sTalHandles.hRegFelixCore,
                FELIX_CORE_CORE_RESET_OFFSET, reg);
            return ret;
        }
        HW_CI_DriverTilingManagement();

        return IMG_SUCCESS;
    }
    return IMG_ERROR_NOT_INITIALISED;
}

