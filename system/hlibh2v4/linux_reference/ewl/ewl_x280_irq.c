/*------------------------------------------------------------------------------
--                                                                                                                               --
--       This software is confidential and proprietary and may be used                                   --
--        only as expressly authorized by a licensing agreement from                                     --
--                                                                                                                               --
--                            Verisilicon.                                                                                    --
--                                                                                                                               --
--                   (C) COPYRIGHT 2014 VERISILICON                                                            --
--                            ALL RIGHTS RESERVED                                                                    --
--                                                                                                                               --
--                 The entire notice above must be reproduced                                                 --
--                  on all copies and should not be removed.                                                    --
--                                                                                                                               --
--------------------------------------------------------------------------------
--
--  Description : Encoder Wrapper Layer (user space module)
--
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/

#include "base_type.h"
#include "ewl.h"
#include "hx280enc.h"   /* This EWL uses the kernel module */
#include "ewl_x280_common.h"
#include "ewl_linux_lock.h"

#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

#include <errno.h>

#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <pthread.h>

#include <semaphore.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*------------------------------------------------------------------------------
    2. External compiler flags
--------------------------------------------------------------------------------

ENC_MODULE_PATH     defines the path for encoder device driver nod.
                        e.g. "/tmp/dev/hx280"
MEMALLOC_MODULE_PATH defines the path for memalloc device driver nod.
                        e.g. "/tmp/dev/memalloc"
ENC_IO_BASE         defines the IO base address for encoder HW registers
                        e.g. 0xC0000000
SDRAM_LM_BASE       defines the base address for SDRAM as seen by HW
                        e.g. 0x80000000

--------------------------------------------------------------------------------
    3. Module defines
------------------------------------------------------------------------------*/

/*******************************************************************************
 Function name   : EWLReserveHw
 Description     : Reserve HW resource for currently running codec
*******************************************************************************/
i32 EWLReserveHw(const void *inst)
{
    hx280ewl_t *enc = (hx280ewl_t *) inst;

    assert(enc != NULL);

    PTRACE("EWLReserveHw: PID %d trying to reserve ...\n", getpid());

    /* Check invalid parameters */
    if(inst == NULL)
        return EWL_ERROR;

    if(binary_semaphore_wait(enc->semid, 0) != 0)
    {
        PTRACE("binary_semaphore_wait error: %s\n", strerror(errno));
        return EWL_ERROR;
    }

    PTRACE("Codec semaphore locked\n");

    /* we have to lock postprocessor also */
    if(binary_semaphore_wait(enc->semid, 1) != 0)
    {
        PTRACE("binary_semaphore_wait error: %s\n", strerror(errno));
        if(binary_semaphore_post(enc->semid, 0) != 0)
        {
            PTRACE("binary_semaphore_post error: %s\n", strerror(errno));
            assert(0);
        }
        return EWL_ERROR;
    }

    PTRACE("Post-processor semaphore locked\n");

    EWLWriteReg(inst, 0x14, 0);



    PTRACE("EWLReserveHw: ENC HW locked by PID %d\n", getpid());

    return EWL_OK;
}

/*******************************************************************************
 Function name   : EWLWaitHwRdy
 Description     : Wait for the encoder semaphore
 Return type     : i32 
 Argument        : void
*******************************************************************************/
i32 EWLWaitHwRdy(const void *inst, u32 *slicesReady,u32 totalsliceNumber,u32* status_register)
{
    hx280ewl_t *enc = (hx280ewl_t *) inst;
    i32 ret = EWL_HW_WAIT_OK;
    u32 prevSlicesReady = 0;

    PTRACE("EWLWaitHw: Start\n");
		
		if (slicesReady)
        prevSlicesReady = *slicesReady;
        
    /* Check invalid parameters */
    if(enc == NULL)
    {
        assert(0);
        return EWL_HW_WAIT_ERROR;
    }

    if (ioctl(enc->fd_enc, HX280ENC_IOCG_CORE_WAIT, &ret))
    {
        PTRACE("ioctl HX280ENC_IOCG_CORE_WAIT failed\n");
        ret = EWL_HW_WAIT_ERROR;
    }
    if (slicesReady)
       *slicesReady = (enc->pRegBase[7] >> 17) & 0xFF;
    *status_register = ret;
    PTRACE("EWLWaitHw: OK!\n");

    return EWL_OK;
}
